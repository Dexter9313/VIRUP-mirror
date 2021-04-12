#include <QApplication>
#include <QLibraryInfo>
#include <QSettings>
#include <QTranslator>
#ifdef Q_OS_UNIX
#include <QDir>
#include <unistd.h>
#endif

#include "Launcher.hpp"
#include "Logger.hpp"
#include "MainWin.hpp"

int main(int argc, char* argv[])
{
	if(argc == 2 && std::string(argv[1]) == "--version")
	{
		std::cout << PROJECT_NAME << " version " << PROJECT_VERSION
		          << std::endl;
		return EXIT_SUCCESS;
	}

	// setup logging
	Logger::init();

	// Set config file names for QSettings
	QCoreApplication::setOrganizationName(PROJECT_NAME);
	QCoreApplication::setApplicationName("config");

	QApplication a(argc, argv);

	// Set arguments
	QCommandLineParser parser;
	parser.addHelpOption();
	QCommandLineOption noLauncher(
	    "no-launcher", QCoreApplication::translate(
	                       "main", "By-pass launcher and launch application."));
	parser.addOption(noLauncher);
	QCommandLineOption config(
	    "config",
	    QCoreApplication::translate("main", "Read .ini config from <file>."),
	    "file");
	parser.addOption(config);
	QCommandLineOption version(
	    "version",
	    QCoreApplication::translate("main", "Display version information."));
	parser.addOption(version);
	parser.process(a);

	// set settings
	QSettings::setDefaultFormat(QSettings::IniFormat);
	if(parser.isSet(config))
	{
		QString configPath(parser.value(config));
		QFileInfo file(configPath);
		QDir dir(file.absoluteDir());
		QCoreApplication::setOrganizationName(dir.dirName());
		dir.cdUp();
		QCoreApplication::setApplicationName(file.baseName());
		QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
		                   dir.absolutePath());
	}

	// set translation
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

	if(!parser.isSet(noLauncher))
	{
		Launcher launcher;
		launcher.init();
		if(launcher.exec() == QDialog::Rejected)
		{
			return EXIT_SUCCESS;
		}
	}

	MainWin w;
	w.setTitle(PROJECT_NAME + QString(" - Loading..."));
	w.setFullscreen(QSettings().value("window/fullscreen").toBool());
	// start event loop
	QCoreApplication::postEvent(&w, new QEvent(QEvent::UpdateRequest));
	return QApplication::exec();

	// close log file
	Logger::close();
}
