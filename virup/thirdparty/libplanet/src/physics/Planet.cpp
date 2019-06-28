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

#include "physics/Planet.hpp"

#include "physics/OrbitalSystem.hpp"

Planet::Planet(QJsonObject const& json, OrbitalSystem const& system)
    : CelestialBody(CelestialBody::Type::PLANET, json, system)
    , randomGen(getPseudoRandomSeed())
{
	parseJSON(json);
}

Planet::Planet(QJsonObject const& json, Orbitable const& parent)
    : CelestialBody(CelestialBody::Type::PLANET, json, parent)
    , randomGen(getPseudoRandomSeed())
{
	parseJSON(json);
}

Planet::Planet(std::string const name,
               CelestialBody::Parameters const& cbParams,
               Parameters const& planetParams, Orbitable const& parent,
               Orbit* orbit)
    : CelestialBody(CelestialBody::Type::PLANET, name, cbParams, parent, orbit)
    , parameters(planetParams)
    , randomGen(getPseudoRandomSeed())
{
}

Planet::Planet(std::string const name,
               CelestialBody::Parameters const& cbParams,
               Parameters const& planetParams, OrbitalSystem const& system,
               Orbit* orbit)
    : CelestialBody(CelestialBody::Type::PLANET, name, cbParams, system, orbit)
    , parameters(planetParams)
    , randomGen(getPseudoRandomSeed())
{
}

Star const* Planet::getHostStar() const
{
	if(getParent() == nullptr)
	{
		return nullptr;
	}

	if(getParent()->getOrbitableType() == Orbitable::Type::STAR)
	{
		return dynamic_cast<Star const*>(getParent());
	}
	return this->getSystem().getAllStarsPointers()[0];
}

QJsonObject Planet::getJSONRepresentation() const
{
	QJsonObject result(CelestialBody::getJSONRepresentation());
	result["type"]       = typeToStr(parameters.type).c_str();
	result["atmosphere"] = parameters.atmosphere;
	result["innerRing"]  = parameters.innerRing;
	result["outerRing"]  = parameters.outerRing;
	return result;
}

std::string Planet::typeToStr(Type type)
{
	switch(type)
	{
		case Type::GAZGIANT:
			return "gazgiant";
		case Type::TERRESTRIAL:
			return "terrestrial";
		default:
			return "generic";
	}
}

Planet::Type Planet::strToType(std::string const& str)
{
	if(str == "gazgiant")
	{
		return Type::GAZGIANT;
	}

	if(str == "terrestrial")
	{
		return Type::TERRESTRIAL;
	}

	return Type::GENERIC;
}

void Planet::parseJSON(QJsonObject const& json)
{
	parameters.type = strToType(
	    json["type"].toString(proceduralTypeStr().c_str()).toStdString());
	if(!json.contains("mass"))
	{
		CelestialBody::parameters.mass = proceduralMass();
	}
	if(!json.contains("color"))
	{
		CelestialBody::parameters.color = proceduralColor();
	}
	if(!json.contains("siderealTimeAtEpoch"))
	{
		CelestialBody::parameters.siderealTimeAtEpoch
		    = proceduralSiderealTimeAtEpoch();
	}
	if(!json.contains("siderealRotationPeriod"))
	{
		CelestialBody::parameters.siderealRotationPeriod
		    = proceduralSiderealRotationPeriod();
	}
	if(!json.contains("northPoleRightAsc"))
	{
		CelestialBody::parameters.northPoleRightAsc
		    = proceduralNorthPoleRightAsc();
	}
	if(!json.contains("northPoleDeclination"))
	{
		CelestialBody::parameters.northPoleDeclination
		    = proceduralNorthPoleDeclination();
	}
	if(!json.contains("oblateness"))
	{
		CelestialBody::parameters.oblateness = proceduralOblateness();
	}
	parameters.atmosphere = json["atmosphere"].toDouble(proceduralAtmosphere());
	parameters.outerRing  = json["outerRing"].toDouble(proceduralOuterRings());
	parameters.innerRing  = json["innerRing"].toDouble(proceduralInnerRings());
}

double Planet::assumedTidalLockingStrengh() const
{
	auto orbit(getOrbit());
	if(orbit == nullptr)
	{
		return 0.0;
	}
	double localAcceleration(constant::G * orbit->getMassiveBodyMass()
	                         / pow(orbit->getParameters().semiMajorAxis, 2));
	// assume tidally locked
	if(localAcceleration > 0.1)
	{
		return 1.0;
	}
	// assume close to tidally locked
	if(localAcceleration > 0.01)
	{
		// (0 to 1)
		double tidalLockingStrenght((localAcceleration - 0.01) / (0.1 - 0.01));
		return tidalLockingStrenght;
	}
	return 0.0;
}

double Planet::proceduralMass() const
{
	// g / cm^3
	double density(1.f);
	float r(randomGen.getRandomNumber(0));
	if(parameters.type == Type::GAZGIANT)
	{
		density = 0.5 + r * 1.5;
	}
	else
	{
		density = 3.0 + r * r * 7.0;
	}
	// convert to kg / m^3
	density *= 1000.0;

	// in m
	double rad(getCelestialBodyParameters().radius);
	Vector3 o(getCelestialBodyParameters().oblateness);
	// in m^3
	double volume
	    = 4.0 * constant::pi * (rad * o[0] * rad * o[1] * rad * o[2]) / 3.0;

	// in kg
	return density * volume;
}

Vector3 Planet::proceduralOblateness() const
{
	// empirical (works well with solar system
	return Vector3(
	    1.0, 1.0,
	    1.0
	        - exp(-getCelestialBodyParameters().siderealRotationPeriod
	              / 16400.0));
}

