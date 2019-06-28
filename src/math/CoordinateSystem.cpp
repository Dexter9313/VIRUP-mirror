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

#include "../../include/math/CoordinateSystem.hpp"

CoordinateSystem::CoordinateSystem(Vector3 const& x0, Vector3 const& x1,
                                   Vector3 const& x2, Vector3 const& origin)
{
	for(unsigned int i(0); i < 3; ++i)
	{
		transform[i][0] = x0[i];
		transform[i][1] = x1[i];
		transform[i][2] = x2[i];
		transform[i][3] = origin[i];
	}
}

void CoordinateSystem::setBase(Vector3 const& x0, Vector3 const& x1,
                               Vector3 const& x2)
{
	for(unsigned int i(0); i < 3; ++i)
	{
		transform[i][0] = x0[i];
		transform[i][1] = x1[i];
		transform[i][2] = x2[i];
	}
}

void CoordinateSystem::setBase(Vector3 const& x0, Vector3 const& x1,
                               Vector3 const& x2, Vector3 const& origin)
{
	for(unsigned int i(0); i < 3; ++i)
	{
		transform[i][0] = x0[i];
		transform[i][1] = x1[i];
		transform[i][2] = x2[i];
		transform[i][3] = origin[i];
	}
}

Vector3 CoordinateSystem::operator[](unsigned int index) const
{
	if(index > 2)
		CRITICAL("Index out of bounds.")
	return Vector3(transform[0][index], transform[1][index],
	               transform[2][index]);
}

Vector3 CoordinateSystem::getOrigin() const
{
	return Vector3(transform[0][3], transform[1][3], transform[2][3]);
}

void CoordinateSystem::setOrigin(Vector3 const& origin)
{
	transform[0][3] = origin[0];
	transform[1][3] = origin[1];
	transform[2][3] = origin[2];
}

CoordinateSystem& CoordinateSystem::rotateAlongX(double angle)
{
	setBase((*this)[0].rotateAlongX(angle), (*this)[1].rotateAlongX(angle),
	        (*this)[2].rotateAlongX(angle));

	return *this;
}

CoordinateSystem& CoordinateSystem::rotateAlongY(double angle)
{
	setBase((*this)[0].rotateAlongY(angle), (*this)[1].rotateAlongY(angle),
	        (*this)[2].rotateAlongY(angle));

	return *this;
}

CoordinateSystem& CoordinateSystem::rotateAlongZ(double angle)
{
	setBase((*this)[0].rotateAlongZ(angle), (*this)[1].rotateAlongZ(angle),
	        (*this)[2].rotateAlongZ(angle));

	return *this;
}

CoordinateSystem& CoordinateSystem::rotate(double angle, Vector3 const& axis)
{
	Matrix4x4 rotationMat(angle, axis);
	setBase(rotationMat * (*this)[0], rotationMat * (*this)[1],
	        rotationMat * (*this)[2]);

	return *this;
}

Vector4 getAbsoluteVectorFromRelative(CoordinateSystem const& coordSys,
                                      Vector4 const& relativeVector)
{
	return coordSys.getTransform() * relativeVector;
}

Vector4 getRelativeVectorFromAbsolute(CoordinateSystem const& coordSys,
                                      Vector4 const& absoluteVector)
{
	// if optimization needed, maybe inverse coordSys transform
	Vector4 translatedVec(absoluteVector[3] == 1
	                          ? absoluteVector - coordSys.getOrigin()
	                          : absoluteVector);
	return Vector4(translatedVec.getProjectionLengthOn(coordSys[0]),
	               translatedVec.getProjectionLengthOn(coordSys[1]),
	               translatedVec.getProjectionLengthOn(coordSys[2]),
	               absoluteVector[3]);
}

Vector3 getAbsoluteVectorFromRelative(CoordinateSystem const& coordSys,
                                      Vector3 const& relativeVector)
{
	return coordSys.getTransform() * relativeVector;
}

Vector3 getRelativeVectorFromAbsolute(CoordinateSystem const& coordSys,
                                      Vector3 const& absoluteVector)
{
	// if optimization needed, maybe inverse coordSys transform
	return Vector3(absoluteVector.getProjectionLengthOn(coordSys[0]),
	               absoluteVector.getProjectionLengthOn(coordSys[1]),
	               absoluteVector.getProjectionLengthOn(coordSys[2]));
}

Vector3 getAbsolutePositionFromRelative(CoordinateSystem const& coordSys,
                                        Vector3 const& relativePosition)
{
	Vector4 result(coordSys.getTransform() * Vector4(relativePosition, 1));
	return Vector3(result[0], result[1], result[2]);
}

Vector3 getRelativePositionFromAbsolute(CoordinateSystem const& coordSys,
                                        Vector3 const& absolutePosition)
{
	// if optimization needed, maybe inverse coordSys transform
	Vector3 translatedPos(absolutePosition - coordSys.getOrigin());
	return Vector3(translatedPos.getProjectionLengthOn(coordSys[0]),
	               translatedPos.getProjectionLengthOn(coordSys[1]),
	               translatedPos.getProjectionLengthOn(coordSys[2]));
}
