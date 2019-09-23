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

int main(int argc, char* argv[])
{
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
	chdir(QCoreApplication::applicationDirPath().toLocal8Bit().data());
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
		chdir((path.toLocal8Bit().data()));
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
	return QApplication::exec();
}
