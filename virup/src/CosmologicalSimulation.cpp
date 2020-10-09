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

#include "CosmologicalSimulation.hpp"

CosmologicalSimulation::CosmologicalSimulation(
    std::string const& gazOctreePath, std::string const& starsOctreePath,
    std::string const& darkMatterOctreePath)
{
	trees.init(gazOctreePath, starsOctreePath, darkMatterOctreePath);
}

BBox CosmologicalSimulation::getBoundingBox() const
{
	return trees.getDataBoundingBox();
}

void CosmologicalSimulation::render(Camera const& camera,
                                    ToneMappingModel const* /*tmm*/)
{
	QMatrix4x4 model;
	QVector3D campos;
	getModelAndCampos(camera, model, campos);

	trees.setAlpha(brightnessMultiplier);
	GLHandler::glf().glEnable(GL_CLIP_DISTANCE0);
	trees.render(camera, model, campos);
	GLHandler::glf().glDisable(GL_CLIP_DISTANCE0);
}
