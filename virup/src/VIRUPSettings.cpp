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
	addColorSetting("gazcolor",
	                QColor::fromRgbF(0.427450f, 0.592156f, 0.709803f),
	                tr("Gaz Color"));
	addColorSetting("starscolor", QColor::fromRgbF(1.f, 1.f, 0.f),
	                tr("Stars Color"));
	addColorSetting("darkmattercolor", QColor::fromRgbF(1.f, 0.5f, 0.15f),
	                tr("Dark Matter Color"));
	addFilePathSetting("cosmolabelsfile", QString(""),
	                   tr("Cosmological Labels File"));

	insertGroup("simulation", tr("Simulation"), 1);
	addDateTimeSetting("starttime", QDateTime::currentDateTimeUtc(),
	                   tr("Start time (UTC)"));
	addBoolSetting("lockedrealtime", false, tr("Lock to Real Time"));

	addDirPathSetting(
	    "solarsystemdir",
	    QFileInfo(QSettings().fileName()).absoluteDir().absolutePath()
	        + "/systems/",
	    tr("Solar System Root Directory"));

	addDirPathSetting(
	    "planetsystemdir",
	    QFileInfo(QSettings().fileName()).absoluteDir().absolutePath()
	        + "/systems/",
	    tr("Exoplanetary Systems Root Directory"));

	insertGroup("quality", tr("Quality"), 2);
	addUIntSetting("texmaxsize", 16, tr("Textures max size (x512)"), 1, 32);
	addUIntSetting("planetquality", 4, tr("Planet rendering quality"), 1, 6);
	addUIntSetting("atmoquality", 6, tr("Atmosphere rendering quality"), 1, 8);

	insertGroup("misc", tr("Miscellaneous"), 3);
	addBoolSetting("showgrid", false, tr("Show Grid"));
	addColorSetting("gridcolor", QColor(255, 255, 255), tr("Grid Color"));
	addVector3DSetting("focuspoint", QVector3D(), tr("Focus Point"),
	                   {{tr("x"), tr("y"), tr("z")}}, -1000, 1000);
	// focuspoint=-0.352592, -0.062213, 0.144314
	addUIntSetting("maxvramusagemb", 500, tr("Max VRAM Usage (in Mb)"), 0,
	               1000000);

	setCurrentIndex(0);
}
