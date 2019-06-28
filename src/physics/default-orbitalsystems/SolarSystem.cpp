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
#include "../../../include/physics/default-orbitalsystems/SolarSystem.hpp"

double sunMass            = 1.9891e30;
double sunRadius          = 696342.0 * 1000.0;
double sunTemperature     = 5778.0;
double solDeclinationTilt = 23.4392811 * constant::pi / 180.0;

SolarSystem::SolarSystem()
    : OrbitalSystem("Solar System")
{
	CelestialBody::Parameters physicalParams;
	physicalParams.mass   = sunMass;
	physicalParams.radius = sunRadius;
	rootOrbitable   = new Star("Sun", physicalParams, sunTemperature, *this);
	declinationTilt = solDeclinationTilt;

	indexNewOrbitable(rootOrbitable);

	progress = new QProgressDialog(QObject::tr("Loading Solar System..."),
	                               QString(), 0, bodiesNb);
	createPlanets();
	createEarthSubSystem();
	createMarsSubSystem();
	createAsteroidBeltSubSystem();
	createJupiterSubSystem();
	createSaturnSubSystem();
	createUranusSubSystem();
	createNeptuneSubSystem();
	createPlutoSubSystem();
	createTransNeptunianSubSystem();
	delete progress;
}

void SolarSystem::createPlanets()
{
	CelestialBody::Parameters physicalParams;
	Planet::Parameters planetParams;

	// for siderealTimeAtEpoch see data/prograde/physics/siderealTimesAtEpoch.py

	// mercury
	physicalParams.radius = 2439.7 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 1.0);
	physicalParams.color                  = Color(75, 77, 80);
	physicalParams.siderealTimeAtEpoch    = 6.069321130640849;
	physicalParams.siderealRotationPeriod = 5067014.4;
	physicalParams.northPoleRightAsc      = constant::pi * 281.01 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 61.45 / 180.0;
	createChild("Mercury", physicalParams, planetParams);
	updateProgress();

	// venus
	physicalParams.radius = (6051.8 + 50.0) * km; // add atmosphere
	physicalParams.oblateness.setXYZ(1.0, 1.0, 1.0);
	physicalParams.color               = Color(244, 190, 98);
	physicalParams.siderealTimeAtEpoch = 3.3587532198310948;
	// physicalParams.siderealRotationPeriod = 20997360.0;
	// ~4.4d ; we actually want the Atmosphere rotation period for visualization
	physicalParams.siderealRotationPeriod = 380234.594;
	physicalParams.northPoleRightAsc      = constant::pi * 92.76 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * -67.16 / 180.0;
	createChild("Venus", physicalParams, planetParams);
	updateProgress();

	// earth
	physicalParams.radius = 6378.1 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 6356.8 / 6378.1);
	physicalParams.color                  = Color(69, 91, 112);
	physicalParams.mass                   = 5.97219 * 1e24;
	physicalParams.siderealTimeAtEpoch    = 1.7593363924107448;
	physicalParams.siderealRotationPeriod = 86164.0905;
	physicalParams.northPoleRightAsc      = constant::pi * 0.0 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 90.0 / 180.0;
	planetParams.atmosphere               = 0.3;
	createChild("Earth", physicalParams, planetParams);
	updateProgress();

	// mars
	physicalParams.radius = 3396.2 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 3376.2 / 3396.2);
	physicalParams.color                  = Color(163, 108, 55);
	physicalParams.mass                   = 6.4171 * 1e23;
	physicalParams.siderealTimeAtEpoch    = 0.7040061418814729;
	physicalParams.siderealRotationPeriod = 88642.6848;
	physicalParams.northPoleRightAsc      = constant::pi * 317.68143 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 52.88650 / 180.0;
	planetParams.atmosphere               = 0.075;
	createChild("Mars", physicalParams, planetParams);
	updateProgress();

	// jupiter
	physicalParams.radius = 71492.0 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 66854.0 / 71492.0);
	physicalParams.color = Color(141, 152, 149);
	physicalParams.mass  = 1.89813 * 1e27;
	// physicalParams.siderealTimeAtEpoch   = 4.667796671159006;
	// physicalParams.siderealRotationPeriod = 35730.0;
	// we actually want the Red Spot rotation period for visualization
	// extrapolated from https://www.calsky.com/cs.cgi?cha=7&sec=6&sub=1
	// predictions over a year
	// a lot of eyeballing also !
	physicalParams.siderealTimeAtEpoch    = constant::pi * 140.0 / 180.0;
	physicalParams.siderealRotationPeriod = 35744.20;
	physicalParams.northPoleRightAsc      = constant::pi * 268.057 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 64.495 / 180.0;
	planetParams.atmosphere               = 0.0;
	createChild("Jupiter", physicalParams, planetParams);
	updateProgress();

	// saturn
	physicalParams.radius = 60268.0 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 54359.0 / 60268.0);
	physicalParams.color                  = Color(240, 219, 154);
	physicalParams.mass                   = 5.68319 * 1e26;
	physicalParams.siderealTimeAtEpoch    = 1.4399218441265502;
	physicalParams.siderealRotationPeriod = 37980.0;
	physicalParams.northPoleRightAsc      = constant::pi * 40.589 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 83.537 / 180.0;
	planetParams.innerRing                = 73788.72 * km;
	planetParams.outerRing                = 139595.64 * km;
	createChild("Saturn", physicalParams, planetParams);
	updateProgress();

	// uranus
	physicalParams.radius = 25559.0 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 24973.0 / 25559.0);
	physicalParams.color                  = Color(185, 212, 222);
	physicalParams.mass                   = 8.681 * 1e25;
	physicalParams.siderealTimeAtEpoch    = 0.22880273383759028;
	physicalParams.siderealRotationPeriod = 62063.712;
	physicalParams.northPoleRightAsc      = constant::pi * 77.311 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 15.175 / 180.0;
	planetParams.innerRing                = 73788.72 * km;
	planetParams.outerRing                = 139595.64 * km;
	createChild("Uranus", physicalParams, planetParams);
	updateProgress();

	// neptune
	physicalParams.radius = 24764.0 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 24341.0 / 24764.0);
	physicalParams.color                  = Color(97, 142, 232);
	physicalParams.mass                   = 1.0241 * 1e26;
	physicalParams.siderealTimeAtEpoch    = 0.29515412512381;
	physicalParams.siderealRotationPeriod = 58000.32;
	physicalParams.northPoleRightAsc      = constant::pi * 299.3 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 42.950 / 180.0;
	planetParams.innerRing                = 0.0;
	planetParams.outerRing                = 0.0;
	createChild("Neptune", physicalParams, planetParams);
	updateProgress();

	// pluto
	physicalParams.radius = 1187.0 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 1.0);
	physicalParams.color                  = Color(151, 106, 66);
	physicalParams.mass                   = 1.30900 * 1e22;
	physicalParams.siderealTimeAtEpoch    = 1.3928349482951936;
	physicalParams.siderealRotationPeriod = 551856.672;
	physicalParams.northPoleRightAsc      = constant::pi * 132.993 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * -6.163 / 180.0;
	createChild("Pluto", physicalParams, planetParams);
	updateProgress();
}

