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
#include "../../../include/physics/default-orbitalsystems/KerbalSystem.hpp"

double kerbolMass        = 1.7565670 * 1e28;
double kerbolRadius      = 261600000.0;
double kerbolTemperature = 5840.0;

KerbalSystem::KerbalSystem()
    : OrbitalSystem("Kerbal System")
{
	CelestialBody::Parameters physicalParams;
	physicalParams.mass   = kerbolMass;
	physicalParams.radius = kerbolRadius;
	rootOrbitable
	    = new Star("Kerbol", physicalParams, kerbolTemperature, *this);

	indexNewOrbitable(rootOrbitable);

	createPlanets();
	createEveSubSystem();
	createKerbinSubSystem();
	createDunaSubSystem();
	createJoolSubSystem();
}

void KerbalSystem::createPlanets()
{
	Orbit::Parameters orbitalParams;
	CelestialBody::Parameters physicalParams;
	Planet::Parameters planetParams;

	Orbit* orbit;
	Planet* planet;

	// moho
	orbitalParams.inclination            = constant::pi * 7.0 / 180.0;
	orbitalParams.ascendingNodeLongitude = constant::pi * 70.0 / 180.0;
	orbitalParams.periapsisArgument      = constant::pi * 15.0 / 180.0;
	orbitalParams.eccentricity           = 0.2;
	orbitalParams.semiMajorAxis          = 5263138304 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 3.14;
	physicalParams.radius                = 250.0 * radiusMultiplier;
	physicalParams.color                 = Color(121, 85, 72);
	orbit  = new Orbit(Orbit::MassiveBodyMass(kerbolMass), orbitalParams);
	planet = new Planet("Moho", physicalParams, planetParams, *rootOrbitable,
	                    orbit);
	addChild(planet, "Kerbol");

	// eve
	orbitalParams.inclination            = constant::pi * 2.1 / 180.0;
	orbitalParams.ascendingNodeLongitude = constant::pi * 15.0 / 180.0;
	orbitalParams.periapsisArgument      = constant::pi * 0.0 / 180.0;
	orbitalParams.eccentricity           = 0.01;
	orbitalParams.semiMajorAxis          = 9832684544 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 3.14;
	physicalParams.mass                  = 1.2244127 * 1e23 * massMultiplier;
	physicalParams.radius                = 700.0 * radiusMultiplier;
	physicalParams.color                 = Color(106, 27, 154);
	planetParams.type                    = Planet::Type::GAZGIANT;
	orbit  = new Orbit(Orbit::MassiveBodyMass(kerbolMass), orbitalParams);
	planet = new Planet("Eve", physicalParams, planetParams, *rootOrbitable,
	                    orbit);
	addChild(planet, "Kerbol");

	// kerbin
	orbitalParams.inclination            = constant::pi * 0.0 / 180.0;
	orbitalParams.ascendingNodeLongitude = constant::pi * 0.0 / 180.0;
	orbitalParams.periapsisArgument      = constant::pi * 0.0 / 180.0;
	orbitalParams.eccentricity           = 0.0;
	orbitalParams.semiMajorAxis          = 13599840256 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 3.14;
	physicalParams.mass                  = 5.2915793 * 1e22 * massMultiplier;
	physicalParams.radius                = 600.0 * radiusMultiplier;
	physicalParams.color                 = Color(40, 53, 147);
	planetParams.type                    = Planet::Type::TERRESTRIAL;
	planetParams.atmosphere              = 0.3;
	orbit  = new Orbit(Orbit::MassiveBodyMass(kerbolMass), orbitalParams);
	planet = new Planet("Kerbin", physicalParams, planetParams, *rootOrbitable,
	                    orbit);
	addChild(planet, "Kerbol");

	// duna
	orbitalParams.inclination            = constant::pi * 0.060 / 180.0;
	orbitalParams.ascendingNodeLongitude = constant::pi * 135.50 / 180.0;
	orbitalParams.periapsisArgument      = constant::pi * 0.0 / 180.0;
	orbitalParams.eccentricity           = 0.050;
	orbitalParams.semiMajorAxis          = 20726155264 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 3.14;
	physicalParams.mass                  = 4.5154812 * 1e21 * massMultiplier;
	physicalParams.radius                = 320.0 * radiusMultiplier;
	physicalParams.color                 = Color(244, 67, 54);
	planetParams.atmosphere              = 0.075;
	orbit  = new Orbit(Orbit::MassiveBodyMass(kerbolMass), orbitalParams);
	planet = new Planet("Duna", physicalParams, planetParams, *rootOrbitable,
	                    orbit);
	addChild(planet, "Kerbol");

	// dres
	orbitalParams.inclination            = constant::pi * 5.0 / 180.0;
	orbitalParams.ascendingNodeLongitude = constant::pi * 280.0 / 180.0;
	orbitalParams.periapsisArgument      = constant::pi * 90.0 / 180.0;
	orbitalParams.eccentricity           = 0.140;
	orbitalParams.semiMajorAxis          = 40839348203 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 314;
	physicalParams.radius                = 138.0 * radiusMultiplier;
	physicalParams.color                 = Color(158, 158, 158);
	orbit  = new Orbit(Orbit::MassiveBodyMass(kerbolMass), orbitalParams);
	planet = new Planet("Dres", physicalParams, planetParams, *rootOrbitable,
	                    orbit);
	addChild(planet, "Kerbol");

	// jool
	orbitalParams.inclination            = constant::pi * 1.3040 / 180.0;
	orbitalParams.ascendingNodeLongitude = constant::pi * 52.0 / 180.0;
	orbitalParams.periapsisArgument      = constant::pi * 0.0 / 180.0;
	orbitalParams.eccentricity           = 0.050;
	orbitalParams.semiMajorAxis          = 68773560320 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 0.1;
	physicalParams.mass                  = 4.2332635 * 1e24 * massMultiplier;
	physicalParams.radius                = 6000.0 * radiusMultiplier;
	physicalParams.color                 = Color(104, 159, 56);
	planetParams.type                    = Planet::Type::GAZGIANT;
	orbit  = new Orbit(Orbit::MassiveBodyMass(kerbolMass), orbitalParams);
	planet = new Planet("Jool", physicalParams, planetParams, *rootOrbitable,
	                    orbit);
	addChild(planet, "Kerbol");

	// eeloo
	orbitalParams.inclination            = constant::pi * 6.150 / 180.0;
	orbitalParams.ascendingNodeLongitude = constant::pi * 50.0 / 180.0;
	orbitalParams.periapsisArgument      = constant::pi * 260.0 / 180.0;
	orbitalParams.eccentricity           = 0.260;
	orbitalParams.semiMajorAxis          = 90118820000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 3.14;
	physicalParams.radius                = 210.0 * radiusMultiplier;
	physicalParams.color                 = Color(240, 240, 240);
	planetParams.type                    = Planet::Type::TERRESTRIAL;
	orbit  = new Orbit(Orbit::MassiveBodyMass(kerbolMass), orbitalParams);
	planet = new Planet("Eeloo", physicalParams, planetParams, *rootOrbitable,
	                    orbit);
	addChild(planet, "Kerbol");
}

