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

#include "physics/Orbitable.hpp"

#include "physics/OrbitalSystem.hpp"
#include "physics/Planet.hpp"
#include "physics/Star.hpp"

QProgressDialog* Orbitable::progress = nullptr;
float Orbitable::value               = 100.f;
float Orbitable::current             = 0.f;

Orbitable::Orbitable(Type type, QJsonObject const& json,
                     OrbitalSystem const& system)
    : system(system)
    , type(type)
    , parent(nullptr)
{
	name = json["name"].toString().toStdString();

	Orbit::MassiveBodyMass parentMass(0.0);
	if(CSVOrbit::csvExistsFor(name))
	{
		orbit = new CSVOrbit(parentMass, name);
	}
	else if(json.contains("orbit"))
	{
		orbit = new Orbit(parentMass, json["orbit"].toObject());
	}
}

Orbitable::Orbitable(Type type, QJsonObject const& json,
                     Orbitable const& parent)
    : system(parent.getSystem())
    , type(type)
    , parent(&parent)
{
	name = json["name"].toString().toStdString();

	Orbit::MassiveBodyMass parentMass(0.0);
	if(parent.type != Type::BINARY)
	{
		parentMass.value = dynamic_cast<CelestialBody const&>(parent)
		                       .getCelestialBodyParameters()
		                       .mass;
	}

	if(CSVOrbit::csvExistsFor(name))
	{
		orbit = new CSVOrbit(parentMass, name);
	}
	else if(json.contains("orbit"))
	{
		orbit = new Orbit(parentMass, json["orbit"].toObject());
	}
}

Orbitable::Orbitable(Type type, std::string name, OrbitalSystem const& system,
                     Orbit* orbit)
    : system(system)
    , type(type)
    , name(std::move(name))
    , parent(nullptr)
    , orbit(orbit)
{
}

Orbitable::Orbitable(Type type, std::string name, Orbitable const& parent,
                     Orbit* orbit)
    : system(parent.getSystem())
    , type(type)
    , name(std::move(name))
    , parent(&parent)
    , orbit(orbit)
{
}

void Orbitable::generateBinariesNames()
{
	if(type != Type::BINARY)
	{
		return;
	}

	std::vector<Orbitable*> nonPlanetsChildren(children);
	for(auto it(nonPlanetsChildren.begin()); it != nonPlanetsChildren.end();)
	{
		if((*it)->type == Type::PLANET)
		{
			nonPlanetsChildren.erase(it);
		}
		else
		{
			++it;
		}
	}

	if(nonPlanetsChildren.size() != 2)
	{
		std::cerr << "Binary isn't binary..." << std::endl;
		exit(EXIT_FAILURE);
	}

	// generate childrens names first
	for(auto o : nonPlanetsChildren)
	{
		o->generateBinariesNames();
	}

	std::string name0(nonPlanetsChildren[0]->getName()),
	    name1(nonPlanetsChildren[1]->getName());

	// find first point of divergence
	unsigned int i(0);
	while(name0[i] == name1[i])
	{
		++i;
	}

	if(name0[i] < name1[i])
	{
		setName(name0.substr(0, i) + name0.substr(i, std::string::npos)
		        + name1.substr(i, std::string::npos));
	}
	else
	{
		setName(name0.substr(0, i) + name1.substr(i, std::string::npos)
		        + name0.substr(i, std::string::npos));
	}
}

void Orbitable::generatePlanetsNames()
{
	// if no parent could set our own name
	if(type == Type::PLANET && parent == nullptr)
	{
		setName(getSystem().getName());
	}

	std::vector<Planet*> planetChildren;
	for(auto o : children)
	{
		if(o->type == Type::PLANET)
		{
			planetChildren.push_back(dynamic_cast<Planet*>(o));
		}
	}

	// sort by SMA
	std::map<double, Planet*> sorted;
	for(auto planet : planetChildren)
	{
		sorted[planet->getOrbit()->getParameters().semiMajorAxis] = planet;
	}
	// set their name by increasing SMA
	char planetLetter(type == Type::PLANET ? 1 : 'b');

	for(auto& pair : sorted)
	{
		pair.second->setName(name + " "
		                     + (type == Type::PLANET
		                            ? std::to_string(planetLetter)
		                            : std::string() + planetLetter));
		++planetLetter;
	}

	for(auto c : children)
	{
		c->generatePlanetsNames();
	}
}