Color Planet::proceduralColor() const
{
	float r(randomGen.getRandomNumber(1));

	Color c1(97, 142, 232), c2(255, 255, 255), c3(216, 202, 157);
	if(parameters.type != Type::GAZGIANT)
	{
		c1 = Color(255, 255, 255);
		c2 = Color(54, 57, 48);
		c3 = Color(231, 125, 17);
	}
	Color result(c1);
	if(r <= 0.6f)
	{
		r /= 0.6f;
		result.r = c1.r * (1.f - r) + c2.r * r;
		result.g = c1.g * (1.f - r) + c2.g * r;
		result.b = c1.b * (1.f - r) + c2.b * r;
	}
	else
	{
		r -= 0.6f;
		r /= 0.4f;
		result.r = c2.r * (1.f - r) + c3.r * r;
		result.g = c2.g * (1.f - r) + c3.g * r;
		result.b = c2.b * (1.f - r) + c3.b * r;
	}
	return result;
}

double Planet::proceduralSiderealTimeAtEpoch() const
{
	return randomGen.getRandomNumber(2) * 2.0 * constant::pi;
}

double Planet::proceduralSiderealRotationPeriod() const
{
	auto orbit(getOrbit());
	float r = randomGen.getRandomNumber(3);

	double randomRotPeriod;
	if(parameters.type == Type::GAZGIANT)
	{
		randomRotPeriod = (r * 20 + 1) * 3600;
	}
	else
	{
		randomRotPeriod = (r * 20 + 8) * 3600;
	}

	// m/s
	double randomEquatorSurfaceVel = 2.0 * constant::pi
	                                 * CelestialBody::parameters.radius
	                                 / randomRotPeriod;
	if(randomEquatorSurfaceVel > getEscapeVelocity() / 2.0)
	{
		randomRotPeriod = 4.0 * constant::pi * CelestialBody::parameters.radius
		                  / getEscapeVelocity();
	}

	if(orbit != nullptr)
	{
		double tidalLockingStrengh(assumedTidalLockingStrengh());
		return orbit->getPeriod() * tidalLockingStrengh
		       + randomRotPeriod * (1.0 - tidalLockingStrengh);
	}
	// not tidally locked
	return randomRotPeriod;
}

double Planet::proceduralNorthPoleRightAsc() const
{
	auto orbit(getOrbit());
	double randomRA(randomGen.getRandomNumber(4) * 2.0 * constant::pi);

	if(orbit != nullptr)
	{
		Vector3 north(orbit->getNorth());
		auto RAdec = getSystem().getRADecFromCarthesian(north);
		double tidalLockingStrengh(assumedTidalLockingStrengh());
		tidalLockingStrengh += 0.1;
		tidalLockingStrengh = fmin(1.0, tidalLockingStrengh);
		tidalLockingStrengh = 1.0 - pow((1.0 - tidalLockingStrengh), 8);
		return RAdec.first * tidalLockingStrengh
		       + randomRA * (1.0 - tidalLockingStrengh);
	}
	return randomRA;
}

double Planet::proceduralNorthPoleDeclination() const
{
	auto orbit(getOrbit());
	double randomDec((randomGen.getRandomNumber(5) - 0.5f) * constant::pi);

	if(orbit != nullptr)
	{
		Vector3 north(orbit->getNorth());
		auto RAdec = getSystem().getRADecFromCarthesian(north);
		double tidalLockingStrengh(assumedTidalLockingStrengh());
		tidalLockingStrengh += 0.1;
		tidalLockingStrengh = fmin(1.0, tidalLockingStrengh);
		tidalLockingStrengh = 1.0 - pow((1.0 - tidalLockingStrengh), 8);
		return RAdec.second * tidalLockingStrengh
		       + randomDec * (1.0 - tidalLockingStrengh);
	}
	return randomDec;
}

std::string Planet::proceduralTypeStr() const
{
	if(CelestialBody::parameters.mass != 0.0
	   && CelestialBody::parameters.radius != 0.0)
	{
		// in m
		double rad(getCelestialBodyParameters().radius);
		Vector3 o(getCelestialBodyParameters().oblateness);
		// in m^3
		double volume
		    = 4.0 * constant::pi * (rad * o[0] * rad * o[1] * rad * o[2]) / 3.0;

		double density(CelestialBody::parameters.mass / volume);
		if(density < 2250)
		{
			return "gazgiant";
		}
		return "terrestrial";
	}
	return randomGen.getRandomNumber(6) < 0.333f ? "terrestrial" : "gazgiant";
}

double Planet::proceduralAtmosphere() const
{
	return 0.3 * pow(randomGen.getRandomNumber(7), 6);
}

double Planet::proceduralOuterRings() const
{
	float probability(0.7f);
	if(parameters.type != Type::GAZGIANT)
	{
		probability = 0.05f;
	}
	float r(randomGen.getRandomNumber(8));
	if(r > probability)
	{
		return 0.0;
	}
	r /= probability;
	r *= r;
	double result(CelestialBody::parameters.radius * (2.0 + (10.0 * r)));
	// don't get unreallistically large rings (depends on tides inward and
	// outward)
	double maxLimit(
	    fmin(getMaximumRocheLimit() * 0.9, getSphereOfInfluenceRadius() * 0.5));
	if(result > maxLimit)
	{
		result = maxLimit;
	}
	if(result < CelestialBody::parameters.radius)
	{
		return 0.0;
	}
	return result;
}

double Planet::proceduralInnerRings() const
{
	float r(randomGen.getRandomNumber(9));
	double outerHeight(parameters.outerRing
	                   - getCelestialBodyParameters().radius);
	if(outerHeight < 0.0)
	{
		return 0.0;
	}
	return getCelestialBodyParameters().radius + (1.0 - r * r) * outerHeight;
}
