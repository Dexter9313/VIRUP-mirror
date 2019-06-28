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

#ifndef VECTOR3D_HPP
#define VECTOR3D_HPP

#include <QJsonObject>
#include <array>
#include <cmath>
#include <iostream>

#include "exceptions/Exceptions.hpp"

/*! \ingroup math
 * Represents a tridimensional vector to be used for algebra and representation
 * of a point/vertex in 3D space.
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
class Vector3
{
  public:
	/*! Default constructor
	 *
	 * Constructs the null vector [0; 0; 0].
	 */
	Vector3();

	/*! Default copy constructor
	 *
	 * Copies all values of components.
	 * \param copiedVector Vector3 to be copied
	 */
	Vector3(Vector3 const& copiedVector) = default;

	/*! Default move constructor
	 *
	 * Moves all values of components.
	 * \param movedVector Vector3 to be moved
	 */
	Vector3(Vector3&& movedVector) = default;

	/*! Constructor from coefficients
	 *
	 * Constructs the vector from an array of coefficients.
	 * \param data array of coefficients
	 */
	Vector3(std::array<double, 3> data);

	/*! Component-wise constructor
	 *
	 * Constructs the vector [x; y; z].
	 * \param x first component's value
	 * \param y second component's value
	 * \param z third component's value
	 */
	Vector3(double x, double y, double z);

	Vector3(QJsonObject const& json, Vector3 const& defaultValue = Vector3());

	/*! Sets all first three components of the vector to new values
	 *
	 * \param x first component's value
	 * \param y second component's value
	 * \param z third component's value
	 */
	void setXYZ(double x, double y, double z);

	/*! Gets the length of the vector
	 *
	 * Classic euclidian L_2 norm is used.
	 */
	double length() const;

	/*! Adds another vector to this vector
	 *
	 * Classic element-wise vector sum is used.
	 * \param vectorToAdd Vector3 to be added.
	 */
	Vector3& operator+=(Vector3 const& vectorToAdd);

	/*! Subtracts another vector to this vector
	 *
	 * Classic element-wise vector subtraction is used.
	 * \param vectorToSubtract Vector3 to be subtracted.
	 */
	Vector3& operator-=(Vector3 const& vectorToSubtract);

	/*! Multiplies this vector by a scalar
	 *
	 * Classic scalar product is used :
	 * this vector [x; y; z] becomes [scalar*x; scalar*y; scalar*z]
	 * \param scalar to be multiply with.
	 */
	Vector3& operator*=(double scalar);

	/*! Divides this vector by a scalar
	 *
	 * Classic scalar division is used :
	 * this vector [x; y; z] becomes [x/scalar; y/scalar; z/scalar]
	 * \param scalar scalar to divide with.
	 */
	Vector3& operator/=(double scalar);

	/*! Default copy assignment operator
	 *
	 * Copies all values of components.
	 * \param copiedVector Vector3 to be copied
	 */
	Vector3& operator=(Vector3 const& copiedVector) = default;

	/*! Default move assignment operator
	 *
	 * Moves all values of components.
	 * \param movedVector Vector3 to be moved
	 */
	Vector3& operator=(Vector3&& movedVector) = default;

	/*! Returns a reference to a specific component of this vector
	 *
	 * Indexes correspondance :
	 *
	 * 0 <-> x
	 *
	 * 1 <-> y
	 *
	 * 2 <-> z
	 *
	 * \param index index of the component
	 * \throws #CriticalException if and only if index > 2
	 */
	double& operator[](unsigned int index);

	/*! Returns the value of a specific component of this vector
	 *
	 * Indexes correspondance :
	 *
	 * 0 <-> x
	 *
	 * 1 <-> y
	 *
	 * 2 <-> z
	 *
	 * \param index index of the component
	 * \throws #CriticalException if and only if index > 2
	 */
	double operator[](unsigned int index) const;

	/*! Returns the unit form of this vector
	 *
	 * It is defined as *this / this->length().
	 */
	Vector3 getUnitForm() const;

	/*! Rotates this vector along the first base axis of the coordinate system
	 *
	 * \param angle angle in radians
	 */
	Vector3& rotateAlongX(double angle);

	/*! Rotates this vector along the second base axis of the coordinate system
	 *
	 * \param angle angle in radians
	 */
	Vector3& rotateAlongY(double angle);

	/*! Rotates this vector along the third base axis of the coordinate system
	 *
	 * \param angle angle in radians
	 */
	Vector3& rotateAlongZ(double angle);

	/*! Length of this vector once projected on another vector
	 *
	 * \param base vector which this vector is projected on
	 */
	double getProjectionLengthOn(Vector3 const& base) const;

	QJsonObject getJSONRepresentation() const;

	/*! Prints this vector in a std::ostream as a nice string
	 *
	 * \param stream stream to print within
	 */
	virtual std::ostream& printInStream(std::ostream& stream) const;

  private:
	std::array<double, 3> data;
};