void SolarSystem::createEarthSubSystem()
{
	CelestialBody::Parameters physicalParams;

	// moon
	physicalParams.radius = 1738.1 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 1736.0 / 1738.1);
	physicalParams.color                  = Color(153, 146, 146);
	physicalParams.siderealTimeAtEpoch    = 0.6009311785663192;
	physicalParams.siderealRotationPeriod = 2360591.5104;
	physicalParams.northPoleRightAsc      = constant::pi * 266.86 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 65.64 / 180.0;
	createChild("Moon", physicalParams, {}, "Earth");
	updateProgress();
}

void SolarSystem::createMarsSubSystem()
{
	CelestialBody::Parameters physicalParams;

	// phobos
	physicalParams.radius                 = 11.2667 * km;
	physicalParams.color                  = Color(113, 112, 110);
	physicalParams.siderealTimeAtEpoch    = 4.023685663761321;
	physicalParams.siderealRotationPeriod = 27553.843872;
	physicalParams.northPoleRightAsc      = constant::pi * 270.0 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 66.5 / 180.0;
	createChild("Phobos", physicalParams, {}, "Mars");
	updateProgress();

	// deimos
	physicalParams.radius                 = 6.2 * km;
	physicalParams.color                  = Color(156, 137, 111);
	physicalParams.siderealTimeAtEpoch    = 5.852159600397493;
	physicalParams.siderealRotationPeriod = 109123.2;
	physicalParams.northPoleRightAsc      = constant::pi * 270.0 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 66.5 / 180.0;
	createChild("Deimos", physicalParams, {}, "Mars");
	updateProgress();
}

