#include <QApplication>
#include <QLibraryInfo>
#include <QSettings>
#include <QTranslator>
#ifdef Q_OS_UNIX
#include <QDir>
#include <unistd.h>
#endif

#include "Launcher.hpp"
#include "MainWin.hpp"

void log(QtMsgType type, const QMessageLogContext& context, const QString& msg);

int main(int argc, char* argv[])
{
	// setup logging
	qInstallMessageHandler(log);

	// Set config file names for QSettings
	QCoreApplication::setOrganizationName(PROJECT_NAME);
	QCoreApplication::setApplicationName("config");
	QSettings::setDefaultFormat(QSettings::IniFormat);

	QApplication a(argc, argv);

	QString localeName(QSettings()
	                       .value("window/language", QLocale::system().name())
	                       .toString());
	QTranslator qtTranslator;
	qtTranslator.load("qt_" + localeName,
	                  QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	QCoreApplication::installTranslator(&qtTranslator);

	QTranslator hvrTranslator;
	hvrTranslator.load("HydrogenVR_" + localeName, "data/translations/");
	QCoreApplication::installTranslator(&hvrTranslator);

	QTranslator programTranslator;
	programTranslator.load(QString(PROJECT_NAME) + "_" + localeName,
	                       "data/translations/");
	QCoreApplication::installTranslator(&programTranslator);

#ifdef Q_OS_UNIX
// set current dir as application dir path to avoid reading coincidental
// data/core that doesn't belong to it
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
	chdir(QCoreApplication::applicationDirPath().toLocal8Bit().data());
#pragma GCC diagnostic pop
	if(!QDir("./data/core").exists())
	{
		// if no data/core in application dir, search in
		// INSTALL_PREFIX/share/PROJECT_NAME
		QString path(QString(INSTALL_PREFIX) + "/share/" + PROJECT_NAME);
		if(!QDir(path).exists())
		{
			// else let it be /usr/share/PROJECT_NAME
			path = QString("/usr/share/") + PROJECT_NAME;
		}
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
		chdir((path.toLocal8Bit().data()));
#pragma GCC diagnostic pop
	}
#endif

	{
		Launcher launcher;
		launcher.init();
		if(launcher.exec() == QDialog::Rejected)
		{
			return 1;
		}
	}

	MainWin w;
	w.setFullscreen(QSettings().value("window/fullscreen").toBool());
	// start event loop
	QCoreApplication::postEvent(&w, new QEvent(QEvent::UpdateRequest));
	return QApplication::exec();
}

void log(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	QByteArray localMsg   = msg.toLocal8Bit();
	const char* file      = context.file != nullptr ? context.file : "";
	const char* shortFile = file;
	if(strlen(shortFile) > strlen(BUILD_SRC_DIR) + 1)
	{
		shortFile += strlen(BUILD_SRC_DIR) + 1;
	}
	const char* function = context.function != nullptr ? context.function : "";

	std::string messageTypeStr;
	switch(type)
	{
		case QtDebugMsg:
			messageTypeStr = "Debug";
			break;
		case QtInfoMsg:
			messageTypeStr = "Info";
			break;
		case QtWarningMsg:
			messageTypeStr = "Warning";
			break;
		case QtCriticalMsg:
			messageTypeStr = "Critical";
			break;
		case QtFatalMsg:
			messageTypeStr = "Fatal";
			break;
	}
	std::cerr << messageTypeStr << " (" << shortFile << ":" << context.line
	          << ", " << function << "):" << std::endl
	          << "\t" << localMsg.constData() << std::endl;
}
