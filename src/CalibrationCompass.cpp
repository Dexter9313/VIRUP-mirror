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

#include "CalibrationCompass.hpp"

double& CalibrationCompass::forcedTickResolution()
{
	static double forcedTickResolution = 0.0;
	return forcedTickResolution;
}

bool& CalibrationCompass::forceProtractorMode()
{
	static bool forceProtractorMode = false;
	return forceProtractorMode;
}

float& CalibrationCompass::serverHorizontalFOV()
{
	static float serverHorizontalFOV = 0.f;
	return serverHorizontalFOV;
}

unsigned int& CalibrationCompass::serverRenderTargetWidth()
{
	static unsigned int serverRenderTargetWidth = 0;
	return serverRenderTargetWidth;
}

float& CalibrationCompass::tilt()
{
	static float tilt = 0.f;
	return tilt;
}

double CalibrationCompass::getDoubleFarRightPixelSubtendedAngle(
    float horizontalFOV, unsigned int renderTargetWidth)
{
	//// compute angle of furthest left and right pixels
	//// screen size := 1 := sum(dw)
	// distance of camera to screen
	const double D(0.5 / tan(horizontalFOV * M_PI / 360.0));
	// length of a pixel on screen
	const double dw(1.0 / renderTargetWidth);
	// length from far right of screen to camera
	const double C(sqrt(D * D + 0.5 * 0.5));
	// length from far right pixel's left to camera
	const double E(sqrt(D * D + (0.5 - dw) * (0.5 - dw)));
	// Al-Kashi
	const double cosAngle((C * C + E * E - dw * dw) / (2.0 * C * E));
	return acos(cosAngle) * 360.0 / M_PI;
}

double CalibrationCompass::getCurrentTickResolution()
{
	double tickRes;
	if(forcedTickResolution() != 0.0)
	{
		tickRes = forcedTickResolution();
	}
	else if(forceProtractorMode())
	{
		tickRes = 0.1;
	}
	else
	{
		tickRes = getDoubleFarRightPixelSubtendedAngle(
		    serverHorizontalFOV(), serverRenderTargetWidth());
	}
	return tickRes;
}

CalibrationCompass::CalibrationCompass()
    : shader("default")
{
	shader.setUniform("color", QColor(255, 0, 0));
	std::vector<float> circleVertices;
	for(int i(0); i < 360; i += 1)
	{
		circleVertices.push_back(sin(i * M_PI / 180.0));
		circleVertices.push_back(0.0);
		circleVertices.push_back(-cos(i * M_PI / 180.0));
	}
	circle.setVertexShaderMapping(shader, {{"position", 3}});
	circle.setVertices(circleVertices);

	// precompute billboards textures
	for(unsigned int i(0); i < 100; ++i)
	{
		billboards.push_back(new Text3D(256, 256));
		billboards[i]->setText(QString::number(i));
		billboards[i]->setColor(QColor(255, 0, 0));
		auto font(billboards[i]->getFont());
		font.setPixelSize(128);
		billboards[i]->setFont(font);
		billboards[i]->setFlags(Qt::AlignCenter);
	}
}

void CalibrationCompass::render(QMatrix4x4 const& angleShiftMat)
{
	QMatrix4x4 tiltMat;
	tiltMat.rotate(tilt(), QVector3D(0.f, 0.f, -1.f));

	GLHandler::glf().glDisable(GL_DEPTH_TEST);

	shader.setUniform("exposure", exposure);
	shader.setUniform("dynamicrange", dynamicrange);

	// NOLINTNEXTLINE(cert-flp30-c, clang-analyzer-security.FloatLoopCounter)
	for(float lat(-M_PI_2); lat < M_PI_2; lat += 10.0 * M_PI / 180.0)
	{
		renderCircle(angleShiftMat * tiltMat, lat);
	}

	double doubleAngle(getCurrentTickResolution());
	renderCompassTicks(angleShiftMat * tiltMat, 1.3, 100.0 * doubleAngle, true);
	renderCompassTicks(angleShiftMat * tiltMat, 1.0, 10.0 * doubleAngle);
	renderCompassTicks(angleShiftMat * tiltMat, 0.7, 1.0 * doubleAngle);

	GLHandler::glf().glEnable(GL_DEPTH_TEST);
}

void CalibrationCompass::renderCircle(QMatrix4x4 const& angleShiftMat,
                                      float latitude)
{
	QMatrix4x4 model;
	model.translate(QVector3D(0.f, sin(latitude), 0.f));
	model.scale(cos(latitude));

	GLHandler::setUpRender(shader, angleShiftMat * model,
	                       GLHandler::GeometricSpace::EYE);
	circle.render(PrimitiveType::LINE_LOOP);
}

void CalibrationCompass::renderCompassTicks(QMatrix4x4 const& angleShiftMat,
                                            float heightMultiplier,
                                            double deltaDeg, bool labels)
{
	GLMesh mesh;
	std::vector<float> vertices;
	unsigned int j(0);
	// NOLINTNEXTLINE(cert-flp30-c, clang-analyzer-security.FloatLoopCounter)
	for(double i(0); i < 360; i += deltaDeg)
	{
		vertices.push_back(sin(i * M_PI / 180.0));
		vertices.push_back(0.0);
		vertices.push_back(-cos(i * M_PI / 180.0));
		float xTop(sin(i * M_PI / 180.0)),
		    yTop(i == 0 ? 0.07 : heightMultiplier * 0.05),
		    zTop(-cos(i * M_PI / 180.0));
		vertices.push_back(xTop);
		vertices.push_back(yTop);
		vertices.push_back(zTop);
		if(labels && j < 100)
		{
			QMatrix4x4 model;
			model.translate(xTop, yTop + 0.04, zTop);
			model.scale(0.1);
			model.rotate(i, QVector3D(0.f, -1.f, 0.f));
			billboards[j]->getModel() = angleShiftMat * model;
			billboards[j]->getShader().setUniform("exposure", exposure);
			billboards[j]->getShader().setUniform("dynamicrange", dynamicrange);
			billboards[j]->render(GLHandler::GeometricSpace::EYE);
		}
		++j;
	}
	mesh.setVertexShaderMapping(shader, {{"position", 3}});
	mesh.setVertices(vertices);

	GLHandler::setUpRender(shader, angleShiftMat,
	                       GLHandler::GeometricSpace::EYE);
	mesh.render(PrimitiveType::LINES);
}

CalibrationCompass::~CalibrationCompass()
{
	for(auto* text : billboards)
	{
		delete text;
	}
}
