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

#include "physics/CSVOrbit.hpp"

CSVOrbit::CSVOrbit(MassiveBodyMass const& massiveBodyMass,
                   std::string const& bodyName)
    : Orbit(MassiveBodyMass(massiveBodyMass),
            Parameters({0.0, 0.0, 0.0, 0.0, 1.0, 0.0}))
    , bodyName(bodyName)
{
	if(bodyName.empty())
	{
		std::cerr << std::string(
		                 "Trying to load CSV orbit files from an empty name...")
		          << std::endl;
		exit(EXIT_FAILURE);
	}

	QString path(QSettings().value("simulation/planetsystemdir").toString()
	             + "/orbital-params/");
	path += bodyName.c_str();

	QDir csvdir(path);
	if(!csvdir.exists())
	{
		std::cerr << std::string("No CSV orbital params directory related to ")
		                 + bodyName + "..."
		          << std::endl;
		exit(EXIT_FAILURE);
	}
	QStringList csvfiles
	    = csvdir.entryList(QStringList() << "*.csv", QDir::Files);
	for(auto filePath : csvfiles)
	{
		QFile file(path + "/" + filePath);
		file.open(QIODevice::ReadOnly);
		file.readLine(); // discard header
		while(!file.atEnd())
		{
			QStringList entry
			    = QString(file.readLine()).simplified().split(',');

			parametersHistory[entry[0].toDouble()] = Parameters({
			    entry[1].toDouble(),
			    entry[2].toDouble(),
			    entry[3].toDouble(),
			    entry[4].toDouble(),
			    entry[5].toDouble(),
			    entry[6].toDouble(),
			});
		}
	}

	updateParameters(0.0);
}

void CSVOrbit::updateParameters(UniversalTime uT)
{
	int64_t day(static_cast<int64_t>(uT) / static_cast<int64_t>(24 * 3600));
	if(uT < 0.0)
	{
		--day;
	}

	auto it1(--parametersHistory.upper_bound(day)),
	    it2(parametersHistory.upper_bound(day));

	if(it2 == parametersHistory.begin())
	{
		parameters                    = it2->second;
		parameters.meanAnomalyAtEpoch = parametersHistory[0].meanAnomalyAtEpoch;
		updatePeriod();
	}
	else if(it2 == parametersHistory.end())
	{
		parameters                    = it1->second;
		parameters.meanAnomalyAtEpoch = parametersHistory[0].meanAnomalyAtEpoch;
		updatePeriod();
	}
	else
	{
		UniversalTime beg(it1->first * 24 * 3600), end(it2->first * 24 * 3600);
		UniversalTime intervalUT(end - beg);
		double interval(intervalUT);
		UniversalTime fracUT((uT - beg) / interval);
		double frac(fracUT);
		Parameters p1(it1->second), p2(it2->second);

		parameters.inclination
		    = interpolateAngle(p1.inclination, p2.inclination, frac);
		parameters.ascendingNodeLongitude = interpolateAngle(
		    p1.ascendingNodeLongitude, p2.ascendingNodeLongitude, frac);

		// Tethys has an orbit pretty much impossible to interpolate :
		// eccentricity is so low that periapsisArgument moves Very fast
		// and meanAnomaly behavior is then changed
		if(bodyName == "Tethys")
		{
			parameters.periapsisArgument = interpolateAngleAlwaysForward(
			    p1.periapsisArgument, p2.periapsisArgument, frac);
		}
		else
		{
			parameters.periapsisArgument = interpolateAngle(
			    p1.periapsisArgument, p2.periapsisArgument, frac);
		}

		parameters.eccentricity
		    = (1.0 - frac) * p1.eccentricity + frac * p2.eccentricity;
		parameters.semiMajorAxis
		    = (1.0 - frac) * p1.semiMajorAxis + frac * p2.semiMajorAxis;

		updatePeriod();

		if(2.0 * interval < getPeriod() || bodyName == "Tethys")
		{
			parameters.meanAnomalyAtEpoch = interpolateAngle(
			    p1.meanAnomalyAtEpoch, p2.meanAnomalyAtEpoch, frac);
		}
		else
		{
			parameters.meanAnomalyAtEpoch = interpolateAngleAlwaysForward(
			    p1.meanAnomalyAtEpoch, p2.meanAnomalyAtEpoch, frac);
		}

		if(getPeriod() < interval)
		{
			parameters.meanAnomalyAtEpoch
			    = p1.meanAnomalyAtEpoch
			      + 2.0 * constant::pi * (frac * interval) / getPeriod();

			parameters.meanAnomalyAtEpoch
			    -= floor(parameters.meanAnomalyAtEpoch / (2.0 * constant::pi))
			       * 2.0 * constant::pi;

			double MAatEnd(p2.meanAnomalyAtEpoch);
			double MAcomputed(p1.meanAnomalyAtEpoch
			                  + 2.0 * constant::pi * interval / getPeriod());

			while(MAatEnd < MAcomputed)
			{
				MAatEnd += 2.0 * constant::pi;
			}

			double error(MAatEnd - MAcomputed);
			error -= round(error / (2.0 * constant::pi)) * 2.0 * constant::pi;

			parameters.meanAnomalyAtEpoch += frac * error;
			parameters.meanAnomalyAtEpoch
			    -= floor(parameters.meanAnomalyAtEpoch / (2.0 * constant::pi))
			       * 2.0 * constant::pi;
		}
	}
}

bool CSVOrbit::csvExistsFor(std::string const& name)
{
	if(name.empty())
	{
		return false;
	}

	QString path(QSettings().value("simulation/planetsystemdir").toString()
	             + "/orbital-params/");
	path += name.c_str();

	QDir csvdir(path);
	return csvdir.exists();
}

double CSVOrbit::interpolateAngle(double before, double after, double frac)
{
	while(fabs(after - before) > 3.1416)
	{
		if(after < before)
		{
			after += 2.0 * constant::pi;
		}
		else
		{
			before += 2.0 * constant::pi;
		}
	}

	return (1.0 - frac) * before + frac * after;
}

double CSVOrbit::interpolateAngleAlwaysForward(double before, double after,
                                               double frac)
{
	if(after < before)
	{
		after += 2.0 * constant::pi;
	}

	return (1.0 - frac) * before + frac * after;
}
