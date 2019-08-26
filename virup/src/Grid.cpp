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

#include "Grid.hpp"

Grid::Grid()
{
	shader = GLHandler::newShader("default");
	GLHandler::setShaderParam(shader, "alpha", 0.5f);
	GLHandler::setShaderParam(
	    shader, "color", QSettings().value("misc/gridcolor").value<QColor>());

	mesh = GLHandler::newMesh();
	std::vector<float> vertices;
	std::vector<unsigned int> elements;
	generateGridVertices(vertices, elements);
	GLHandler::setVertices(mesh, vertices, shader, {{"position", 3}}, elements);

	scaleText3D.setColor(QColor(0, 255, 0));

	PythonQtHandler::addObject("Grid", this);
}

void Grid::setColor(QColor const& color)
{
	QSettings().setValue("misc/gridcolor", color);
	GLHandler::setShaderParam(shader, "color", color);
}

void Grid::renderCosmo(Camera const& cam)
{
	QMatrix4x4 model(cam.dataToWorldTransform());

	double camScale(cam.scale);
	camScale = pow(10, round(log10(camScale)));

	Vector3 camPos(cam.worldToDataPosition(Utils::fromQt(
	    cam.hmdSpaceToWorldTransform() * QVector3D(0.f, 0.f, 0.f))));
	camPos[0] = customRound(camPos[0], 0.1 / camScale);
	camPos[1] = customRound(camPos[1], 0.1 / camScale);
	camPos[2] = 0.0;

	QMatrix4x4 t;
	t.translate(Utils::toQt(camPos));
	t.scale(1.0 / camScale);

	GLHandler::setUpRender(shader, model * t);
	GLHandler::render(mesh, GLHandler::PrimitiveType::LINES);

	QString newScaleText(generateScaleTextCosmo(camScale));
	if(newScaleText != scaleText)
	{
		scaleText = newScaleText;
		scaleText3D.setText(scaleText);
	}

	QMatrix4x4 t2;
	t2.translate(0.5, 0.3, 0.0);
	scaleText3D.getModel() = model * t * t2;
	scaleText3D.render();
}

void Grid::renderPlanet(OrbitalSystemCamera const& cam)
{
	double camScale(CelestialBodyRenderer::overridenScale);

	QMatrix4x4 dataToWorld;
	dataToWorld.scale(camScale);
	dataToWorld.translate(Utils::toQt(-1.0 * cam.relativePosition));

	QMatrix4x4 model(dataToWorld);

	camScale = pow(10, round(log10(camScale)));

	Vector3 camPos(
	    Utils::fromQt(dataToWorld.inverted() * cam.hmdSpaceToWorldTransform()
	                  * QVector3D(0.f, 0.f, 0.f)));
	camPos[0] = customRound(camPos[0], 0.1 / camScale);
	camPos[1] = customRound(camPos[1], 0.1 / camScale);
	camPos[2] = 0.0;

	QMatrix4x4 t;
	t.translate(Utils::toQt(camPos));
	t.scale(1.0 / camScale);

	GLHandler::setUpRender(shader, model * t);
	GLHandler::render(mesh, GLHandler::PrimitiveType::LINES);

	QString newScaleText(generateScaleTextPlanet(camScale));
	if(newScaleText != scaleText)
	{
		scaleText = newScaleText;
		scaleText3D.setText(scaleText);
	}

	QMatrix4x4 t2;
	t2.translate(0.5, 0.3, 0.0);
	scaleText3D.getModel() = model * t * t2;
	scaleText3D.render();
}

Grid::~Grid()
{
	GLHandler::deleteShader(shader);
	GLHandler::deleteMesh(mesh);
}

