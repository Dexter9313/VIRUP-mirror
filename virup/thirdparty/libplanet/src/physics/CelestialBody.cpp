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

#include "../../include/physics/CelestialBody.hpp"

CelestialBody::CelestialBody(Type type, QJsonObject const& json,
                             OrbitalSystem const& system)
    : Orbitable(orbitableType(type), json, system)
    , type(type)
{
	parseJSON(json);
}

CelestialBody::CelestialBody(Type type, QJsonObject const& json,
                             Orbitable const& parent)
    : Orbitable(orbitableType(type), json, parent)
    , type(type)
{
	parseJSON(json);
}

CelestialBody::CelestialBody(Type type, std::string const name,
                             Parameters const& parameters,
                             OrbitalSystem const& system, Orbit* orbit)
    : Orbitable(orbitableType(type), name, system, orbit)
    , parameters(std::move(parameters))
    , type(type)
{
}

CelestialBody::CelestialBody(Type type, std::string const name,
                             Parameters const& parameters,
                             Orbitable const& parent, Orbit* orbit)
    : Orbitable(orbitableType(type), name, parent, orbit)
    , parameters(std::move(parameters))
    , type(type)
{
}

double CelestialBody::getPrimeMeridianSiderealTimeAtUT(UniversalTime uT) const
{
	operator%=<UniversalTime>(uT, parameters.siderealRotationPeriod);

	return parameters.siderealTimeAtEpoch
	       + 2.0 * constant::pi * static_cast<double>(uT)
	             / parameters.siderealRotationPeriod;
}

double CelestialBody::getEscapeVelocity(double altitude) const
{
	return sqrt(2.0 * constant::G * parameters.mass
	            / (parameters.radius + altitude));
}

double CelestialBody::getSphereOfInfluenceRadius() const
{
	auto parent(getParent());
	if(parent == nullptr
	   || parent->getOrbitableType() == Orbitable::Type::BINARY
	   || getOrbit() == nullptr)
	{
		return DBL_MAX;
	}
	return getOrbit()->getParameters().semiMajorAxis
	       * pow(parameters.mass
	                 / dynamic_cast<CelestialBody const*>(parent)
	                       ->getCelestialBodyParameters()
	                       .mass,
	             2.0 / 5.0);
}

double CelestialBody::getMaximumRocheLimit() const
{
	// kg
	double mass(getCelestialBodyParameters().mass);

	// in m
	double rad(getCelestialBodyParameters().radius);
	Vector3 o(getCelestialBodyParameters().oblateness);
	// in m^3
	double volume
	    = 4.0 * constant::pi * (rad * o[0] * rad * o[1] * rad * o[2]) / 3.0;

	// in kg/m^3
	double density(mass / volume);

	// in m
	return 2.422849865 * parameters.radius * pow(density / 500.0, 1.0 / 3.0);
}

QJsonObject CelestialBody::getJSONRepresentation() const
{
	QJsonObject result = Orbitable::getJSONRepresentation();

	result["mass"]       = parameters.mass;
	result["radius"]     = parameters.radius;
	result["oblateness"] = parameters.oblateness.getJSONRepresentation();
	result["color"]      = parameters.color.getJSONRepresentation();
	result["siderealTimeAtEpoch"]    = parameters.siderealTimeAtEpoch;
	result["siderealRotationPeriod"] = parameters.siderealRotationPeriod;
	result["northPoleRightAsc"]      = parameters.northPoleRightAsc;
	result["northPoleDeclination"]   = parameters.northPoleDeclination;

	return result;
}

void CelestialBody::parseJSON(QJsonObject const& json)
{
	parameters.mass = json["mass"].toDouble();
	// TEMP generate radius
	parameters.radius = json["radius"].toDouble(70000000.0);
	parameters.oblateness
	    = Vector3(json["oblateness"].toObject(), Vector3(1.0, 1.0, 1.0));
	parameters.color
	    = Color(json["color"].toObject(), Color(255, 255, 255, 255));
	parameters.siderealTimeAtEpoch = json["siderealTimeAtEpoch"].toDouble();
	parameters.siderealRotationPeriod
	    = json["siderealRotationPeriod"].toDouble(DBL_MAX);
	parameters.northPoleRightAsc = json["northPoleRightAsc"].toDouble();
	parameters.northPoleDeclination
	    = json["northPoleDeclination"].toDouble(1.5707963705062866);
}

Orbitable::Type CelestialBody::orbitableType(Type celestialBodyType)
{
	return celestialBodyType == Type::STAR ? Orbitable::Type::STAR
	                                       : Orbitable::Type::PLANET;
}
