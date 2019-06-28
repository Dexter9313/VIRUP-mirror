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

#ifndef VECTOR4D_HPP
#define VECTOR4D_HPP

#include <array>
#include <cmath>
#include <iostream>

#include "Vector3.hpp"
#include "exceptions/Exceptions.hpp"

/*! \ingroup math
 * Represents a quadridimensional vector to be used for algebra and
 * representation
 * of a point/vertex in 3D space as [homogeneous
 * coordinates](https://en.wikipedia.org/wiki/Homogeneous_coordinates#Use_in_computer_graphics)
 * to simplify transforms. In a way, the fourth component authorizes or forbids
 * translation on the vector
 * when multiplied by a transformation matrix (position-vector vs
 * velocity-vector for example).
 *
 * See \ref math group description for conventions and notations.
 *
 * This class is used whenever possible instead of any library vector class to
 * ensure libraries can be changed easily if needed. Converters for different
 * library vector classes are provided in \ref Utils.hpp. Usage of another
 * library vector class should only be done for performance reasons (likely in
 * the \ref math module where algebraic algorithms are written) and should be
 * properly documented.
 */

class Vector4
{
  public:
	/*! Default constructor
	 *
	 * Constructs the null vector [0; 0; 0; 0].
	 */
	Vector4();

	/*! Default copy constructor
	 *
	 * Copies all values of components.
	 * \param copiedVector Vector4 to be copied
	 */
	Vector4(Vector4 const& copiedVector) = default;

	/*! Default move constructor
	 *
	 * Moves all values of components.
	 * \param movedVector Vector4 to be moved
	 */
	Vector4(Vector4&& movedVector) = default;

	/*! Constructor from coefficients
	 *
	 * Constructs the vector from an array of coefficients.
	 * \param data array of coefficients
	 */
	Vector4(std::array<double, 4> data);

	/*! Component-wise constructor
	 *
	 * Constructs the vector [x; y; z; w].
	 * \param x first component's value
	 * \param y second component's value
	 * \param z third component's value
	 * \param w fourth component's value
	 */
	Vector4(double x, double y, double z, double w);

	/*! Constructor from Vector3
	 *
	 * Constructs the vector [vec3[0]; vec3[1]; vec3[2]; w].
	 * \param vec3 Vector3 to construct from
	 * \param w fourth component's value, default is 0 (not a position-vector)
	 */
	Vector4(Vector3 const& vec3, double w = 0);

	/*! Sets all first three components of the vector to new values
	 *
	 * \param x first component's value
	 * \param y second component's value
	 * \param z third component's value
	 */
	void setXYZ(double x, double y, double z);

	/*! Sets all four components of the vector to new values
	 *
	 * \param x first component's value
	 * \param y second component's value
	 * \param z third component's value
	 * \param w fourth component's value
	 */
	void setXYZW(double x, double y, double z, double w);

	/*! Gets the length of the vector as a 3D vector (w is ignored)
	 *
	 * Classic euclidian L_2 norm is used.
	 */
	double length() const;

	/*! Adds another vector to this vector
	 *
	 * Element-wise vector sum is used for x, y and z components.
	 *
	 * w remains unchanged, unless vectorToAdd's w component is 1, then the w
	 * component of this will be set to 1. This ensures commutativity of the
	 * addition and makes the most sense. Indeed,
	 * "a position + a translation = a position" seems sensible.
	 * \param vectorToAdd Vector4 to be added.
	 */
	Vector4& operator+=(Vector4 const& vectorToAdd);

	/*! Subtracts another vector to this vector
	 *
	 * Equivalent of doing *this += -1*vectorToSubtract.
	 *
	 * See Vector4::operator+= and operator* for more details.
	 * \param vectorToSubtract Vector4 to be subtracted.
	 */
	Vector4& operator-=(Vector4 const& vectorToSubtract);

	/*! Multiplies this vector by a scalar
	 *
	 * Element-wise scalar product is used for x, y and z components.
	 * w remains unchanged. Concretely
	 * this vector [x; y; z; w] becomes [scalar*x; scalar*y; scalar*z; w]
	 * \param scalar to be multiply with.
	 */
	Vector4& operator*=(double scalar);

	/*! Divides this vector by a scalar
	 *
	 * Equivalent of doing *this *= (1.0 / scalar)
	 *
	 * See Vector4::operator*= for more details.
	 * \param scalar scalar to divide with.
	 */
	Vector4& operator/=(double scalar);

	/*! Default copy assignment operator
	 *
	 * Copies all values of components.
	 * \param copiedVector Vector4 to be copied
	 */
	Vector4& operator=(Vector4 const& copiedVector) = default;

	/*! Default move assignment operator
	 *
	 * Moves all values of components.
	 * \param movedVector Vector4 to be moved
	 */
	Vector4& operator=(Vector4&& movedVector) = default;

	/*! Returns a reference to a specific component of this vector
	 *
	 * Indexes correspondance :
	 * 0 <-> x
	 *
	 * 1 <-> y
	 *
	 * 2 <-> z
	 *
	 * 3 <-> w
	 *
	 * \param index index of the component
	 * \throws #CriticalException if and only if index > 2
	 *
	 * \param index index of the component
	 */
	double& operator[](unsigned int index);