void Orbitable::parseChildren(QJsonObject const& json)
{
	bool createdProgress(false);
	if(progress == nullptr)
	{
		createdProgress = true;
		progress        = new QProgressDialog(
            QObject::tr("Loading ") + getSystem().getName().c_str() + "...",
            QString(), 0, 100);
		value   = 100.f;
		current = 0.f;
	}

	if(!json["planets"].toArray().empty())
	{
		value /= json["planets"].toArray().size();
		for(auto val : json["planets"].toArray())
		{
			auto p = new Planet(val.toObject(), *this);
			p->parseChildren(val.toObject());
			children.push_back(p);
		}
		value *= json["planets"].toArray().size();
	}
	else
	{
		current += value;
		QCoreApplication::processEvents();
		progress->setValue(current);
	}

	if(createdProgress)
	{
		delete progress;
		progress = nullptr;
	}

	// binaries are the only orbitables to have non-planet children
	if(type != Type::BINARY)
	{
		return;
	}

	for(auto val : json["binaries"].toArray())
	{
		// binaries don't have their own class because they are pure orbitables
		auto b = new Orbitable(Type::BINARY, val.toObject(), *this);
		b->parseChildren(val.toObject());
		children.push_back(b);
	}

	for(auto val : json["stars"].toArray())
	{
		auto s = new Star(val.toObject(), *this);
		s->parseChildren(val.toObject());
		children.push_back(s);
	}
}

float Orbitable::getPseudoRandomSeed() const
{
	QString hash(
	    QCryptographicHash::hash(name.data(), QCryptographicHash::Md5).toHex());

	float seed(0.f);
	for(auto c : hash)
	{
		seed += c.toLatin1();
	}
	return seed;
}

std::vector<Orbitable*> Orbitable::getAllDescendants() const
{
	std::vector<Orbitable*> result(children);
	for(auto child : children)
	{
		std::vector<Orbitable*> childDescendants(child->getAllDescendants());
		result.insert(result.end(), childDescendants.begin(),
		              childDescendants.end());
	}
	return result;
}

// TODO(florian) : optimize (no need to seek stars as planet children for ex)
std::vector<Orbitable*> Orbitable::getAllDescendants(Type type) const
{
	std::vector<Orbitable*> result;
	for(auto child : children)
	{
		if(child->type == type)
		{
			result.push_back(child);
		}
		std::vector<Orbitable*> childDescendants(
		    child->getAllDescendants(type));
		result.insert(result.end(), childDescendants.begin(),
		              childDescendants.end());
	}
	return result;
}

void Orbitable::addChild(Orbitable* child)
{
	children.push_back(child);
}

Vector3 Orbitable::getRelativePositionAtUT(UniversalTime uT) const
{
	if(orbit == nullptr)
	{
		return Vector3(0.0, 0.0, 0.0);
	}

	return orbit->getPositionAtUT(uT);
}

Vector3 Orbitable::getAbsolutePositionAtUT(UniversalTime uT) const
{
	Vector3 result(getRelativePositionAtUT(uT));

	if(parent != nullptr)
	{
		// At least works for JPL Horizon Data
		result += parent->getAbsolutePositionAtUT(uT);
		/*result = getAbsolutePositionFromRelative(
		    parent->getAttachedCoordinateSystemAtUT(uT), result);*/
	}

	return result;
}

/*
CoordinateSystem
    Orbitable::getAttachedCoordinateSystemAtUT(UniversalTime uT) const
{
    return orbit->getRelativeCoordinateSystemAtUT(uT);
}*/

QJsonObject Orbitable::getJSONRepresentation() const
{
	QJsonObject result;

	if(orbit != nullptr && !orbit->isLoadedFromFile())
	{
		result["orbit"] = orbit->getJSONRepresentation();
	}

	result["name"] = name.c_str();

	if(!children.empty())
	{
		QJsonArray childrenJSONplanets;
		for(auto child : children)
		{
			if(child->type == Type::PLANET)
			{
				childrenJSONplanets.push_back(child->getJSONRepresentation());
			}
		}
		result["planets"] = childrenJSONplanets;

		QJsonArray childrenJSONstars;
		for(auto child : children)
		{
			if(child->type == Type::PLANET)
			{
				childrenJSONstars.push_back(child->getJSONRepresentation());
			}
		}
		result["stars"] = childrenJSONstars;

		QJsonArray childrenJSONbinaries;
		for(auto child : children)
		{
			if(child->type == Type::PLANET)
			{
				childrenJSONbinaries.push_back(child->getJSONRepresentation());
			}
		}
		result["binaries"] = childrenJSONbinaries;
	}

	return result;
}

