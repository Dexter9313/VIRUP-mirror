/*
    Copyright (C) 2020 Florian Cabot <florian.cabot@hotmail.fr>

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

#ifndef UNIVERSEELEMENT_HPP
#define UNIVERSEELEMENT_HPP

#include "Camera.hpp"
#include "ToneMappingModel.hpp"
#include "math/Vector3.hpp"

class UniverseElement
{
  public:
	enum class ReferenceFrame
	{
		EQUATORIAL,
		ECLIPTIC,
		GALACTIC,
	};

	UniverseElement() = default;
	virtual void render(Camera const& camera, ToneMappingModel const* tmm) = 0;
	virtual ~UniverseElement();

	float brightnessMultiplier = 1.f;

	double unit                   = 1.0;                    // in kpc
	Vector3 solarsystemPosition   = Vector3(0.0, 0.0, 0.0); // in unit
	ReferenceFrame referenceFrame = ReferenceFrame::EQUATORIAL;

	static QMatrix4x4 const& equatorialToEcliptic();
	static QMatrix4x4 const& galacticToEcliptic();
	static QMatrix4x4 transform(ReferenceFrame from, ReferenceFrame to);

  protected:
	void getModelAndCampos(Camera const& camera, QMatrix4x4& model,
	                       QVector3D& campos);
};

#endif // UNIVERSEELEMENT_HPP
