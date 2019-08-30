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

#include "BaseInputManager.hpp"

BaseInputManager::BaseInputManager()
{
	addAction(Qt::Key_F1, {"toggledbgcam", tr("Toggle Debug Camera")});
	addAction(Qt::Key_F2, {"togglewireframe", tr("Toggle Wireframe Mode")});
	addAction(Qt::Key_F8, {"togglepyconsole", tr("Toggle Python Console")});
	addAction(Qt::Key_F11, {"togglevr", tr("Toggle Virtual Reality")});
	addAction(Qt::ALT + Qt::Key_Return,
	          {"togglefullscreen", tr("Toggle Fullscreen")});
	addAction(Qt::Key_Escape, {"quit", tr("Quit")});
}

// NOLINTNEXTLINE(fuchsia-overloaded-operator)
BaseInputManager::Action BaseInputManager::
    operator[](QKeySequence const& key) const
{
	return mapping[key.toString()];
}

void BaseInputManager::addAction(QKeySequence const& defaultKey, Action action)
{
	QKeySequence dKey = QSettings()
	                        .value("controls/" + action.id, defaultKey)
	                        .value<QKeySequence>();
	mapping[dKey.toString()] = std::move(action);
}
