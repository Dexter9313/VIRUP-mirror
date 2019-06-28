/*
    Copyright (C) 2018 Florian Cabot <florian.cabot@hotmail.fr>

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

#ifndef PLANETRENDERER_HPP
#define PLANETRENDERER_HPP

#include <QFileInfo>

#include "DetailedPlanetRenderer.hpp"
#include "GLHandler.hpp"
#include "Primitives.hpp"
#include "graphics/OrbitalSystemCamera.hpp"
#include "physics/Planet.hpp"
#include "physics/Star.hpp"

#include "graphics/Utils.hpp"
#include "graphics/renderers/CelestialBodyRenderer.hpp"
#include "physics/Planet.hpp"

class PlanetRenderer : public CelestialBodyRenderer
{
  public:
	PlanetRenderer(Planet const* drawnBody);
	virtual void updateMesh(UniversalTime uT,
	                        OrbitalSystemCamera const& camera) override;
	virtual void render(BasicCamera const& camera) override;
	~PlanetRenderer();

  private:
	void loadPlanet();
	void unloadPlanet(bool waitIfPlanetNotLoaded = false);
	static QColor computePointColor(Planet const& drawnPlanet,
	                                Star const* star);

	float boundingSphere;
	bool culled = false;
	Star const* const hostStar;

	DetailedPlanetRenderer* planet = nullptr;
	bool customModel               = false;
	QVector3D lightpos;
	float lightradius;
	QColor lightcolor;
	std::array<QVector4D, 5>
	    neighborsPosRadius; // 3D position + radius of 5 closest neighbors
	std::array<QVector3D, 5>
	    neighborsOblateness; // oblateness of 5 closest neighbors

	// UNLOADED
	GLHandler::ShaderProgram unloadedShader;
	GLHandler::Mesh unloadedMesh;
};

#endif // PLANETRENDERER_HPP