/*! Addition of two vectors
 * \relates Vector3
 *
 * Classic vector sum.
 *
 * Uses Vector3::operator+= so that it has the same addition semantic.
 * \param a first addition parameter
 * \param b second addition parameter
 */
Vector3 operator+(Vector3 const& a, Vector3 const& b);

/*! Subtraction of two vectors
 * \relates Vector3
 *
 * Classic vector subtraction.
 *
 * Uses Vector3::operator-= so that it has the same subtraction semantic.
 * \param a first subtraction parameter
 * \param b second subtraction parameter
 */
Vector3 operator-(Vector3 const& a, Vector3 const& b);

/*! Dot product of two vectors
 * \relates Vector3
 *
 * \param a first dot product parameter
 * \param b second dot product parameter
 */
double dotProduct(Vector3 const& a, Vector3 const& b);

/*! Cross product of two vectors
 * \relates Vector3
 *
 * \param a first cross product parameter
 * \param b second cross product parameter
 */
Vector3 crossProduct(Vector3 const& a, Vector3 const& b);

/*! Product between a vector and a scalar
 * \relates Vector3
 *
 * Uses Vector3::operator*= so that it has the same product semantic.
 * \param vector vector to be multiplied
 * \param scalar scalar to be multiplied
 */
Vector3 operator*(Vector3 const& vector, double scalar);

/*! Product between a scalar and a vector
 * \relates Vector3
 *
 * Uses Vector3::operator*= so that it has the same product semantic.
 * \param scalar scalar to be multiplied
 * \param vector vector to be multiplied
 */
Vector3 operator*(double scalar, Vector3 const& vector);

/*! Division between a vector and a scalar
 * \relates Vector3
 *
 * Uses Vector3::operator/= so that it has the same product semantic.
 * \param vector vector to be divided
 * \param scalar scalar which divides
 */
Vector3 operator/(Vector3 const& vector, double scalar);

/*! Tests equality between two vectors
 * \relates Vector3
 *
 * Each components have to be equal.
 * \param a first vector to compare to second
 * \param b second vector to compare to first
 */
bool operator==(Vector3 const& a, Vector3 const& b);

/*! Tests difference between two vectors
 * \relates Vector3
 *
 * Uses == so that it has the same equality semantic.
 * \param a first vector to compare to second
 * \param b second vector to compare to first
 */
bool operator!=(Vector3 const& a, Vector3 const& b);

/*! Printing in stream operator
 * \relates Vector3
 *
 * Uses Vector3::printInStream so that it uses the same string representation.
 * \param stream stream to print within
 * \param vector vector to be printed
 */
std::ostream& operator<<(std::ostream& stream, Vector3 const& vector);

/*! Distance between two points
 * \relates Vector3
 *
 * Uses Vector3::length so that it uses the same norm (L_2 in this case).
 *
 * \param pointA first point
 * \param pointB second point
 */
double distance(Vector3 const& pointA, Vector3 const& pointB);

#endif // VECTOR3D_HPP
