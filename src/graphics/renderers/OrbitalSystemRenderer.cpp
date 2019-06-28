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
#include "graphics/renderers/OrbitalSystemRenderer.hpp"

bool OrbitalSystemRenderer::autoCameraTarget = true;

OrbitalSystemRenderer::OrbitalSystemRenderer(OrbitalSystem* drawnSystem)
    : drawnSystem(drawnSystem)
{
	for(auto body : drawnSystem->getAllPlanetsPointers())
	{
		bodyRenderers.push_back(new PlanetRenderer(body));
	}
	for(auto star : drawnSystem->getAllStarsPointers())
	{
		bodyRenderers.push_back(new StarRenderer(star));
	}
}

void OrbitalSystemRenderer::updateMesh(UniversalTime uT,
                                       OrbitalSystemCamera& camera)
{
	sortedRenderers.clear();
	for(auto bodyRenderer : bodyRenderers)
	{
		sortedRenderers[camera
		                    .getRelativePositionTo(bodyRenderer->getDrawnBody(),
		                                           uT)
		                    .length()]
		    = bodyRenderer;
	}

	if(autoCameraTarget)
	{
		auto closest(sortedRenderers.begin()->second->getDrawnBody());
		while(camera.getRelativePositionTo(closest, uT).length()
		          > closest->getSphereOfInfluenceRadius()
		      && closest->getParent() != nullptr
		      && closest->getParent()->getOrbitableType()
		             != Orbitable::Type::BINARY)
		{
			closest = dynamic_cast<CelestialBody const*>(closest->getParent());
		}

		if(closest != camera.target)
		{
			camera.relativePosition
			    = -1.0 * camera.getRelativePositionTo(closest, uT);
			camera.target = closest;
		}
	}

	for(auto& pair : sortedRenderers)
	{
		pair.second->updateMesh(uT, camera);
	}
}

void OrbitalSystemRenderer::render(BasicCamera const& camera)
{
	auto it(sortedRenderers.end());
	while(it != sortedRenderers.begin())
	{
		--it;
		it->second->render(camera);
	}
}

OrbitalSystemRenderer::~OrbitalSystemRenderer()
{
	for(auto bodyRenderer : bodyRenderers)
	{
		delete bodyRenderer;
	}
}
