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

#ifndef KERBALSYSTEM_HPP
#define KERBALSYSTEM_HPP

#include "math/constants.hpp"

#include "../OrbitalSystem.hpp"

/*! \ingroup phys
 * [The Kerbol System](https://wiki.kerbalspaceprogram.com/wiki/Kerbol_System)
 * from [Kerbal Space Program](https://kerbalspaceprogram.com/).
 *
 * This class is doomed to be replaced by a config file. Orbital Systems still
 * don't support loading/saving to/from files so for now a few of them have
 * been hardcoded (see also \ref SolarSystem).
 *
 * This system is implemented to show that Prograde isn't necessarly tied to a
 * real star system and can simulate a wide variety of systems. It is also a
 * tribute to KSP.
 *
 * See \ref phys group description for conventions and notations.
 */
class KerbalSystem : public OrbitalSystem
{
  public:
	/*! Default Constructor
	 *
	 * Populates itself with some of the Kerbol System data.
	 */
	KerbalSystem();

  private:
	void createPlanets();
	void createEveSubSystem();
	void createKerbinSubSystem();
	void createDunaSubSystem();
	void createJoolSubSystem();

	double distanceMultiplier = 1.f;
	double radiusMultiplier   = 1000.f;
	double massMultiplier     = 1.f;
};

#endif // KERBALSYSTEM_HPP
