/*
    Copyright (C) 2020 Florian Cabot <florian.cabot@hotmail.fr>

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

#include "CSVObjects.hpp"

GLHandler::Texture& CSVObjects::starTex()
{
	static GLHandler::Texture starTex;
	return starTex;
}

GLHandler::Texture& CSVObjects::galTex()
{
	static GLHandler::Texture galTex;
	return galTex;
}

bool& CSVObjects::texLoaded()
{
	static bool texLoaded(false);
	return texLoaded;
}

CSVObjects::CSVObjects(QString const& csvFile, bool galaxies)
    : shader(galaxies ? "galaxies" : "stars")
    , galaxies(galaxies)
    , conShader("default")
{
	if(!texLoaded())
	{
		starTex() = GLHandler::newTexture(
		    getAbsoluteDataPath("images/star.png").toLatin1().data());
		galTex() = GLHandler::newTexture(
		    QSettings().value("data/sdssatlas").toString().toLatin1().data());
		GLHandler::generateMipmap(starTex());
		GLHandler::generateMipmap(galTex());
		texLoaded() = true;
	}
	QFile file(csvFile);
	if(file.open(QFile::ReadOnly))
	{
		QString line(file.readLine().data());

		// parse first line for columns numbers
		QList<QString> columns
		    = {"names",   "x",       "y",       "z",  "absmag",
		       "rabsmag", "gabsmag", "babsmag", "ci", "teff"};
		std::map<QString, int> columnsNumbers;

		for(int i(0); i < line.split(",").size(); ++i)
		{
			QString str(line.split(",")[i]);
			if(columns.contains(str.simplified()))
			{
				columnsNumbers[str.simplified()] = i;
			}
		}

		line = file.readLine().data();
		unsigned int i(0);
		while(line != "")
		{
			++i;
			objects.push_back(parseLine(line, columnsNumbers));
			for(auto const& starName : objects[objects.size() - 1].names)
			{
				indexByName[starName.name] = objects.size() - 1;
			}
			line = file.readLine().data();
		}
		file.close();
	}

	std::vector<float> vertices;
	for(auto object : objects)
	{
		vertices.push_back(object.x);
		vertices.push_back(object.y);
		vertices.push_back(object.z);
		vertices.push_back(object.absmag);
		vertices.push_back(object.color.redF());
		vertices.push_back(object.color.greenF());
		vertices.push_back(object.color.blueF());
	}

	mesh.setVertices(vertices, shader,
	                 {{"position", 3}, {"absmag", 1}, {"color", 3}});
}

CSVObjects::CSVObjects(QString const& csvFile,
                       QString const& constellationsFile)
    : CSVObjects(csvFile)
{
	containsConstellations = true;
	conShader.setUniform("color", QColor(255, 0, 0));

	std::vector<float> vertices;
	std::vector<unsigned int> elements;

	QFile file(constellationsFile);
	if(file.open(QFile::ReadOnly))
	{
		QString line(file.readLine().data());
		QString currentName = "";
		Vector3 currentPosSum;
		unsigned int posNumber = 0;
		while(line != "")
		{
			if(line[0] != '\t')
			{
				if(posNumber > 0)
				{
					// average position
					currentPosSum /= posNumber;
					auto labelText = new LabelRenderer(
					    currentName, QColor(255, 0, 0), false);
					conLabels.emplace_back(currentPosSum, labelText);
				}
				currentName   = line.simplified();
				currentPosSum = Vector3();
				posNumber     = 0;
				line          = file.readLine().data();
				continue;
			}

			auto starsNames = line.simplified().split(',');

			for(auto const& starName : starsNames)
			{
				int index(-1);
				if(indexByName.count(starName) > 0)
				{
					index = indexByName[starName];
				}

				if(index == -1)
				{
					// print not found stars
					// qDebug() << starName;
					continue;
				}

				Object object(objects[index]);
				elements.push_back(vertices.size() / 3);
				vertices.push_back(object.x);
				vertices.push_back(object.y);
				vertices.push_back(object.z);

				currentPosSum += Vector3(object.x, object.y, object.z);
				++posNumber;
			}
			elements.push_back(0xFFFF);

			line = file.readLine().data();
		}
		if(posNumber > 0)
		{
			// average position
			currentPosSum /= posNumber;
			auto labelText
			    = new LabelRenderer(currentName, QColor(255, 0, 0), false);
			conLabels.emplace_back(currentPosSum, labelText);
		}
		file.close();
	}

	conMesh.setVertices(vertices, conShader, {{"position", 3}}, elements);
}

BBox CSVObjects::getBoundingBox() const
{
	// TODO compute
	return {};
}

void CSVObjects::render(Camera const& camera, ToneMappingModel const* tmm)
{
	QMatrix4x4 model;
	QVector3D campos;
	getModelAndCampos(camera, model, campos);

	GLHandler::glf().glEnable(GL_CLIP_DISTANCE0);
	GLHandler::glf().glEnable(GL_POINT_SPRITE);
	GLHandler::glf().glEnable(GL_PROGRAM_POINT_SIZE);
	GLHandler::beginTransparent(GL_ONE, GL_ONE);
	shader.setUniform("pixelSolidAngle", camera.pixelSolidAngle());
	shader.setUniform("brightnessMultiplier", brightnessMultiplier);
	shader.setUniform("campos", campos);
	if(galaxies)
	{
		shader.setUniform("atlassize", QVector2D(47, 10));
		shader.setUniform("colormix", colormix);
	}
	else
	{
		shader.setUniform("camexp", tmm->exposure);
		shader.setUniform("camdynrange", tmm->dynamicrange);
	}
	GLHandler::useTextures({galaxies ? galTex() : starTex()});
	GLHandler::setUpRender(shader, model);
	mesh.render();
	GLHandler::endTransparent();
	GLHandler::glf().glDisable(GL_PROGRAM_POINT_SIZE);
	GLHandler::glf().glDisable(GL_POINT_SPRITE);
	GLHandler::glf().glDisable(GL_CLIP_DISTANCE0);

	if(containsConstellations)
	{
		GLHandler::glf().glEnable(GL_MULTISAMPLE);
		GLHandler::beginTransparent(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		GLHandler::glf().glEnable(GL_LINE_SMOOTH);
		GLHandler::glf().glLineWidth(2.f);
		GLHandler::glf().glEnable(GL_PRIMITIVE_RESTART);
		GLHandler::glf().glPrimitiveRestartIndex(0xFFFF);

		conShader.setUniform("alpha", constellationsAlpha);
		conShader.setUniform("exposure", tmm->exposure);
		conShader.setUniform("dynamicrange", tmm->dynamicrange);

		GLHandler::setUpRender(conShader, model);
		conMesh.render(PrimitiveType::LINE_STRIP);

		GLHandler::glf().glDisable(GL_PRIMITIVE_RESTART);
		GLHandler::glf().glLineWidth(1.f);
		GLHandler::glf().glDisable(GL_LINE_SMOOTH);
		GLHandler::endTransparent();

		if(constellationsLabels > 0.f)
		{
			for(auto conLabel : conLabels)
			{
				Vector3 pos(conLabel.first);
				Vector3 camRelPos(Utils::fromQt(campos) - pos);
				Vector3 unitRelPos(camRelPos.getUnitForm());

				// orient label towards camera and place before 8000
				float yaw(atan2(unitRelPos[1], unitRelPos[0]));
				float pitch(-1.0 * asin(unitRelPos[2]));
				double rescale(camRelPos.length() <= 8000.0
				                   ? 1.0
				                   : 8000.0 / camRelPos.length());

				QMatrix4x4 model2;
				model2.translate(rescale * Utils::toQt(pos));
				model2.rotate(yaw * 180.f / M_PI + 90.f, 0.0, 0.0, 1.0);
				model2.rotate(pitch * 180.f / M_PI + 90.f, 1.0, 0.0, 0.0);
				model2.scale(rescale * camRelPos.length() / 3.0);
				conLabel.second->updateModel(model * model2);

				conLabel.second->setAlpha(constellationsLabels);
				conLabel.second->render(tmm->exposure, tmm->dynamicrange);
			}
		}
		GLHandler::glf().glDisable(GL_MULTISAMPLE);
	}
}

QColor CSVObjects::colorFromColorIndex(float ci)
{
	// https://stackoverflow.com/questions/21977786/star-b-v-color-index-to-apparent-rgb-color
	float temperature
	    = 4600
	      * ((1.f / ((0.92f * ci) + 1.7f)) + (1.f / ((0.92f * ci) + 0.62f)));

	return GLHandler::sRGBToLinear(
	    Utils::toQt(blackbody::colorFromTemperature(temperature)));
}

CSVObjects::Object
    CSVObjects::parseLine(QString const& line,
                          std::map<QString, int> const& columnsNumbers)
{
	Object result;
	QStringList splitted(line.split(","));

	try
	{
		QString names = splitted[columnsNumbers.at("names")].simplified();
		if(names != "")
		{
			QStringList namesList = names.split('|');
			for(auto const& richName : namesList)
			{
				QStringList splittedName = richName.split(':');
				QString name(splittedName[0]);
				Designation desig = Designation::UNKNOWN;
				if(splittedName.size() > 1)
				{
					QString desigStr(splittedName[1]);
					if(desigStr == "proper")
					{
						desig = Designation::PROPER;
					}
					else if(desigStr == "bayer")
					{
						desig = Designation::BAYER;
					}
					else if(desigStr == "flam")
					{
						desig = Designation::FLAMSTEED;
					}
				}
				result.names.push_back({desig, name});
			}
		}
	}
	catch(...)
	{
	}

	result.x = splitted[columnsNumbers.at("x")].simplified().toDouble();
	result.y = splitted[columnsNumbers.at("y")].simplified().toDouble();
	result.z = splitted[columnsNumbers.at("z")].simplified().toDouble();

	try
	{
		result.absmag
		    = splitted[columnsNumbers.at("absmag")].simplified().toDouble();
		try
		{
			result.color = colorFromColorIndex(
			    splitted[columnsNumbers.at("ci")].simplified().toDouble());
		}
		catch(...)
		{
			result.color = GLHandler::sRGBToLinear(
			    Utils::toQt(blackbody::colorFromTemperature(
			        splitted[columnsNumbers.at("teff")]
			            .simplified()
			            .toDouble())));
		}
	}
	catch(...)
	{
		double rabsmag
		    = splitted[columnsNumbers.at("rabsmag")].simplified().toDouble();
		double gabsmag
		    = splitted[columnsNumbers.at("gabsmag")].simplified().toDouble();
		double babsmag
		    = splitted[columnsNumbers.at("babsmag")].simplified().toDouble();

		// invalid magnitudes
		if(rabsmag < -90.0)
		{
			rabsmag *= -1.0;
		}
		if(gabsmag < -90.0)
		{
			gabsmag *= -1.0;
		}
		if(babsmag < -90.0)
		{
			babsmag *= -1.0;
		}

		double r      = Color::illuminanceFromMag(rabsmag);
		double g      = Color::illuminanceFromMag(gabsmag);
		double b      = Color::illuminanceFromMag(babsmag);
		result.absmag = Color::magFromIlluminance(r + g + b);

		auto xyY     = Color::rgbtoxyY(r, g, b);
		auto rgb     = Color::xyYtorgb(xyY[0], xyY[1], 1.0);
		result.color = QColor::fromRgbF(rgb[0], rgb[1], rgb[2], 1.0);
	}

	return result;
}

CSVObjects::~CSVObjects()
{
	if(texLoaded())
	{
		GLHandler::deleteTexture(starTex());
		GLHandler::deleteTexture(galTex());
		texLoaded() = false;
	}

	if(containsConstellations)
	{
		for(auto conLabel : conLabels)
		{
			delete conLabel.second;
		}
	}
}

QString CSVObjects::desigToStr(Designation desig)
{
	switch(desig)
	{
		case Designation::UNKNOWN:
			return QObject::tr("Unknown");
		case Designation::PROPER:
			return QObject::tr("Proper name");
		case Designation::BAYER:
			return QObject::tr("Bayer");
		case Designation::FLAMSTEED:
			return QObject::tr("Flamsteed");
	}
	return "";
}
