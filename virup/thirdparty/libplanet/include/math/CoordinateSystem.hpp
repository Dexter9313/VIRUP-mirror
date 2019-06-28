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

#ifndef COORDINATESYSTEM_HPP
#define COORDINATESYSTEM_HPP

#include <array>

#include "Matrix4x4.hpp"
#include "Vector4.hpp"
#include "exceptions/Exceptions.hpp"

/*! \ingroup math
 * Vector-space basis way to represent a combination of rotations and
 * translations on vectors and positions.
 *
 * See \ref math group description for conventions.
 *
 * Vector-space used is R^3. This representation is a particular orthonormal
 * basis for R^3 that is rotated and translated. Algebra
 * traditionally only considers vectors but we will consider positions as well
 * through [homogeneous
 * coordinates](https://en.wikipedia.org/wiki/Homogeneous_coordinates#Use_in_computer_graphics),
 * which are vectors with a starting point. Thus,
 * our coordinate system has an origin.
 *
 * We call "absolute system" the coordinate system in which the basis vectors of
 * this particular coordinate system are expressed.
 */
class CoordinateSystem
{
  public:
	/*! Default constructor
	 *
	 * Constructs a coordinate system with no rotation and no translation.
	 * Basis vector are [1, 0, 0], [0, 1, 0] and [0, 0, 1] and origin is [0, 0,
	 * 0].
	 */
	CoordinateSystem() = default;

	/*! Default copy constructor
	 *
	 * Copies all of the three basis vectors and the origin.
	 * \param copiedSystem CoordinateSystem to be copied
	 */
	CoordinateSystem(CoordinateSystem const& copiedSystem) = default;

	/*! Default move constructor
	 *
	 * Moves all of the three basis vectors and the origin.
	 * \param movedSystem CoordinateSystem to be moved
	 */
	CoordinateSystem(CoordinateSystem&& movedSystem) = default;

	/*! Constructor from basis vectors and origin
	 *
	 * Constructs the coordinate system with basis vectors x0, x1, x2 and an
	 * origin.
	 * \param x0 first basis vector
	 * \param x1 second basis vector
	 * \param x2 third basis vector
	 * \param origin origin, by default [0, 0, 0]
	 */
	CoordinateSystem(Vector3 const& x0, Vector3 const& x1, Vector3 const& x2,
	                 Vector3 const& origin = Vector3());

	/*! Gets the transformation matrix associated to the coordinate system.
	 *
	 * If a matrix mat is returned, then for any #Vector4 v expressed in this
	 * coordinate system, then mat*v is v expressed in the absolute system.
	 */
	Matrix4x4 getTransform() const { return transform; };

	/*! Changes basis vectors only
	 *
	 * Reconstructs the coordinate system with basis vectors x0, x1 and x2. The
	 * origin remains unchanged.
	 * \param x0 first basis vector
	 * \param x1 second basis vector
	 * \param x2 third basis vector
	 */
	void setBase(Vector3 const& x0, Vector3 const& x1, Vector3 const& x2);

	/*! Changes basis vectors and origin
	 *
	 * Reconstructs the coordinate system with basis vectors x, y, z and an
	 * origin.
	 * \param x0 first basis vector
	 * \param x1 second basis vector
	 * \param x2 third basis vector
	 * \param origin origin, by default [0, 0, 0]
	 */
	void setBase(Vector3 const& x0, Vector3 const& x1, Vector3 const& x2,
	             Vector3 const& origin);

	/*! Default copy assignment operator
	 *
	 * Copies all of the three basis vectors and the origin.
	 * \param copiedSystem CoordinateSystem to be copied
	 */
	CoordinateSystem& operator=(CoordinateSystem const& copiedSystem) = default;

	/*! Default move assignment operator
	 *
	 * Moves all of the three basis vectors and the origin.
	 * \param movedSystem CoordinateSystem to be moved
	 */
	CoordinateSystem& operator=(CoordinateSystem&& movedSystem) = default;

