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
    : shader("default")
    , scaleText3D(1024, 256)
{
	shader.setUniform("alpha", 0.5f);
	shader.setUniform("color",
	                  QSettings().value("misc/gridcolor").value<QColor>());

	std::vector<float> vertices;
	std::vector<unsigned int> elements;
	generateGridVertices(vertices, elements);
	mesh.setVertexShaderMapping(shader, {{"position", 3}});
	mesh.setVertices(vertices, elements);

	scaleText3D.setColor(QColor(0, 255, 0));

	PythonQtHandler::addObject("Grid", this);
}

void Grid::setColor(QColor const& color)
{
	QSettings().setValue("misc/gridcolor", color);
	shader.setUniform("color", color);
}

void Grid::render(double scale, double height)
{
	// round scale to nearest power of 10
	double roundscale((pow(10, round(log10(scale)))));
	double gridScale(1.0 * scale / roundscale);

	QMatrix4x4 t;
	t.translate(QVector3D(0.f, height, 0.f));
	t.scale(gridScale);

	GLHandler::beginTransparent();
	GLHandler::setUpRender(shader, t,
	                       GLHandler::GeometricSpace::STANDINGTRACKED);
	mesh.render(PrimitiveType::LINES);
	GLHandler::endTransparent();

	QString newScaleText(generateScaleText(10.0 * roundscale));
	if(newScaleText != scaleText)
	{
		scaleText = newScaleText;
		scaleText3D.setText(scaleText);
	}

	QMatrix4x4 t2;
	t2.rotate(-90.f, 1.f, 0.f, 0.f);
	t2.translate(1.0f, -0.1f, 0.f);
	scaleText3D.getModel() = t * t2;
	scaleText3D.render(GLHandler::GeometricSpace::STANDINGTRACKED);
}

/* void Grid::renderCosmo(Camera const& cam)
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

    GLHandler::beginTransparent();
    GLHandler::setUpRender(shader, model * t);
    mesh.render(GLHandler::PrimitiveType::LINES);
    GLHandler::endTransparent();

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

    GLHandler::beginTransparent();
    GLHandler::setUpRender(shader, model * t);
    mesh.render(GLHandler::PrimitiveType::LINES);
    GLHandler::endTransparent();

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
} */

void Grid::generateGridVertices(std::vector<float>& vertices,
                                std::vector<unsigned int>& elements)
{
	const unsigned int subdivisions(20);

	for(unsigned int i(0); i <= subdivisions; ++i)
	{
		for(unsigned int j(0); j <= subdivisions; ++j)
		{
			vertices.push_back((2.f * i) / subdivisions - 1.f);
			vertices.push_back(0.f);
			vertices.push_back(-1.f);

			vertices.push_back((2.f * i) / subdivisions - 1.f);
			vertices.push_back(0.f);
			vertices.push_back(1.f);

			elements.push_back(2 * (j * (subdivisions + 1) + i));
			elements.push_back(2 * (j * (subdivisions + 1) + i) + 1);
		}
	}

	unsigned int elemShift(2 * (subdivisions + 1) * (subdivisions + 1));
	for(unsigned int i(0); i <= subdivisions; ++i)
	{
		for(unsigned int j(0); j <= subdivisions; ++j)
		{
			vertices.push_back(-1.f);
			vertices.push_back(0.f);
			vertices.push_back((2.f * i) / subdivisions - 1.f);

			vertices.push_back(1.f);
			vertices.push_back(0.f);
			vertices.push_back((2.f * i) / subdivisions - 1.f);

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

/*
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
}*/

QString Grid::generateScaleText(double scale)
{
	double scaleInM(1.0 / scale);

	if(scaleInM > 100.0)
	{
		scaleInM /= 1000.0;
	}
	else
	{
		return QString::number(scaleInM) + " m";
	}

	if(scaleInM > 29979.2)
	{
		scaleInM /= 299792.0;
	}
	else
	{
		return QString::number(scaleInM) + " km";
	}

	if(scaleInM > 60.0)
	{
		scaleInM /= 60.0;
	}
	else
	{
		return QString::number(scaleInM) + " light-seconds";
	}

	if(scaleInM > 60.0)
	{
		scaleInM /= 60.0;
	}
	else
	{
		return QString::number(scaleInM) + " light-minutes";
	}

	if(scaleInM > 24.0)
	{
		scaleInM /= 24.0;
	}
	else
	{
		return QString::number(scaleInM) + " light-hours";
	}

	if(scaleInM > 365.25)
	{
		scaleInM /= 365.25;
	}
	else
	{
		return QString::number(scaleInM) + " light-days";
	}

	if(scaleInM > 1.0e6)
	{
		scaleInM /= 1.0e6;
	}
	else
	{
		return QString::number(scaleInM) + " light-years";
	}

	if(scaleInM > 1.0e3)
	{
		scaleInM /= 1.0e3;
	}
	else
	{
		return QString::number(scaleInM) + " Mly";
	}

	return QString::number(scaleInM) + " Gly";
}
