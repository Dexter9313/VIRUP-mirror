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

#ifndef ORBIT_HPP
#define ORBIT_HPP

#include <QJsonObject>
#include <cmath>
#include <iostream>

#include "math/CoordinateSystem.hpp"
#include "math/EccentricAnomalySolver.hpp"
#include "math/constants.hpp"

#include "UniversalTime.hpp"

class Orbit
{
  public:
	struct Parameters
	{
		double inclination;
		double ascendingNodeLongitude;
		double periapsisArgument;
		double eccentricity;
		double semiMajorAxis; // or altitude of periapsis if parabolic orbit
		double meanAnomalyAtEpoch;
	};

	struct MassiveBodyMass
	{
		explicit MassiveBodyMass(double value)
		    : value(value){};
		double value;
	};

	struct Period
	{
		explicit Period(double value)
		    : value(value){};
		double value;
	};

  public:
	Orbit() = delete;
	Orbit(MassiveBodyMass const& massiveBodyMass, QJsonObject const& json);
	Orbit(MassiveBodyMass const& massiveBodyMass, Parameters parameters);
	Orbit(Period const& period, Parameters parameters);
	Orbit(Orbit const& copiedOrbit) = default;
	bool isValid() const { return valid; };
	virtual bool isLoadedFromFile() const { return false; };
	virtual void updateParameters(UniversalTime uT) { (void) uT; };
	double getMassiveBodyMass() const;
	Parameters getParameters() const;
	double getPeriod() const;
	// defined as unit vector pointing along the angular momentum of the orbit
	Vector3 getNorth() const;
	virtual double getMeanAnomalyAtUT(UniversalTime uT);
	double getEccentricAnomalyAtUT(UniversalTime uT);
	double getTrueAnomalyAtUT(UniversalTime uT);
	double getMassiveBodyDistanceAtUT(UniversalTime uT);
	Vector3 getPositionAtUT(UniversalTime uT);
	// Vector3 getVelocityAtUT(UniversalTime uT) const;
	CoordinateSystem getRelativeCoordinateSystemAtUT(UniversalTime uT);
	std::ostream& displayAsText(std::ostream& stream) const;
	QJsonObject getJSONRepresentation() const;
	virtual ~Orbit(){};

	static double SMAfromPeriodMass(double period, double massiveBodyMass);
	static double massiveBodyMassFromElements(double semiMajorAxis,
	                                          double period);

  protected:
	void updatePeriod();

	Parameters parameters;

  private:
	bool valid = false;
	double massiveBodyMass;

	// period is precomputed as it is often used and doesn't change
	double period;

	// cache
	UniversalTime cacheUT;
	Vector3 position;
};

std::ostream& operator<<(std::ostream& stream, Orbit const& orbit);

#endif // ORBIT_HPP