void SolarSystem::createAsteroidBeltSubSystem()
{
	CelestialBody::Parameters physicalParams;

	// ceres
	physicalParams.radius = 481.6 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 445.6 / 481.6);
	physicalParams.color                  = Color(54, 57, 48);
	physicalParams.siderealTimeAtEpoch    = 3.422892798391968;
	physicalParams.siderealRotationPeriod = 32667.84;
	physicalParams.northPoleRightAsc      = constant::pi * 291.42744 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 66.764 / 180.0;
	createChild("Ceres", physicalParams, {});
	updateProgress();

	// pallas
	physicalParams.radius                 = 256.0 * km;
	physicalParams.color                  = Color(54, 54, 54);
	physicalParams.siderealTimeAtEpoch    = 3.532654368722707;
	physicalParams.siderealRotationPeriod = 28127.52;
	// kind of random values
	physicalParams.northPoleRightAsc    = constant::pi * 0.0 / 180.0;
	physicalParams.northPoleDeclination = constant::pi * 10.0 / 180.0;
	createChild("Pallas", physicalParams, {});
	updateProgress();

	// juno
	physicalParams.radius = 135.7 * km;
	physicalParams.color  = Color(63, 63, 63);
	// UNKNOWN
	physicalParams.siderealTimeAtEpoch    = 0.0;
	physicalParams.siderealRotationPeriod = 25954.56;
	// kind of random values
	physicalParams.northPoleRightAsc    = constant::pi * 0.0 / 180.0;
	physicalParams.northPoleDeclination = constant::pi * 10.0 / 180.0;
	createChild("Juno", physicalParams, {});
	updateProgress();

	// vesta
	physicalParams.radius = 262.7 * km;
	physicalParams.color  = Color(89, 89, 89);
	// UNKNOWN
	physicalParams.siderealTimeAtEpoch    = 0.0;
	physicalParams.siderealRotationPeriod = 19232.64;
	physicalParams.northPoleRightAsc      = constant::pi * 308.0 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 48.0 / 180.0;
	createChild("Vesta", physicalParams, {});
	updateProgress();
}

