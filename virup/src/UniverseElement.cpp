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

#include "UniverseElement.hpp"

QMatrix4x4 UniverseElement::getRelToAbsTransform() const
{
	QMatrix4x4 relToAbsTransform;
	relToAbsTransform.scale(unit);
	relToAbsTransform.translate(-1.0 * Utils::toQt(solarsystemPosition));
	relToAbsTransform = transform(referenceFrame, ReferenceFrame::ECLIPTIC)
	                    * relToAbsTransform;

	return relToAbsTransform;
}

QMatrix4x4 const& UniverseElement::equatorialToEcliptic()
{
	static QMatrix4x4 equatorialToEcliptic
	    = QMatrix4x4(1.0, 6.19344636e-05, 2.6982713e-05, 0.0,      //
	                 -6.74978101e-05, 0.91747101, 0.39780267, 0.0, //
	                 -1.01181804e-07, -0.39780266, 0.917471, 0.0,  //
	                 0.0, 0.0, 0.0, 1.0);

	return equatorialToEcliptic;
}

QMatrix4x4 const& UniverseElement::galacticToEcliptic()
{
	static QMatrix4x4 galacticToEcliptic
	    = QMatrix4x4(-0.05494273, 0.49410207, -0.86766607, 0.0,  //
	                 -0.99382033, -0.11100021, -0.00027913, 0.0, //
	                 -0.09644906, 0.86228889, 0.49714731, 0.0,   //
	                 0.0, 0.0, 0.0, 1.0);

	return galacticToEcliptic;
}

QMatrix4x4 UniverseElement::transform(ReferenceFrame from, ReferenceFrame to)
{
	if(to == from)
	{
		return {};
	}
	if(to == ReferenceFrame::ECLIPTIC)
	{
		if(from == ReferenceFrame::EQUATORIAL)
		{
			return equatorialToEcliptic();
		}
		return galacticToEcliptic();
	}
	if(from == ReferenceFrame::ECLIPTIC)
	{
		return transform(to, from).inverted();
	}
	return transform(from, ReferenceFrame::ECLIPTIC)
	       * transform(ReferenceFrame::ECLIPTIC, to);
}

void UniverseElement::getModelAndCampos(Camera const& camera, QMatrix4x4& model,
                                        QVector3D& campos)
{
	auto relToAbsTransform(getRelToAbsTransform());
	model = camera.dataToWorldTransform() * relToAbsTransform;

	campos
	    = relToAbsTransform.inverted() * Utils::toQt(camera.getTruePosition());
}
