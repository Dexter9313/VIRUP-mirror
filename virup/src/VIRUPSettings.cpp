/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@epfl.ch>

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

#include "VIRUPSettings.hpp"

VIRUPSettings::VIRUPSettings(QWidget* parent)
	: SettingsWidget(parent)
{
	insertGroup("data", tr("Data"), 0);
	addFilePathSetting("gazfile", QString(""), tr("Gaz File"));
	addFilePathSetting("starsfile", QString(""), tr("Stars File"));
	addFilePathSetting("darkmatterfile", QString(""), tr("Dark Matter File"));
	addBoolSetting("loaddarkmatter", true, tr("Load Dark Matter"));
	addColorSetting("gazcolor", QColor::fromRgbF(0.427450f, 0.592156f, 0.709803f), tr("Gaz Color"));
	addColorSetting("starscolor", QColor::fromRgbF(1.f, 1.f, 0.f), tr("Stars Color"));
	addColorSetting("darkmattercolor", QColor::fromRgbF(1.f, 0.5f, 0.15f), tr("Dark Matter Color"));

	insertGroup("misc", tr("Miscellaneous"), 1);
	addBoolSetting("showcube", false, tr("Show Cube"));
	addColorSetting("cubecolor", QColor(255, 255, 255), tr("Cube Color"));
	addVector3DSetting("focuspoint", QVector3D(), tr("Focus Point"), {{tr("x"), tr("y"), tr("z")}}, -1000, 1000);
	// focuspoint=-0.352592, -0.062213, 0.144314
	addUIntSetting("maxvramusagemb", 500, tr("Max VRAM Usage (in Mb)"), 0, 1000000);

	setCurrentIndex(0);
}
