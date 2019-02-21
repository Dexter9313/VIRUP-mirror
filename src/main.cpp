#include <QApplication>
#include <QProcess>
#include <QSettings>
#ifdef Q_OS_UNIX
#include <QDir>
#include <unistd.h>
#endif

#include <PythonQt.h>
#include <gui/PythonQtScriptingConsole.h>

#include "Launcher.hpp"
#include "MainWin.hpp"

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

	QApplication a(argc, argv);

	// init PythonQt and Python
	PythonQt::init(PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut);

	// get the __main__ python module
	PythonQtObjectPtr mainModule = PythonQt::self()->getMainModule();

	QProcess process;
	process.start("python3 -c \"import sys;print(sys.path)\"");
	process.waitForFinished(-1);
	QString syspath(process.readAllStandardOutput());
	mainModule.evalScript("import sys\nsys.path=" + syspath + "\ndel sys");

	{
		Launcher launcher;
		launcher.init();
		if(launcher.exec() == QDialog::Rejected)
			return 1;
	}

	MainWin w;
	PythonQtScriptingConsole console(nullptr, mainModule);
	console.show();
	mainModule.addObject("mainwin", &w);

	if(QSettings().value("window/fullscreen").toBool())
		w.showFullScreen();
	else
		w.show();
	return a.exec();
}