void SolarSystem::createJupiterSubSystem()
{
	CelestialBody::Parameters physicalParams;

	// io
	physicalParams.radius                 = 1821.6 * km;
	physicalParams.color                  = Color(208, 189, 152);
	physicalParams.siderealTimeAtEpoch    = 1.7118221447865978;
	physicalParams.siderealRotationPeriod = 152853.50471;
	physicalParams.northPoleRightAsc      = constant::pi * 268.057 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 64.495 / 180.0;
	createChild("Io", physicalParams, {}, "Jupiter");
	updateProgress();

	// europa
	physicalParams.radius                 = 1560.8 * km;
	physicalParams.color                  = Color(230, 219, 189);
	physicalParams.siderealTimeAtEpoch    = 6.008339408633722;
	physicalParams.siderealRotationPeriod = 306822.0384;
	physicalParams.northPoleRightAsc      = constant::pi * 268.057 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 64.495 / 180.0;
	createChild("Europa", physicalParams, {}, "Jupiter");
	updateProgress();

	// ganymede
	physicalParams.radius                 = 2634.1 * km;
	physicalParams.color                  = Color(130, 124, 103);
	physicalParams.siderealTimeAtEpoch    = 0.3161974813725083;
	physicalParams.siderealRotationPeriod = 618153.375744;
	physicalParams.northPoleRightAsc      = constant::pi * 268.057 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 64.495 / 180.0;
	createChild("Ganymede", physicalParams, {}, "Jupiter");
	updateProgress();

	// callisto
	physicalParams.radius                 = 2410.3 * km;
	physicalParams.color                  = Color(129, 99, 51);
	physicalParams.siderealTimeAtEpoch    = 4.322661995256511;
	physicalParams.siderealRotationPeriod = 1441931.18976;
	physicalParams.northPoleRightAsc      = constant::pi * 268.057 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 64.495 / 180.0;
	createChild("Callisto", physicalParams, {}, "Jupiter");
	updateProgress();
}

void SolarSystem::createSaturnSubSystem()
{
	CelestialBody::Parameters physicalParams;

	// mimas
	physicalParams.radius                 = 198.2 * km;
	physicalParams.color                  = Color(116, 117, 116);
	physicalParams.siderealTimeAtEpoch    = 1.2155833485761178;
	physicalParams.siderealRotationPeriod = 81388.8;
	physicalParams.northPoleRightAsc      = constant::pi * 40.589 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 83.537 / 180.0;
	createChild("Mimas", physicalParams, {}, "Saturn");
	updateProgress();

	// enceladus
	physicalParams.radius                 = 252.1 * km;
	physicalParams.color                  = Color(156, 156, 155);
	physicalParams.siderealTimeAtEpoch    = 0.13550134480931364;
	physicalParams.siderealRotationPeriod = 118386.8352;
	physicalParams.northPoleRightAsc      = constant::pi * 40.589 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 83.537 / 180.0;
	createChild("Enceladus", physicalParams, {}, "Saturn");
	updateProgress();

	// tethys
	physicalParams.radius                 = 531.1 * km;
	physicalParams.color                  = Color(166, 166, 156);
	physicalParams.siderealTimeAtEpoch    = 0.8407241612918419;
	physicalParams.siderealRotationPeriod = 163106.0928;
	physicalParams.northPoleRightAsc      = constant::pi * 40.589 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 83.537 / 180.0;
	createChild("Tethys", physicalParams, {}, "Saturn");
	updateProgress();

	// dione
	physicalParams.radius                 = 561.9 * km;
	physicalParams.color                  = Color(126, 126, 123);
	physicalParams.siderealTimeAtEpoch    = 1.1236765986768573;
	physicalParams.siderealRotationPeriod = 236469.456;
	physicalParams.northPoleRightAsc      = constant::pi * 40.589 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 83.537 / 180.0;
	createChild("Dione", physicalParams, {}, "Saturn");
	updateProgress();

	// rhea
	physicalParams.radius                 = 763.8 * km;
	physicalParams.color                  = Color(106, 107, 94);
	physicalParams.siderealTimeAtEpoch    = 1.4800620020294997;
	physicalParams.siderealRotationPeriod = 390373.5168;
	physicalParams.northPoleRightAsc      = constant::pi * 40.589 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 83.537 / 180.0;
	createChild("Rhea", physicalParams, {}, "Saturn");
	updateProgress();

	// titan
	physicalParams.radius = 2574.73 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 1.0);
	physicalParams.color                  = Color(254, 175, 56);
	physicalParams.siderealTimeAtEpoch    = 5.251245365815998;
	physicalParams.siderealRotationPeriod = 1377648.0;
	physicalParams.northPoleRightAsc      = constant::pi * 40.589 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 83.537 / 180.0;
	createChild("Titan", physicalParams, {}, "Saturn");
	updateProgress();

	// hyperion
	physicalParams.radius = 135.0 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 1.0);
	physicalParams.color = Color(187, 142, 101);
	// UNKNOWN
	physicalParams.siderealTimeAtEpoch    = 0.0;
	physicalParams.siderealRotationPeriod = 1123200.0;
	// kind of random values
	physicalParams.northPoleRightAsc    = constant::pi * 40.589 / 180.0;
	physicalParams.northPoleDeclination = constant::pi * 10.537 / 180.0;
	createChild("Hyperion", physicalParams, {}, "Saturn");
	updateProgress();

	// iapetus
	physicalParams.radius                 = 734.5 * km;
	physicalParams.color                  = Color(131, 123, 116);
	physicalParams.siderealTimeAtEpoch    = 0.6189284346518459;
	physicalParams.siderealRotationPeriod = 6853377.6;
	physicalParams.northPoleRightAsc      = constant::pi * 40.589 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 83.537 / 180.0;
	createChild("Iapetus", physicalParams, {}, "Saturn");
	updateProgress();
}

