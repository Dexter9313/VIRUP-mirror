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

#ifndef CSVORBIT_HPP
#define CSVORBIT_HPP

#include <QDir>
#include <QSettings>
#include <map>
#include <string>

#include "Orbit.hpp"

class CSVOrbit : public Orbit
{
  public:
	CSVOrbit(MassiveBodyMass const& massiveBodyMass,
	         std::string const& bodyName);
	virtual bool isLoadedFromFile() const override { return true; };
	virtual void updateParameters(UniversalTime uT) override;
	virtual double getMeanAnomalyAtUT(UniversalTime uT) override
	{
		updateParameters(uT);
		return parameters.meanAnomalyAtEpoch;
	};
	static bool csvExistsFor(std::string const& name);

  private:
	static double interpolateAngle(double before, double after, double frac);
	static double interpolateAngleAlwaysForward(double before, double after,
	                                            double frac);

	std::map<int64_t, Orbit::Parameters> parametersHistory;
	std::string bodyName;
};

#endif // CSVORBIT_HPP
