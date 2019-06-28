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

#ifndef STAR_HPP
#define STAR_HPP

#include <QImage>
#include <QJsonObject>
#include <physics/Color.hpp>
#include <string>

#include "CelestialBody.hpp"
#include "physics/blackbody.hpp"

class Star : public CelestialBody
{
  public:
	// constructors recompute CelestialBody::Parameters' color depending on
	// temperature
	Star(QJsonObject const& json, OrbitalSystem const& system);
	Star(QJsonObject const& json, Orbitable const& parent);
	Star(std::string const name, CelestialBody::Parameters const& parameters,
	     double temperature, Orbitable const& parent, Orbit* orbit = nullptr);
	Star(std::string const name, CelestialBody::Parameters const& parameters,
	     double temperature, OrbitalSystem const& system,
	     Orbit* orbit = nullptr);

	double getTemperature() const { return temperature; };
	virtual QJsonObject getJSONRepresentation() const override;

  private:
	double temperature;

	// sRGB
	Color computeColor() const;
};

#endif // STAR_HPP