Orbitable::~Orbitable()
{
	if(orbit != nullptr)
	{
		delete orbit;
	}
	for(Orbitable* child : children)
	{
		delete child;
	}
}

Vector3 Orbitable::getRelativePositionAtUt(Orbitable const* from,
                                           Orbitable const* to,
                                           UniversalTime uT)
{
	if(from == to)
	{
		return Vector3(0.0, 0.0, 0.0);
	}

	// Chain of relative positions from root orbitable to from.
	// The sum of the stored Vector3 should be absolute position
	// of from.
	std::vector<std::pair<Orbitable const*, Vector3>>
	    fromAncestorsRelativePositions;
	// Chain of relative positions from root orbitable to to.
	// The sum of the stored Vector3 should be absolute position
	// of to.
	std::vector<std::pair<Orbitable const*, Vector3>>
	    toAncestorsRelativePositions;

	// construct fromAncestorsRelativePositions
	fromAncestorsRelativePositions.push_back(
	    {from, from->getRelativePositionAtUT(uT)});
	Orbitable const* currentOrbitable(from);
	Orbitable const* parent(currentOrbitable->getParent());
	while(parent != nullptr)
	{
		fromAncestorsRelativePositions.push_back(
		    {parent, parent->getRelativePositionAtUT(uT)});
		currentOrbitable = parent;
		parent           = currentOrbitable->getParent();
	}

	// construct toAncestorsRelativePositions
	toAncestorsRelativePositions.push_back(
	    {to, to->getRelativePositionAtUT(uT)});
	currentOrbitable = to;
	parent           = currentOrbitable->getParent();
	while(parent != nullptr)
	{
		toAncestorsRelativePositions.push_back(
		    {parent, parent->getRelativePositionAtUT(uT)});
		currentOrbitable = parent;
		parent           = currentOrbitable->getParent();
	}

	// go up the tree from lowest node to start from both sides at the same
	// depth
	unsigned int i(0), j(0);
	if(fromAncestorsRelativePositions.size()
	   > toAncestorsRelativePositions.size())
	{
		i = fromAncestorsRelativePositions.size()
		    - toAncestorsRelativePositions.size();
	}
	else
	{
		j = toAncestorsRelativePositions.size()
		    - fromAncestorsRelativePositions.size();
	}

	Orbitable const* commonAncestor(nullptr);
	for(; i < fromAncestorsRelativePositions.size(); ++i, ++j)
	{
		if(fromAncestorsRelativePositions[i].first
		   == toAncestorsRelativePositions[j].first)
		{
			commonAncestor = fromAncestorsRelativePositions[i].first;
		}
	}

	// now we are in the reference frame of the common ancestor
	// "absolute" is in this reference frame
	Vector3 fromAbsolute(0.0, 0.0, 0.0), toAbsolute(0.0, 0.0, 0.0);
	// gain some computation but the algorithm would world without this check
	if(commonAncestor == nullptr)
	{
		fromAbsolute = from->getAbsolutePositionAtUT(uT);
		toAbsolute   = to->getAbsolutePositionAtUT(uT);
	}
	else
	{
		for(unsigned int k(0);
		    fromAncestorsRelativePositions[k].first != commonAncestor
		    // add if commonAncestor can be nullptr
		    // && i < fromAncestorsRelativePositions.size()
		    ;
		    ++k)
		{
			fromAbsolute += fromAncestorsRelativePositions[k].second;
		}

		for(unsigned int k(0);
		    toAncestorsRelativePositions[k].first != commonAncestor
		    // add if commonAncestor can be nullptr
		    // && i < toAncestorsRelativePositions.size()
		    ;
		    ++k)
		{
			toAbsolute += toAncestorsRelativePositions[k].second;
		}
	}

	return toAbsolute - fromAbsolute;
}