void KerbalSystem::createEveSubSystem()
{
	Orbit::Parameters orbitalParams;
	CelestialBody::Parameters physicalParams;

	Orbitable* eve((*this)["Eve"]);
	double eveMass(
	    dynamic_cast<CelestialBody*>(eve)->getCelestialBodyParameters().mass);

	Orbit* orbit;
	Planet* planet;

	// gilly
	orbitalParams.inclination            = constant::pi * 12.0 / 180.0;
	orbitalParams.ascendingNodeLongitude = constant::pi * 80.0 / 180.0;
	orbitalParams.periapsisArgument      = constant::pi * 10.0 / 180.0;
	orbitalParams.eccentricity           = 0.550;
	orbitalParams.semiMajorAxis          = 31500000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 0.9;
	physicalParams.radius                = 13.0 * radiusMultiplier;
	physicalParams.color                 = Color(188, 170, 164);
	orbit  = new Orbit(Orbit::MassiveBodyMass(eveMass), orbitalParams);
	planet = new Planet("Gilly", physicalParams, {}, *eve, orbit);
	addChild(planet, "Eve");
}

void KerbalSystem::createKerbinSubSystem()
{
	Orbit::Parameters orbitalParams;
	CelestialBody::Parameters physicalParams;

	Orbitable* kerbin((*this)["Kerbin"]);
	double kerbinMass(dynamic_cast<CelestialBody*>(kerbin)
	                      ->getCelestialBodyParameters()
	                      .mass);

	Orbit* orbit;
	Planet* planet;

	// mun
	orbitalParams.inclination            = constant::pi * 0.0 / 180.0;
	orbitalParams.ascendingNodeLongitude = constant::pi * 0.0 / 180.0;
	orbitalParams.periapsisArgument      = constant::pi * 0.0 / 180.0;
	orbitalParams.eccentricity           = 0.0;
	orbitalParams.semiMajorAxis          = 12000000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 1.7;
	physicalParams.radius                = 200.0 * radiusMultiplier;
	physicalParams.color                 = Color(158, 158, 158);
	orbit  = new Orbit(Orbit::MassiveBodyMass(kerbinMass), orbitalParams);
	planet = new Planet("Mun", physicalParams, {}, *kerbin, orbit);
	addChild(planet, "Kerbin");

	// minmus
	orbitalParams.inclination            = constant::pi * 6.0 / 180.0;
	orbitalParams.ascendingNodeLongitude = constant::pi * 78.0 / 180.0;
	orbitalParams.periapsisArgument      = constant::pi * 38.0 / 180.0;
	orbitalParams.eccentricity           = 0.0;
	orbitalParams.semiMajorAxis          = 47000000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 0.9;
	physicalParams.radius                = 60.0 * radiusMultiplier;
	physicalParams.color                 = Color(178, 223, 219);
	orbit  = new Orbit(Orbit::MassiveBodyMass(kerbinMass), orbitalParams);
	planet = new Planet("Minmus", physicalParams, {}, *kerbin, orbit);
	addChild(planet, "Kerbin");
}

