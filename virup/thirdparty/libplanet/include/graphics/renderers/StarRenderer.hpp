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
#ifndef STARRENDERER_HPP
#define STARRENDERER_HPP

#include "Billboard.hpp"
#include "graphics/OrbitalSystemCamera.hpp"
#include "graphics/Utils.hpp"
#include "graphics/renderers/CelestialBodyRenderer.hpp"
#include "math/MathUtils.hpp"
#include "physics/Star.hpp"
#include "physics/UniversalTime.hpp"

class StarRenderer : public CelestialBodyRenderer
{
  public:
	StarRenderer(Star const* drawnStar);
	virtual void updateMesh(UniversalTime uT,
	                        OrbitalSystemCamera const& camera) override;
	virtual void render(BasicCamera const& camera) override;

  private:
	double billboardOriginalEdgeSize;
	Billboard billboard;

	GLHandler::Mesh detailedMesh;
	GLHandler::ShaderProgram detailedShader;
	GLHandler::Texture blackbodyTex;

	static QImage getBillboardImage(Star const* star);
};

#endif // STARRENDERER_HPP