void Grid::generateGridVertices(std::vector<float>& vertices,
                                std::vector<unsigned int>& elements)
{
	const unsigned int subdivisions(20);

	// commented cube version

	/*for(unsigned int i(0); i <= subdivisions; ++i)
	{
	    for(unsigned int j(0); j <= subdivisions; ++j)
	    {
	        vertices.push_back((2.f * i) / subdivisions - 1.f);
	        vertices.push_back((2.f * j) / subdivisions - 1.f);
	        // vertices.push_back(-1.f);
	        vertices.push_back(0.f);

	        vertices.push_back((2.f * i) / subdivisions - 1.f);
	        vertices.push_back((2.f * j) / subdivisions - 1.f);
	        // vertices.push_back(1.f);
	        vertices.push_back(0.f);

	        elements.push_back(2 * (j * (subdivisions + 1) + i));
	        elements.push_back(2 * (j * (subdivisions + 1) + i) + 1);
	    }
	}

	unsigned int elemShift(2 * (subdivisions + 1) * (subdivisions + 1));
*/
	unsigned int elemShift(0);
	for(unsigned int i(0); i <= subdivisions; ++i)
	{
		for(unsigned int j(0); j <= subdivisions; ++j)
		{
			vertices.push_back((2.f * i) / subdivisions - 1.f);
			vertices.push_back(-1.f);
			// vertices.push_back((2.f * j) / subdivisions - 1.f);
			vertices.push_back(0.f);

			vertices.push_back((2.f * i) / subdivisions - 1.f);
			vertices.push_back(1.f);
			// vertices.push_back((2.f * j) / subdivisions - 1.f);
			vertices.push_back(0.f);

			elements.push_back(2 * (j * (subdivisions + 1) + i) + elemShift);
			elements.push_back(2 * (j * (subdivisions + 1) + i) + 1
			                   + elemShift);
		}
	}

	elemShift = 2 * (subdivisions + 1) * (subdivisions + 1);
	// elemShift *= 2;

	for(unsigned int i(0); i <= subdivisions; ++i)
	{
		for(unsigned int j(0); j <= subdivisions; ++j)
		{
			vertices.push_back(-1.f);
			vertices.push_back((2.f * i) / subdivisions - 1.f);
			// vertices.push_back((2.f * j) / subdivisions - 1.f);
			vertices.push_back(0.f);

			vertices.push_back(1.f);
			vertices.push_back((2.f * i) / subdivisions - 1.f);
			// vertices.push_back((2.f * j) / subdivisions - 1.f);
			vertices.push_back(0.f);

			elements.push_back(2 * (j * (subdivisions + 1) + i) + elemShift);
			elements.push_back(2 * (j * (subdivisions + 1) + i) + 1
			                   + elemShift);
		}
	}
}

double Grid::customRound(double x, double scale)
{
	return scale * round(x / scale);
}

QString Grid::generateScaleTextCosmo(double scale)
{
	double scaleInLY(3261.56 * 0.1 / scale);

	if(scaleInLY < 1.0)
	{
		scaleInLY *= 365.25;
	}
	else
	{
		return QString::number(scaleInLY) + " light-years";
	}

	if(scaleInLY < 1.0)
	{
		scaleInLY *= 24.0;
	}
	else
	{
		return QString::number(scaleInLY) + " light-days";
	}

	if(scaleInLY < 1.0)
	{
		scaleInLY *= 60.0;
	}
	else
	{
		return QString::number(scaleInLY) + " light-days";
	}

	if(scaleInLY < 1.0)
	{
		scaleInLY *= 60.0;
	}
	else
	{
		return QString::number(scaleInLY) + " light-minutes";
	}

	if(scaleInLY < 0.1)
	{
		scaleInLY *= 299792.0;
	}
	else
	{
		return QString::number(scaleInLY) + " light-seconds";
	}
	return QString::number(scaleInLY) + " km";
}

QString Grid::generateScaleTextPlanet(double scale)
{
	double scaleInKM(0.1 / (1000.0 * scale));

	if(scaleInKM > 29979.2)
	{
		scaleInKM /= 299792.0;
	}
	else
	{
		return QString::number(scaleInKM) + " km";
	}

	if(scaleInKM > 60.0)
	{
		scaleInKM /= 60.0;
	}
	else
	{
		return QString::number(scaleInKM) + " light-seconds";
	}

	if(scaleInKM > 60.0)
	{
		scaleInKM /= 60.0;
	}
	else
	{
		return QString::number(scaleInKM) + " light-minutes";
	}

	if(scaleInKM > 24.0)
	{
		scaleInKM /= 24.0;
	}
	else
	{
		return QString::number(scaleInKM) + " light-hours";
	}

	if(scaleInKM > 365.25)
	{
		scaleInKM /= 365.25;
	}
	else
	{
		return QString::number(scaleInKM) + " light-days";
	}
	return QString::number(scaleInKM) + " light-years";
}