void SolarSystem::createUranusSubSystem()
{
	CelestialBody::Parameters physicalParams;

	// miranda
	physicalParams.radius = 237.1 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 232.9 / 237.1);
	physicalParams.color                  = Color(142, 133, 127);
	physicalParams.siderealTimeAtEpoch    = 3.4079336130672515;
	physicalParams.siderealRotationPeriod = 122124.5856;
	physicalParams.northPoleRightAsc      = constant::pi * 77.311 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 15.175 / 180.0;
	createChild("Miranda", physicalParams, {}, "Uranus");
	updateProgress();

	// ariel
	physicalParams.radius = 579.5 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 577.7 / 579.5);
	physicalParams.color                  = Color(132, 116, 116);
	physicalParams.siderealTimeAtEpoch    = 0.7353285244469319;
	physicalParams.siderealRotationPeriod = 217728.0;
	physicalParams.northPoleRightAsc      = constant::pi * 77.311 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 15.175 / 180.0;
	createChild("Ariel", physicalParams, {}, "Uranus");
	updateProgress();

	// umbriel
	physicalParams.radius = 584.7 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 1.0);
	physicalParams.color                  = Color(84, 81, 78);
	physicalParams.siderealTimeAtEpoch    = 3.313362003400425;
	physicalParams.siderealRotationPeriod = 358041.6;
	physicalParams.northPoleRightAsc      = constant::pi * 77.311 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 15.175 / 180.0;
	createChild("Umbriel", physicalParams, {}, "Uranus");
	updateProgress();

	// titania
	physicalParams.radius = 788.4 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 1.0);
	physicalParams.color                  = Color(126, 106, 92);
	physicalParams.siderealTimeAtEpoch    = 4.618360842157154;
	physicalParams.siderealRotationPeriod = 752218.6176;
	physicalParams.northPoleRightAsc      = constant::pi * 77.311 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 15.175 / 180.0;
	createChild("Titania", physicalParams, {}, "Uranus");
	updateProgress();

	// oberon
	physicalParams.radius = 761.4 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 1.0);
	physicalParams.color                  = Color(102, 93, 81);
	physicalParams.siderealTimeAtEpoch    = 5.862611035753226;
	physicalParams.siderealRotationPeriod = 1163223.4176;
	physicalParams.northPoleRightAsc      = constant::pi * 77.311 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 15.175 / 180.0;
	createChild("Oberon", physicalParams, {}, "Uranus");
	updateProgress();
}