	/*! Returns the value of a specific component of this vector
	 *
	 * Indexes correspondance :
	 * 0 <-> x
	 *
	 * 1 <-> y
	 *
	 * 2 <-> z
	 *
	 * 3 <-> w
	 *
	 * \param index index of the component
	 * \throws #CriticalException if and only if index > 2
	 */
	double operator[](unsigned int index) const;

	/*! Returns the unit form of this vector
	 *
	 * It is defined as *this / this->length().
	 * Notice that the w component of this vector is unchanged and that
	 * Vector4::length ignores it.
	 */
	Vector4 getUnitForm() const;

	/*! Rotates this vector along the first base axis of the coordinate system
	 *
	 * w is preserved.
	 * \param angle angle in radians
	 */
	Vector4& rotateAlongX(double angle);

	/*! Rotates this vector along the second base axis of the coordinate system
	 *
	 * w is preserved.
	 * \param angle angle in radians
	 */
	Vector4& rotateAlongY(double angle);

	/*! Rotates this vector along the third base axis of the coordinate system
	 *
	 * w is preserved.
	 * \param angle angle in radians
	 */
	Vector4& rotateAlongZ(double angle);

	/*! Length of this vector once projected on another vector
	 *
	 * base is normalized before the projection so that its length
	 * doesn't change the result.
	 *
	 * Notice that the projection length has then a meaning in
	 * absolute coordinates only (absolute coordinates in which these two
	 * vectors are expressed). Base doesn't define a relative scaled coordinate
	 * system.
	 * \param base vector which this vector is projected on
	 */
	double getProjectionLengthOn(Vector4 const& base) const;

	/*! Prints this vector in a std::ostream as a nice string
	 *
	 * \param stream stream to print within
	 */
	virtual std::ostream& printInStream(std::ostream& stream) const;

  private:
	std::array<double, 4> data;
};

/*! Addition of two vectors
 * \relates Vector4
 *
 * Uses Vector4::operator+= so that it has the same addition semantic.
 * \param a first addition parameter
 * \param b second addition parameter
 */
Vector4 operator+(Vector4 const& a, Vector4 const& b);

/*! Subtraction of two vectors
 * \relates Vector4
 *
 * Uses Vector4::operator-= so that it has the same subtraction semantic.
 * \param a first subtraction parameter
 * \param b second subtraction parameter
 */
Vector4 operator-(Vector4 const& a, Vector4 const& b);

/*! Dot product of two vectors
 * \relates Vector4
 *
 * Equivalent to converting both parameters to Vector3s then calling
 * dotProduct() on them as Vector3s (w component is ignored).
 * \param a first dot product parameter
 * \param b second dot product parameter
 */
double dotProduct(Vector4 const& a, Vector4 const& b);

/*! Cross product of two vectors
 * \relates Vector4
 *
 * Equivalent to converting both parameters to Vector3s then calling
 * crossProduct() on them as Vector3s. This results in a Vector3 on which
 * we append a w component defined as in the following :
 * the w component of the result is set to 1 if and only if both
 * w components of a and b are 1, and 0 instead.
 * This means the result is a position-vector if and only if both
 * parameters are position-vectors, which makes the most sense.
 * \param a first cross product parameter
 * \param b second cross product parameter
 */
Vector4 crossProduct(Vector4 const& a, Vector4 const& b);

/*! Product between a vector and a scalar
 * \relates Vector4
 *
 * Uses Vector4::operator*= so that it has the same product semantic.
 * \param vector vector to be multiplied
 * \param scalar scalar to be multiplied
 */
Vector4 operator*(Vector4 const& vector, double scalar);

/*! Product between a scalar and a vector
 * \relates Vector4
 *
 * Uses Vector4::operator*= so that it has the same product semantic.
 * \param scalar scalar to be multiplied
 * \param vector vector to be multiplied
 */
Vector4 operator*(double scalar, Vector4 const& vector);

/*! Division between a vector and a scalar
 * \relates Vector4
 *
 * Uses Vector4::operator/= so that it has the same product semantic.
 * \param vector vector to be divided
 * \param scalar scalar which divides
 */
Vector4 operator/(Vector4 const& vector, double scalar);

/*! Tests equality between two vectors
 * \relates Vector4
 *
 * Each components have to be equal, w included.
 * \param a first vector to compare to second
 * \param b second vector to compare to first
 */
bool operator==(Vector4 const& a, Vector4 const& b);

/*! Tests difference between two vectors
 * \relates Vector4
 *
 * Uses == so that it has the same equality semantic.
 * \param a first vector to compare to second
 * \param b second vector to compare to first
 */
bool operator!=(Vector4 const& a, Vector4 const& b);

/*! Printing in stream operator
 * \relates Vector4
 *
 * Uses Vector4::printInStream so that it uses the same string representation.
 * \param stream stream to print within
 * \param vector vector to be printed
 */
std::ostream& operator<<(std::ostream& stream, Vector4 const& vector);

#endif // VECTOR4D_HPP
