#include <QApplication>
#include <QSettings>
#ifdef Q_OS_UNIX
#include <QDir>
#include <unistd.h>
#endif

#include "MainWin.hpp"
#include "utils.hpp"

int main(int argc, char* argv[])
{
#ifdef Q_OS_UNIX
	if(!QDir("./data/shaders").exists())
		chdir((QString("/usr/share/") + PROJECT_NAME).toLocal8Bit().data());
#endif
	// Set config file names for QSettings
	QCoreApplication::setOrganizationName(PROJECT_NAME);
	QCoreApplication::setApplicationName("config");
	QSettings::setDefaultFormat(QSettings::IniFormat);
	initSettings();

	QApplication a(argc, argv);
	MainWin w;
	if(QSettings().value("window/fullscreen").toBool())
		w.showFullScreen();
	else
		w.show();

	return a.exec();
}
