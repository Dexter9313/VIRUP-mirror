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

#include "../../include/physics/OrbitalSystem.hpp"

OrbitalSystem::OrbitalSystem(std::string name)
    : name(std::move(name))
{
}

OrbitalSystem::OrbitalSystem(std::string name, QJsonObject const& json)
    : name(std::move(name))
{
	if(json.contains("binaries"))
	{
		if(json.contains("stars") || json.contains("planets"))
		{
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
			qWarning() << "WARNING: Ill-formed JSON : Several root orbitable "
			              "types declared. Taking binary as root...";
		}

		QJsonArray binArray(json["binaries"].toArray());
		if(binArray.size() > 1)
		{
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
			qWarning()
			    << "WARNING: Ill-formed JSON : Several binaries declared "
			       "as root orbitable. Taking first binary as root...";
		}
		else if(binArray.empty())
		{
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
			qWarning()
			    << "ERROR: Ill-formed JSON : Root \"binaries\" array is empty.";
			exit(EXIT_FAILURE);
		}
		QJsonObject jsonBin(binArray[0].toObject());
		rootOrbitable = new Orbitable(Orbitable::Type::BINARY, jsonBin, *this);
		rootOrbitable->parseChildren(jsonBin);
	}
	else if(json.contains("stars"))
	{
		if(json.contains("planets"))
		{
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
			qWarning() << "WARNING: Ill-formed JSON : Several root orbitable "
			              "types declared. Taking star as root...";
		}

		QJsonArray starArray(json["stars"].toArray());
		if(starArray.size() > 1)
		{
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
			qWarning() << "WARNING: Ill-formed JSON : Several stars declared "
			              "as root orbitable. Taking first star as root...";
		}
		else if(starArray.empty())
		{
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
			qWarning()
			    << "ERROR: Ill-formed JSON : Root \"stars\" array is empty.";
			exit(EXIT_FAILURE);
		}
		QJsonObject jsonStar(starArray[0].toObject());
		rootOrbitable = new Star(jsonStar, *this);
		rootOrbitable->parseChildren(jsonStar);
	}
	else if(json.contains("planets"))
	{
		QJsonArray planetArray(json["planets"].toArray());
		if(planetArray.size() > 1)
		{
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
			qWarning() << "WARNING: Ill-formed JSON : Several planets declared "
			              "as root orbitable. Taking first planet as root...";
		}
		else if(planetArray.empty())
		{
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
			qWarning()
			    << "ERROR: Ill-formed JSON : Root \"planets\" array is empty.";
			exit(EXIT_FAILURE);
		}
		QJsonObject jsonPlanet(planetArray[0].toObject());
		rootOrbitable = new Planet(jsonPlanet, *this);
		rootOrbitable->parseChildren(jsonPlanet);
	}
	else
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
		qWarning() << "ERROR: Ill-formed JSON : System is empty or doesn't "
		              "contain a root binary, star or planet (can't find keys "
		              "\"binaries\", \"stars\" or \"planets\" at root level).";
		exit(EXIT_FAILURE);
	}
	// generate names for orbitables that don't have any
	generateStarsNames();
	rootOrbitable->generateBinariesNames();
	rootOrbitable->generatePlanetsNames();
	indexNewOrbitable(rootOrbitable);
	declinationTilt = json["declinationTilt"].toDouble();

	for(auto orbitable : rootOrbitable->getAllDescendants())
	{
		indexNewOrbitable(orbitable);
	}
}

OrbitalSystem::OrbitalSystem(std::string name, Orbitable* rootOrbitable,
                             double declinationTilt)
    : rootOrbitable(rootOrbitable)
    , declinationTilt(declinationTilt)
    , name(std::move(name))
{
	indexNewOrbitable(rootOrbitable);
	for(auto orbitable : rootOrbitable->getAllDescendants())
	{
		indexNewOrbitable(orbitable);
	}
}

bool OrbitalSystem::isValid() const
{
	for(auto orbitable : getAllOrbitablesPointers())
	{
		if(!orbitable->isValid())
		{
			return false;
		}
	}
	return true;
}

void OrbitalSystem::generateStarsNames()
{
	if(rootOrbitable->getOrbitableType() == Orbitable::Type::STAR)
	{
		rootOrbitable->setName(name);
		return;
	}

	std::vector<Orbitable*> stars(
	    rootOrbitable->getAllDescendants(Orbitable::Type::STAR));

	// sort by mass
	std::map<double, Orbitable*> sorted;
	for(auto star : stars)
	{
		sorted[dynamic_cast<Star*>(star)->getCelestialBodyParameters().mass]
		    = star;
	}

	// set their name by decreasing mass
	char starLetter('A');
	for(auto iter = sorted.rbegin(); iter != sorted.rend(); ++iter)
	{
		iter->second->setName(name + " " + starLetter);
		++starLetter;
	}
}

std::pair<double, double>
    OrbitalSystem::getRADecFromCarthesian(Vector3 dir) const
{
	std::pair<double, double> result;

	dir.rotateAlongX(declinationTilt);

	result.first  = atan2(dir[1], dir[0]);
	result.second = asin(dir[2]);

	return result;
}

