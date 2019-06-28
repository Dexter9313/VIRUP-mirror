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

#include "physics/Star.hpp"

Star::Star(QJsonObject const& json, OrbitalSystem const& system)
    : CelestialBody(CelestialBody::Type::STAR, json, system)
{
	temperature      = json["temperature"].toDouble(6000.0);
	parameters.color = computeColor();
}

Star::Star(QJsonObject const& json, Orbitable const& parent)
    : CelestialBody(CelestialBody::Type::STAR, json, parent)
{
	temperature      = json["temperature"].toDouble(6000.0);
	parameters.color = computeColor();
}

Star::Star(std::string const name, CelestialBody::Parameters const& parameters,
           double temperature, Orbitable const& parent, Orbit* orbit)
    : CelestialBody(CelestialBody::Type::STAR, name, parameters, parent, orbit)
    , temperature(temperature)
{
	this->parameters.color = computeColor();
}

Star::Star(std::string const name, CelestialBody::Parameters const& parameters,
           double temperature, OrbitalSystem const& system, Orbit* orbit)
    : CelestialBody(CelestialBody::Type::STAR, name, parameters, system, orbit)
    , temperature(temperature)
{
	this->parameters.color = computeColor();
}

Color Star::computeColor() const
{
	Color sRGBcolor(blackbody::colorFromTemperature(temperature));
	return sRGBcolor;
}

QJsonObject Star::getJSONRepresentation() const
{
	QJsonObject result(CelestialBody::getJSONRepresentation());
	result["temperature"] = temperature;
	return result;
}
