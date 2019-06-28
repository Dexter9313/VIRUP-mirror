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

#ifndef SOLARSYSTEM_HPP
#define SOLARSYSTEM_HPP

#include <QCoreApplication>
#include <QProgressDialog>

#include "math/constants.hpp"

#include "../OrbitalSystem.hpp"

/*! \ingroup phys
 * [The orbital system we live in](https://en.wikipedia.org/wiki/Solar_System).
 * (As of 2018 anyway... ;) )
 *
 * This class is doomed to be replaced by a config file. Orbital Systems still
 * don't support loading/saving to/from files so for now a few of them have
 * been hardcoded (see also \ref KerbalSystem).
 *
 * For now, only the eight major planets, Pluto and a few moons
 * (the Moon, Phobos and Deimos) have been included.
 *
 * See \ref phys group description for conventions and notations.
 */
class SolarSystem : public OrbitalSystem
{
  public:
	/*! Default Constructor
	 *
	 * Populates itself with some of the real Solar System data.
	 */
	SolarSystem();

  private:
	void createPlanets();
	void createEarthSubSystem();
	void createMarsSubSystem();
	void createAsteroidBeltSubSystem();
	void createJupiterSubSystem();
	void createSaturnSubSystem();
	void createUranusSubSystem();
	void createNeptuneSubSystem();
	void createPlutoSubSystem();
	void createTransNeptunianSubSystem();

	double au = 149597870700.0;
	double km = 1000.f;

	void createChild(std::string name,
	                 CelestialBody::Parameters const& physParams,
	                 Planet::Parameters const& planetParams,
	                 std::string const& parent = "");

	// TEMP
	QProgressDialog* progress;
	const unsigned int bodiesNb = 38;
	unsigned int current        = 0;

	void updateProgress();
};

#endif // SOLARSYSTEM_HPP