	/*! Returns basis vector x{index}
	 *
	 * \param index index of the basis vector
	 *
	 * \throws #CriticalException if and only if index > 2
	 */
	Vector3 operator[](unsigned int index) const;

	/*! Returns the origin's position
	 */
	Vector3 getOrigin() const;

	/*! Set the origin's position
	 *
	 * \param origin vector to set as origin
	 */
	void setOrigin(Vector3 const& origin);

	/*! Rotates the coordinate system around the absolute X = [1; 0; 0] vector
	 *
	 * Rotation occur with constant origin (around an axis parallel to X and
	 * passing through origin). \param angle angle of the rotation
	 */
	CoordinateSystem& rotateAlongX(double angle);

	/*! Rotates the coordinate system around the absolute Y = [0; 1; 0] vector
	 *
	 * Rotation occur with constant origin (around an axis parallel to Y and
	 * passing through origin). \param angle angle of the rotation
	 */
	CoordinateSystem& rotateAlongY(double angle);

	/*! Rotates the coordinate system around the absolute Z = [0; 0; 1] vector
	 *
	 * Rotation occur with constant origin (around an axis parallel to Z and
	 * passing through origin). \param angle angle of the rotation
	 */
	CoordinateSystem& rotateAlongZ(double angle);

	/*! Rotates the coordinate system around the an axis
	 *
	 * Rotation occur with constant origin (around an axis parallel to the axis
	 * given and passing through origin). \param angle angle of the rotation
	 * \param axis axis to rotate around
	 */
	CoordinateSystem& rotate(double angle, Vector3 const& axis);

  private:
	Matrix4x4 transform;
};

/*! Transforms a Vector4 from relative coordinates to absolute coordinates
 * \relates CoordinateSystem
 * \relates Vector4
 *
 * \param coordSys coordinate system to consider
 * \param relativeVector vector to transform
 */
Vector4 getAbsoluteVectorFromRelative(CoordinateSystem const& coordSys,
                                      Vector4 const& relativeVector);

/*! Transforms a Vector4 from absolute coordinates to relative coordinates
 * \relates CoordinateSystem
 * \relates Vector4
 *
 * \param coordSys coordinate system to consider
 * \param absoluteVector vector to transform
 */
Vector4 getRelativeVectorFromAbsolute(CoordinateSystem const& coordSys,
                                      Vector4 const& absoluteVector);

/*! Transforms a vector from relative coordinates to absolute coordinates
 * \relates CoordinateSystem
 * \relates Vector3
 *
 * \param coordSys coordinate system to consider
 * \param relativeVector vector to transform
 */
Vector3 getAbsoluteVectorFromRelative(CoordinateSystem const& coordSys,
                                      Vector3 const& relativeVector);

/*! Transforms a position from relative coordinates to absolute coordinates
 * \relates CoordinateSystem
 * \relates Vector3
 *
 * \param coordSys coordinate system to consider
 * \param relativePosition position to transform
 */
Vector3 getAbsolutePositionFromRelative(CoordinateSystem const& coordSys,
                                        Vector3 const& relativePosition);

/*! Transforms a vector from absolute coordinates to relative coordinates
 * \relates CoordinateSystem
 * \relates Vector3
 *
 * \param coordSys coordinate system to consider
 * \param absoluteVector vector to transform
 */
Vector3 getRelativeVectorFromAbsolute(CoordinateSystem const& coordSys,
                                      Vector3 const& absoluteVector);

/*! Transforms a position from absolute coordinates to relative coordinates
 * \relates CoordinateSystem
 * \relates Vector3
 *
 * \param coordSys coordinate system to consider
 * \param absolutePosition position to transform
 */
Vector3 getRelativePositionFromAbsolute(CoordinateSystem const& coordSys,
                                        Vector3 const& absolutePosition);

#endif // COORDINATESYSTEM_HPP
