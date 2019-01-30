#include "utils.hpp"

void initSettings()
{
	QSettings settings;

	settings.beginGroup("window");
	if(!settings.contains("width"))
		settings.setValue("width", 1500);
	if(!settings.contains("height"))
		settings.setValue("height", 800);
	if(!settings.contains("fullscreen"))
		settings.setValue("fullscreen", false);
	settings.endGroup();

	settings.beginGroup("vr");
	if(!settings.contains("enabled"))
		settings.setValue("enabled", true);
	settings.endGroup();

	settings.beginGroup("debugcamera");
	if(!settings.contains("enabled"))
		settings.setValue("enabled", false);
	if(!settings.contains("followhmd"))
		settings.setValue("followhmd", false);
	if(!settings.contains("debuginheadset"))
		settings.setValue("debuginheadset", false);
	settings.endGroup();

	settings.beginGroup("data");
	if(!settings.contains("gazfile"))
		settings.setValue("gazfile", QString(""));
	if(!settings.contains("starsfile"))
		settings.setValue("starsfile", QString(""));
	if(!settings.contains("darkmatterfile"))
		settings.setValue("darkmatterfile", QString(""));
	if(!settings.contains("loaddarkmatter"))
		settings.setValue("loaddarkmatter", true);
	if(!settings.contains("gazcolor"))
		settings.setValue("gazcolor", QVector3D(0.427450f, 0.592156f, 0.709803f));
	if(!settings.contains("starscolor"))
		settings.setValue("starscolor", QVector3D(1.0f, 1.0f, 0.0f));
	if(!settings.contains("darkmattercolor"))
		settings.setValue("darkmattercolor", QVector3D(1.f, 0.5f, 0.15f));
	settings.endGroup();

	settings.beginGroup("misc");
	if(!settings.contains("showcube"))
		settings.setValue("showcube", false);
	if(!settings.contains("cubecolor"))
		settings.setValue("cubecolor", QVector3D(1.f, 1.f, 1.f));
	if(!settings.contains("focuspoint"))
		settings.setValue("focuspoint", QVector3D(0.f, 0.f, 0.f));
	// focuspoint=-0.352592, -0.062213, 0.144314
	if(!settings.contains("maxvramusagemb"))
		settings.setValue("maxvramusagemb", 500);
	settings.endGroup();
}

