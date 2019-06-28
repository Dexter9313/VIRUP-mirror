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

#ifndef CELESTIALBODYRENDERER_H
#define CELESTIALBODYRENDERER_H

#include <QMatrix4x4>

#include "GLHandler.hpp"
#include "graphics/OrbitalSystemCamera.hpp"
#include "graphics/Utils.hpp"
#include "physics/CelestialBody.hpp"
#include "physics/OrbitalSystem.hpp"

class CelestialBodyRenderer
{
  public:
	CelestialBodyRenderer(CelestialBody const* drawnBody,
	                      QColor const& pointColor);
	virtual void updateMesh(UniversalTime uT,
	                        OrbitalSystemCamera const& camera);
	virtual void render(BasicCamera const& camera);
	CelestialBody const* getDrawnBody() const { return drawnBody; };
	virtual ~CelestialBodyRenderer();

	static double overridenScale;

  protected:
	void handleDepth() const;

	CelestialBody const* drawnBody;

	QMatrix4x4 model;
	QVector3D position;
	Vector3 camRelPos;
	double scale;
	double apparentAngle;

	QMatrix4x4 baseRotation;   // only align axis, no sideral time
	QMatrix4x4 properRotation; // full rotation, sideral time included

  private:
	// POINT
	GLHandler::ShaderProgram pointShader;
	GLHandler::Mesh pointMesh;

	bool clearDepthBuffer = false;

	void renderPoint();
};

#endif // CELESTIALBODYRENDERER_H