void OrbitalSystem::addChild(Orbitable* child, std::string const& parent)
{
	if(parent.empty())
	{
		rootOrbitable->addChild(child);
	}
	else
	{
		orbitables[parent]->addChild(child);
	}

	indexNewOrbitable(child);
}

Orbitable* OrbitalSystem::operator[](std::string const& name)
{
	return orbitables[name];
}

Orbitable const* OrbitalSystem::operator[](std::string const& name) const
{
	return orbitables.at(name);
}

std::vector<std::string> OrbitalSystem::getAllOrbitablesNames() const
{
	std::vector<std::string> result;

	for(auto pair : orbitables)
	{
		result.push_back(pair.first);
	}

	return result;
}

std::vector<Orbitable*> OrbitalSystem::getAllOrbitablesPointers() const
{
	std::vector<Orbitable*> result;

	for(auto pair : orbitables)
	{
		result.push_back(pair.second);
	}

	return result;
}

std::vector<std::string> OrbitalSystem::getAllBinariesNames() const
{
	std::vector<std::string> result;

	for(auto pair : binaries)
	{
		result.push_back(pair.first);
	}

	return result;
}

std::vector<Orbitable*> OrbitalSystem::getAllBinariesPointers() const
{
	std::vector<Orbitable*> result;

	for(auto pair : binaries)
	{
		result.push_back(pair.second);
	}

	return result;
}

std::vector<std::string> OrbitalSystem::getAllCelestialBodiesNames() const
{
	std::vector<std::string> result;

	for(auto pair : celestialBodies)
	{
		result.push_back(pair.first);
	}

	return result;
}

std::vector<CelestialBody*> OrbitalSystem::getAllCelestialBodiesPointers() const
{
	std::vector<CelestialBody*> result;

	for(auto pair : celestialBodies)
	{
		result.push_back(pair.second);
	}

	return result;
}

std::vector<std::string> OrbitalSystem::getAllStarsNames() const
{
	std::vector<std::string> result;

	for(auto pair : stars)
	{
		result.push_back(pair.first);
	}

	return result;
}

std::vector<Star*> OrbitalSystem::getAllStarsPointers() const
{
	std::vector<Star*> result;

	for(auto pair : stars)
	{
		result.push_back(pair.second);
	}

	return result;
}

std::vector<std::string> OrbitalSystem::getAllPlanetsNames() const
{
	std::vector<std::string> result;

	for(auto pair : planets)
	{
		result.push_back(pair.first);
	}

	return result;
}

std::vector<Planet*> OrbitalSystem::getAllPlanetsPointers() const
{
	std::vector<Planet*> result;

	for(auto pair : planets)
	{
		result.push_back(pair.second);
	}

	return result;
}

std::vector<std::string> OrbitalSystem::getAllFirstClassPlanetsNames() const
{
	std::vector<std::string> result;

	for(auto pair : firstClassPlanets)
	{
		result.push_back(pair.first);
	}

	return result;
}

std::vector<Planet*> OrbitalSystem::getAllFirstClassPlanetsPointers() const
{
	std::vector<Planet*> result;

	for(auto pair : firstClassPlanets)
	{
		result.push_back(pair.second);
	}

	return result;
}

std::vector<std::string> OrbitalSystem::getAllSatellitePlanetsNames() const
{
	std::vector<std::string> result;

	for(auto pair : satellitePlanets)
	{
		result.push_back(pair.first);
	}

	return result;
}

std::vector<Planet*> OrbitalSystem::getAllSatellitePlanetsPointers() const
{
	std::vector<Planet*> result;

	for(auto pair : satellitePlanets)
	{
		result.push_back(pair.second);
	}

	return result;
}

QJsonObject OrbitalSystem::getJSONRepresentation() const
{
	QJsonObject result;
	result["declinationTilt"] = declinationTilt;

	QJsonArray arr;
	arr.push_back(rootOrbitable->getJSONRepresentation());

	if(rootOrbitable->getOrbitableType() == Orbitable::Type::BINARY)
	{
		result["binaries"] = arr;
	}
	else if(rootOrbitable->getOrbitableType() == Orbitable::Type::STAR)
	{
		result["stars"] = arr;
	}
	else
	{
		result["planets"] = arr;
	}

	return result;
}

OrbitalSystem::~OrbitalSystem()
{
	delete rootOrbitable;
}

void OrbitalSystem::indexNewOrbitable(Orbitable* orbitable)
{
	orbitables[orbitable->getName()] = orbitable;
	if(orbitable->getOrbitableType() == Orbitable::Type::BINARY)
	{
		binaries[orbitable->getName()] = orbitable;
	}
	else
	{
		auto celestialBody(dynamic_cast<CelestialBody*>(orbitable));
		celestialBodies[orbitable->getName()] = celestialBody;
		if(celestialBody->getCelestialBodyType() == CelestialBody::Type::STAR)
		{
			auto star(dynamic_cast<Star*>(celestialBody));
			stars[star->getName()] = star;
		}
		else
		{
			auto planet(dynamic_cast<Planet*>(celestialBody));
			planets[planet->getName()] = planet;
			if(planet->getParent() == nullptr
			   || planet->getParent()->getOrbitableType()
			          != Orbitable::Type::PLANET)
			{
				firstClassPlanets[planet->getName()] = planet;
			}
			else
			{
				satellitePlanets[planet->getName()] = planet;
			}
		}
	}
}
