/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@hotmail.fr>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef PYTHONQTHANDLER_H
#define PYTHONQTHANDLER_H

#ifdef PYTHONQT
#include <PythonQt.h>
#ifdef PYTHONQT_QTALL
#include <PythonQt_QtAll.h>
#endif
#include <gui/PythonQtScriptingConsole.h>
#endif

#include <QObject>
#include <QProcess>
#include <QString>
#include <QVariant>

class PythonQtHandler
{
  public:
	PythonQtHandler() = delete;
	static bool isSupported();
	static void init();
	template <typename T>
	static void addClass(QString const& name, QString const& package = "");
	static void addVariable(QString const& name, QVariant const& v);
	static QVariant getVariable(QString const& name);
	static void removeVariable(QString const& name);
	static void addObject(QString const& name, QObject* object);
	static QVariant evalScript(QString const& script);
	static void evalFile(QString const& filename);
	static void openConsole();
	static void toggleConsole();
	static void closeConsole();
	static void clean();

  private:
#ifdef PYTHONQT
	static PythonQtObjectPtr* mainModule;
	static PythonQtScriptingConsole* console;
#endif
};

template <typename T>
void PythonQtHandler::addClass(QString const& name, QString const& package)
{
#ifdef PYTHONQT
	qRegisterMetaType<T>(name.toLatin1().constData());
	PythonQt::self()->registerCPPClass(name.toLatin1().constData(), "",
	                                   package.toLatin1().constData());
#endif
}

#endif // PYTHONQTHANDLER_H
