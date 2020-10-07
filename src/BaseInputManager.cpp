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
	addAction(Qt::Key_F1, {"toggledbgcam", tr("Toggle Debug Camera")}, true);
	addAction(Qt::Key_F2, {"togglewireframe", tr("Toggle Wireframe Mode")},
	          true);
	addAction(Qt::Key_F8, {"togglepyconsole", tr("Toggle Python Console")},
	          true);
	addAction(Qt::Key_F10, {"screenshot", tr("Take Screenshot")}, true);
	addAction(Qt::Key_F11, {"togglevr", tr("Toggle Virtual Reality")}, true);
	addAction(Qt::ALT + Qt::Key_Return,
	          {"togglefullscreen", tr("Toggle Fullscreen")}, true);
	addAction(Qt::Key_E, {"autoexposure", tr("Toggle Automatic Exposure")},
	          true);
	addAction(Qt::Key_PageUp, {"exposureup", tr("Increase Exposure")}, true);
	addAction(Qt::Key_PageDown, {"exposuredown", tr("Decrease Exposure")},
	          true);
	addAction(Qt::Key_Insert, {"dynamicrangeup", tr("Increase Dynamic Range")},
	          true);
	addAction(Qt::Key_Delete,
	          {"dynamicrangedown", tr("Decrease Dynamic Range")}, true);
	addAction(Qt::Key_Escape, {"quit", tr("Quit")}, true);
}

// NOLINTNEXTLINE(fuchsia-overloaded-operator)
BaseInputManager::Action BaseInputManager::
    operator[](QKeySequence const& key) const
{
	return mapping[key.toString()];
}

void BaseInputManager::addAction(QKeySequence const& defaultKey, Action action)
{
	addAction(defaultKey, std::move(action), false);
}

void BaseInputManager::addAction(QKeySequence const& defaultKey, Action action,
                                 bool engine)
{
	QKeySequence dKey = QSettings()
	                        .value("controls/" + action.id, defaultKey)
	                        .value<QKeySequence>();
	mapping[dKey.toString()] = std::move(action);
	if(engine)
	{
		orderedEngineKeys.push_back(dKey.toString());
	}
	else
	{
		orderedProgramKeys.push_back(dKey.toString());
	}
}