void SolarSystem::createNeptuneSubSystem()
{
	CelestialBody::Parameters physicalParams;

	// triton
	physicalParams.radius = 1353.4 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 1.0);
	physicalParams.color                  = Color(167, 153, 125);
	physicalParams.siderealTimeAtEpoch    = 0.5654432608671653;
	physicalParams.siderealRotationPeriod = 507772.8;
	physicalParams.northPoleRightAsc      = constant::pi * 119.3 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * -42.950 / 180.0;
	createChild("Triton", physicalParams, {}, "Neptune");
	updateProgress();
}

void SolarSystem::createPlutoSubSystem()
{
	CelestialBody::Parameters physicalParams;

	// charon
	physicalParams.radius = 606.0 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 1.0);
	physicalParams.color                  = Color(119, 103, 94);
	physicalParams.siderealTimeAtEpoch    = 4.536447817866964;
	physicalParams.siderealRotationPeriod = 551856.70656;
	physicalParams.northPoleRightAsc      = constant::pi * 132.993 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * -6.163 / 180.0;
	createChild("Charon", physicalParams, {}, "Pluto");
	updateProgress();
}

void SolarSystem::createTransNeptunianSubSystem()
{
	CelestialBody::Parameters physicalParams;
	Planet::Parameters planetParams;

	// Actual orientations are unknown

	// eris
	physicalParams.radius = 1163.0 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 1.0);
	physicalParams.color                  = Color(214, 204, 184);
	physicalParams.siderealTimeAtEpoch    = 0.0;
	physicalParams.siderealRotationPeriod = 93240.0;
	physicalParams.northPoleRightAsc      = constant::pi * 0.0 / 180.0;
	physicalParams.northPoleDeclination   = constant::pi * 90.0 / 180.0;
	createChild("Eris", physicalParams, {});
	updateProgress();

	// makemake
	physicalParams.radius = 725.0 * km;
	physicalParams.oblateness.setXYZ(1.0, 1.0, 1.0);
	physicalParams.color = Color(106, 86, 78);
	// physicalParams.siderealTimeAtEpoch    = ;
	physicalParams.siderealRotationPeriod = 82175.76;
	// physicalParams.northPoleRightAsc      = constant::pi * / 180.0;
	// physicalParams.northPoleDeclination   = constant::pi * / 180.0;
	createChild("Makemake", physicalParams, {});
	updateProgress();

	// haumea
	physicalParams.radius = 960.0 * km;
	physicalParams.oblateness.setXYZ(960.0 / 960.0, 770.0 / 960.0,
	                                 495.0 / 960.0);
	physicalParams.color   = Color(215, 205, 191);
	planetParams.innerRing = 2252.0 * km;
	planetParams.outerRing = 2322.0 * km;
	// physicalParams.siderealTimeAtEpoch    = ;
	physicalParams.siderealRotationPeriod = 14095.8144;
	// physicalParams.northPoleRightAsc      = constant::pi * / 180.0;
	// physicalParams.northPoleDeclination   = constant::pi * / 180.0;
	createChild("Haumea", physicalParams, planetParams);

	planetParams.innerRing = 0.0;
	planetParams.outerRing = 0.0;
	updateProgress();
}

void SolarSystem::createChild(std::string name,
                              CelestialBody::Parameters const& physParams,
                              Planet::Parameters const& planetParams,
                              std::string const& parent)
{
	Orbitable* parentPtr(parent.empty() ? rootOrbitable : (*this)[parent]);
	auto parentBody(dynamic_cast<CelestialBody*>(parentPtr));
	Planet* planet = new Planet(
	    name, physParams, planetParams, *parentPtr,
	    new CSVOrbit(Orbit::MassiveBodyMass(
	                     parentBody->getCelestialBodyParameters().mass),
	                 name));
	addChild(planet, parentPtr->getName());
}

void SolarSystem::updateProgress()
{
	++current;
	QCoreApplication::processEvents();
	progress->setValue(current);
}
