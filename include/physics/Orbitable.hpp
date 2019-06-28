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

#ifndef ORBITABLE_HPP
#define ORBITABLE_HPP

#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QProgressDialog>

#include "CSVOrbit.hpp"

class OrbitalSystem;

/*! \ingroup phys
 * Represents any virtual or real object that has an orbit and/or can be
 * orbited, such as barycenters (or binary barycenters), stars or planets for
 * example.
 *
 * If parent == nullptr, i.e. if
 */
class Orbitable
{
  public:
	enum class Type
	{
		BINARY,
		STAR,
		PLANET,
	};

	Orbitable(Type type, QJsonObject const& json, OrbitalSystem const& system);
	Orbitable(Type type, QJsonObject const& json, Orbitable const& parent);
	// /!\ will take ownership of orbit ; make sure it doesn't go out of scope
	// later !
	Orbitable(Type type, std::string name, OrbitalSystem const& system,
	          Orbit* orbit = nullptr);
	// /!\ will take ownership of orbit ; make sure it doesn't go out of scope
	// later !
	Orbitable(Type type, std::string name, Orbitable const& parent,
	          Orbit* orbit = nullptr);
	bool isValid() const
	{
		if(orbit != nullptr && !orbit->isValid())
		{
			return false;
		}
		return true;
	}

	void generateBinariesNames();
	void generatePlanetsNames();

	// we have to separate children parsing because parsing directly in
	// constructor means children can't dynamic_cast us if they need to
	void parseChildren(QJsonObject const& json);

	OrbitalSystem const& getSystem() const { return system; };
	Type getOrbitableType() const { return type; };
	std::string const& getName() const { return name; };
	void setName(std::string const& name)
	{
		if(this->name.empty())
		{
			this->name = name;
		}
	};
	float getPseudoRandomSeed() const;
	Orbitable const* getParent() const { return parent; };
	Orbit const* getOrbit() const { return orbit; };
	Orbit* getOrbit() { return orbit; };

	std::vector<Orbitable*> const& getChildren() const { return children; };
	std::vector<Orbitable*> getAllDescendants() const;
	// filter by some type
	std::vector<Orbitable*> getAllDescendants(Type type) const;

	// will take ownership of child
	void addChild(Orbitable* child);

	Vector3 getRelativePositionAtUT(UniversalTime uT) const;
	Vector3 getAbsolutePositionAtUT(UniversalTime uT) const;
	// Vector3 getAbsoluteVelocityAtUT(UniversalTime uT) const;
	// CoordinateSystem getAttachedCoordinateSystemAtUT(UniversalTime uT) const;

	virtual QJsonObject getJSONRepresentation() const;
	virtual ~Orbitable();

	// Will try to get more significant digits than the awful
	// to.absolute - from.absolute
	static Vector3 getRelativePositionAtUt(Orbitable const* from,
	                                       Orbitable const* to,
	                                       UniversalTime uT);

  private:
	OrbitalSystem const& system;
	Type type;
	std::string name;
	Orbitable const* parent = nullptr;
	Orbit* orbit            = nullptr;
	std::vector<Orbitable*> children;

	/** LOADING BAR **/

	static QProgressDialog* progress;
	static float value;
	static float current;
};

#endif // ORBITABLE_HPP
