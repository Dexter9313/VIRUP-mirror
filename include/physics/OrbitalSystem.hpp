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

#ifndef ORBITALSYSTEM_HPP
#define ORBITALSYSTEM_HPP

#include <QCoreApplication>
#include <QJsonArray>
#include <QProgressDialog>
#include <QtDebug>
#include <map>
#include <vector>

#include "Orbitable.hpp"
#include "Planet.hpp"
#include "Star.hpp"

class OrbitalSystem
{
  protected:
	OrbitalSystem(std::string name);

  public:
	OrbitalSystem(std::string name, QJsonObject const& json);
	OrbitalSystem(std::string name, Orbitable* rootOrbitable,
	              double declinationTilt);
	OrbitalSystem(OrbitalSystem const&) = delete;
	OrbitalSystem& operator=(OrbitalSystem const&) = delete;
	bool isValid() const;

	void generateStarsNames();

	double getDeclinationTilt() const { return declinationTilt; };
	Orbitable const& getRootOrbitable() const { return *rootOrbitable; };

	std::pair<double, double> getRADecFromCarthesian(Vector3 dir) const;

	// will take ownership of child
	void addChild(Orbitable* child,
	              std::string const& parent = std::string(""));

	Orbitable* operator[](std::string const& name);
	Orbitable const* operator[](std::string const& name) const;

	std::vector<std::string> getAllOrbitablesNames() const;
	std::vector<Orbitable*> getAllOrbitablesPointers() const;

	std::vector<std::string> getAllBinariesNames() const;
	std::vector<Orbitable*> getAllBinariesPointers() const;

	std::vector<std::string> getAllCelestialBodiesNames() const;
	std::vector<CelestialBody*> getAllCelestialBodiesPointers() const;

	std::vector<std::string> getAllStarsNames() const;
	std::vector<Star*> getAllStarsPointers() const;

	std::vector<std::string> getAllPlanetsNames() const;
	std::vector<Planet*> getAllPlanetsPointers() const;

	// planets that don't orbit other planets
	std::vector<std::string> getAllFirstClassPlanetsNames() const;
	std::vector<Planet*> getAllFirstClassPlanetsPointers() const;

	// planets that orbit other planets
	std::vector<std::string> getAllSatellitePlanetsNames() const;
	std::vector<Planet*> getAllSatellitePlanetsPointers() const;

	std::string getName() const { return name; };
	QJsonObject getJSONRepresentation() const;
	virtual ~OrbitalSystem();

  protected:
	Orbitable* rootOrbitable;
	double declinationTilt;

	void indexNewOrbitable(Orbitable* orbitable);

  private:
	std::string name;

	// indexing dictionaries
	std::map<std::string, Orbitable*> orbitables;
	std::map<std::string, Orbitable*> binaries;
	std::map<std::string, CelestialBody*> celestialBodies;
	std::map<std::string, Star*> stars;
	std::map<std::string, Planet*> planets;
	std::map<std::string, Planet*> firstClassPlanets;
	std::map<std::string, Planet*> satellitePlanets;
};

#endif // ORBITALSYSTEM_HPP
