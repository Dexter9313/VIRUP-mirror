#include <QApplication>
#include <QSettings>
#ifdef Q_OS_UNIX
#include <QDir>
#include <unistd.h>
#endif

#include "Launcher.hpp"
#include "MainWin.hpp"

int main(int argc, char* argv[])
{
#ifdef Q_OS_UNIX
	if(!QDir("./data/shaders").exists())
	{
		chdir((QString("/usr/share/") + PROJECT_NAME).toLocal8Bit().data());
	}
#endif
	// Set config file names for QSettings
	QCoreApplication::setOrganizationName(PROJECT_NAME);
	QCoreApplication::setApplicationName("config");
	QSettings::setDefaultFormat(QSettings::IniFormat);

	QApplication a(argc, argv);

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
