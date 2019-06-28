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

#ifndef CELESTIALBODY_HPP
#define CELESTIALBODY_HPP

#include <QJsonArray>
#include <QJsonObject>
#include <cmath>
#include <vector>

#include "CSVOrbit.hpp"
#include "Color.hpp"
#include "Orbit.hpp"
#include "Orbitable.hpp"
#include "math/MathUtils.hpp"

class CelestialBody : public Orbitable
{
  public:
	enum class Type
	{
		PLANET,
		STAR,
	};

	struct Parameters
	{
		double mass;   // kg
		double radius; // m
		Vector3 oblateness = Vector3(1.0, 1.0, 1.0);
		Color color;

		/* ROTATION */
		double siderealTimeAtEpoch = 0.0; // angle between FP of Aries and Prime
		                                  // Meridian in radians
		double siderealRotationPeriod = DBL_MAX; // in seconds

		double northPoleRightAsc    = 0.0;                // in rad
		double northPoleDeclination = constant::pi / 2.0; // in rad
	};

	CelestialBody(Type type, QJsonObject const& json,
	              OrbitalSystem const& system);
	CelestialBody(Type type, QJsonObject const& json, Orbitable const& parent);
	CelestialBody(Type type, std::string const name,
	              Parameters const& parameters, Orbitable const& parent,
	              Orbit* orbit = nullptr);
	CelestialBody(Type type, std::string const name,
	              Parameters const& parameters, OrbitalSystem const& system,
	              Orbit* orbit = nullptr);

	Type getCelestialBodyType() const { return type; };
	Parameters const& getCelestialBodyParameters() const { return parameters; };
	double getPrimeMeridianSiderealTimeAtUT(UniversalTime uT) const;
	double getEscapeVelocity(double altitude = 0.0) const;
	double getSphereOfInfluenceRadius() const;
	// Roche limit for a fluid of 500kg/m^3
	// see https://en.wikipedia.org/wiki/Roche_limit
	// rings can be more or less within this limit (above it would form moons)
	double getMaximumRocheLimit() const;
	virtual ~CelestialBody(){};

	virtual QJsonObject getJSONRepresentation() const override;

  protected:
	Parameters parameters;

  private:
	Type type;

	void parseJSON(QJsonObject const& json);
	static Orbitable::Type orbitableType(Type celestialBodyType);
};

#endif // CELESTIALBODY_HPP
