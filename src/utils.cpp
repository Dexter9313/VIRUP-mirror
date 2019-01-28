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
}

