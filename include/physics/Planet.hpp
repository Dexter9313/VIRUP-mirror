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

#ifndef PLANET_HPP
#define PLANET_HPP

#include <cmath>

#include "CelestialBody.hpp"
#include "math/DeterministicRandomFloat.hpp"

class Star;

class Planet : public CelestialBody
{
  public:
	enum class Type
	{
		GENERIC,
		TERRESTRIAL,
		GAZGIANT,
	};

	struct Parameters
	{
		Type type         = Type::GENERIC;
		double atmosphere = 0.0;
		double innerRing  = 0.0;
		double outerRing  = 0.0;
	};

	Planet(QJsonObject const& json, OrbitalSystem const& system);
	Planet(QJsonObject const& json, Orbitable const& parent);
	Planet(std::string const name, CelestialBody::Parameters const& cbParams,
	       Parameters const& planetParams, Orbitable const& parent,
	       Orbit* orbit = nullptr);
	Planet(std::string const name, CelestialBody::Parameters const& cbParams,
	       Parameters const& planetParams, OrbitalSystem const& system,
	       Orbit* orbit = nullptr);

	Parameters getPlanetParameters() const { return parameters; };
	Star const* getHostStar() const;
	virtual QJsonObject getJSONRepresentation() const override;

  private:
	Parameters parameters;

	static std::string typeToStr(Type type);
	static Type strToType(std::string const& str);

	void parseJSON(QJsonObject const& json);

	DeterministicRandomFloat randomGen;

	double assumedTidalLockingStrengh() const;

	double proceduralMass() const;
	Vector3 proceduralOblateness() const;
	Color proceduralColor() const;
	double proceduralSiderealTimeAtEpoch() const;
	double proceduralSiderealRotationPeriod() const;
	double proceduralNorthPoleRightAsc() const;
	double proceduralNorthPoleDeclination() const;

	std::string proceduralTypeStr() const;
	double proceduralAtmosphere() const;
	double proceduralOuterRings() const;
	double proceduralInnerRings() const;
};

#endif // PLANET_HPP