void KerbalSystem::createDunaSubSystem()
{
	Orbit::Parameters orbitalParams;
	CelestialBody::Parameters physicalParams;

	Orbitable* duna((*this)["Duna"]);
	double dunaMass(
	    dynamic_cast<CelestialBody*>(duna)->getCelestialBodyParameters().mass);

	Orbit* orbit;
	Planet* planet;

	// ike
	orbitalParams.inclination            = constant::pi * 0.2 / 180.0;
	orbitalParams.ascendingNodeLongitude = constant::pi * 0.0 / 180.0;
	orbitalParams.periapsisArgument      = constant::pi * 0.0 / 180.0;
	orbitalParams.eccentricity           = 0.030;
	orbitalParams.semiMajorAxis          = 3200000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 1.7;
	physicalParams.radius                = 130.0 * radiusMultiplier;
	physicalParams.color                 = Color(158, 158, 158);
	orbit  = new Orbit(Orbit::MassiveBodyMass(dunaMass), orbitalParams);
	planet = new Planet("Ike", physicalParams, {}, *duna, orbit);
	addChild(planet, "Duna");
}

void KerbalSystem::createJoolSubSystem()
{
	Orbit::Parameters orbitalParams;
	CelestialBody::Parameters physicalParams;
	Planet::Parameters planetParams;

	Orbitable* jool((*this)["Jool"]);
	double joolMass(
	    dynamic_cast<CelestialBody*>(jool)->getCelestialBodyParameters().mass);

	Orbit* orbit;
	Planet* planet;

	// laythe
	orbitalParams.inclination            = constant::pi * 0.0 / 180.0;
	orbitalParams.ascendingNodeLongitude = constant::pi * 0.0 / 180.0;
	orbitalParams.periapsisArgument      = constant::pi * 0.0 / 180.0;
	orbitalParams.eccentricity           = 0.0;
	orbitalParams.semiMajorAxis          = 27184000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 3.14;
	physicalParams.radius                = 500.0 * radiusMultiplier;
	physicalParams.color                 = Color(26, 35, 126);
	planetParams.atmosphere              = 0.3;
	orbit  = new Orbit(Orbit::MassiveBodyMass(joolMass), orbitalParams);
	planet = new Planet("Laythe", physicalParams, {}, *jool, orbit);
	addChild(planet, "Jool");

	// vall
	orbitalParams.inclination            = constant::pi * 0.0 / 180.0;
	orbitalParams.ascendingNodeLongitude = constant::pi * 0.0 / 180.0;
	orbitalParams.periapsisArgument      = constant::pi * 0.0 / 180.0;
	orbitalParams.eccentricity           = 0.0;
	orbitalParams.semiMajorAxis          = 43152000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 0.9;
	physicalParams.radius                = 300.0 * radiusMultiplier;
	physicalParams.color                 = Color(187, 222, 251);
	orbit  = new Orbit(Orbit::MassiveBodyMass(joolMass), orbitalParams);
	planet = new Planet("Vall", physicalParams, {}, *jool, orbit);
	addChild(planet, "Jool");

	// tylo
	orbitalParams.inclination            = constant::pi * 0.0250 / 180.0;
	orbitalParams.ascendingNodeLongitude = constant::pi * 0.0 / 180.0;
	orbitalParams.periapsisArgument      = constant::pi * 0.0 / 180.0;
	orbitalParams.eccentricity           = 0.0;
	orbitalParams.semiMajorAxis          = 68500000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 3.14;
	physicalParams.radius                = 600.0 * radiusMultiplier;
	physicalParams.color                 = Color(215, 204, 233);
	orbit  = new Orbit(Orbit::MassiveBodyMass(joolMass), orbitalParams);
	planet = new Planet("Tylo", physicalParams, {}, *jool, orbit);
	addChild(planet, "Jool");

	// bop
	orbitalParams.inclination            = constant::pi * 15.0 / 180.0;
	orbitalParams.ascendingNodeLongitude = constant::pi * 10.0 / 180.0;
	orbitalParams.periapsisArgument      = constant::pi * 25.0 / 180.0;
	orbitalParams.eccentricity           = 0.240;
	orbitalParams.semiMajorAxis          = 128500000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 0.9;
	physicalParams.radius                = 65.0 * radiusMultiplier;
	physicalParams.color                 = Color(121, 85, 72);
	orbit  = new Orbit(Orbit::MassiveBodyMass(joolMass), orbitalParams);
	planet = new Planet("Bop", physicalParams, {}, *jool, orbit);
	addChild(planet, "Jool");

	// pol
	orbitalParams.inclination            = constant::pi * 4.250 / 180.0;
	orbitalParams.ascendingNodeLongitude = constant::pi * 2.0 / 180.0;
	orbitalParams.periapsisArgument      = constant::pi * 15.0 / 180.0;
	orbitalParams.eccentricity           = 0.170;
	orbitalParams.semiMajorAxis          = 179890000 * distanceMultiplier;
	orbitalParams.meanAnomalyAtEpoch     = 0.9;
	physicalParams.radius                = 44.0 * radiusMultiplier;
	physicalParams.color                 = Color(215, 204, 233);
	orbit  = new Orbit(Orbit::MassiveBodyMass(joolMass), orbitalParams);
	planet = new Planet("Pol", physicalParams, {}, *jool, orbit);
	addChild(planet, "Jool");
}
