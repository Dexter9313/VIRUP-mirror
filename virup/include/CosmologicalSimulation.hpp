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

#ifndef COSMOLOGICALSIMULATION_HPP
#define COSMOLOGICALSIMULATION_HPP

#include "UniverseElement.hpp"
#include "methods/TreeMethodLOD.hpp"

class CosmologicalSimulation : public UniverseElement
{
  public:
	CosmologicalSimulation(std::string const& gazOctreePath,
	                       std::string const& starsOctreePath,
	                       std::string const& darkMatterOctreePath);
	virtual BBox getBoundingBox() const override;
	virtual void render(Camera const& camera,
	                    ToneMappingModel const* tmm) override;
	~CosmologicalSimulation() = default;

  public:
	TreeMethodLOD trees;
};

#endif // COSMOLOGICALSIMULATION_HPP
