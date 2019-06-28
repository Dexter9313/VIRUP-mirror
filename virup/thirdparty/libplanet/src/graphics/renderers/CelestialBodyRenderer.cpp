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

#include "graphics/renderers/CelestialBodyRenderer.hpp"

double CelestialBodyRenderer::overridenScale = 0.0;
CelestialBodyRenderer::CelestialBodyRenderer(CelestialBody const* drawnBody,
                                             QColor const& pointColor)
    : drawnBody(drawnBody)
    , pointShader(GLHandler::newShader("colored"))
    , pointMesh(GLHandler::newMesh())
{
	model = QMatrix4x4();
	model.translate(Utils::toQt(this->drawnBody->getAbsolutePositionAtUT(0)));

	// ROTATION

	Vector3 x(1.0, 0.0, 0.0), y(0.0, 1.0, 0.0);
	x.rotateAlongZ(drawnBody->getCelestialBodyParameters().northPoleRightAsc);
	y.rotateAlongZ(drawnBody->getCelestialBodyParameters().northPoleRightAsc);
	Vector3 planetLocalZ // north pole
	    = Matrix4x4(
	          drawnBody->getCelestialBodyParameters().northPoleDeclination,
	          -1.0 * y)
	      * x;

	planetLocalZ.rotateAlongX(-1.f
	                          * drawnBody->getSystem().getDeclinationTilt());
	planetLocalZ = planetLocalZ.getUnitForm();

	Vector3 planetLocalY(
	    crossProduct(planetLocalZ, Vector3(1.0, 0.0, 0.0)).getUnitForm());
	Vector3 planetLocalX(
	    crossProduct(planetLocalY, planetLocalZ).getUnitForm());

	baseRotation = QMatrix4x4(
	    planetLocalX[0], planetLocalY[0], planetLocalZ[0], 0.f, planetLocalX[1],
	    planetLocalY[1], planetLocalZ[1], 0.f, planetLocalX[2], planetLocalY[2],
	    planetLocalZ[2], 0.f, 0.f, 0.f, 0.f, 1.f);

	// POINT
	float r, g, b;
	r = pointColor.redF();
	g = pointColor.greenF();
	b = pointColor.blueF();
	GLHandler::setVertices(pointMesh, {0.f, 0.f, 0.f, r, g, b, 1.f},
	                       pointShader, {{"position", 3}, {"color", 4}});
}

void CelestialBodyRenderer::updateMesh(UniversalTime uT,
                                       OrbitalSystemCamera const& camera)
{
	camRelPos = camera.getRelativePositionTo(drawnBody, uT);

	double centerPosition = 9900.f;

	double camDist(camRelPos.length());
	scale = centerPosition / camDist;

	if(overridenScale != 0.0 && overridenScale < scale)
	{
		scale            = overridenScale;
		clearDepthBuffer = false;
	}
	else
	{
		clearDepthBuffer = true;
	}

	double radiusScale(drawnBody->getCelestialBodyParameters().radius * scale);
	position = Utils::toQt(scale * camRelPos);
	apparentAngle
	    = 2.0 * atan(drawnBody->getCelestialBodyParameters().radius / camDist);

	model = QMatrix4x4();
	model.translate(Utils::toQt(scale * camRelPos));
	model.scale(radiusScale);

	// custom models have (1, 0, 0) at planetographic origin
	// non custom have (-1, 0, 0) at planetographic origin
	float siderealTime = drawnBody->getPrimeMeridianSiderealTimeAtUT(uT);

	QMatrix4x4 sideralRotation;
	sideralRotation.rotate(siderealTime * 180.f / constant::pi,
	                       QVector3D(0.f, 0.f, 1.f));

	properRotation = baseRotation * sideralRotation;
}

void CelestialBodyRenderer::render(BasicCamera const& /*camera*/)
{
	renderPoint();
	handleDepth();
}

void CelestialBodyRenderer::handleDepth() const
{
	if(clearDepthBuffer)
	{
		GLHandler::clearDepthBuffer();
	}
}

void CelestialBodyRenderer::renderPoint()
{
	GLHandler::setUpRender(pointShader, model);
	GLHandler::render(pointMesh);
}

CelestialBodyRenderer::~CelestialBodyRenderer()
{
	GLHandler::deleteMesh(pointMesh);
	GLHandler::deleteShader(pointShader);
}
