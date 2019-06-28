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

#ifndef MATRIX4X4_HPP
#define MATRIX4X4_HPP

// \TOCHECK what is called "classic" is classic

#include <array>
#include <iostream>

#include "Vector4.hpp"

/*! \ingroup math
 * Mainly used to represent as a 4x4 matrix an [affine
 * transformation](https://en.wikipedia.org/wiki/Transformation_matrix#Affine_transformations)
 * on 3D vectors
 * which are represented as #Vector4 in [homogeneous
 * coordinates](https://en.wikipedia.org/wiki/Homogeneous_coordinates#Use_in_computer_graphics).
 *
 * See \ref math group description for conventions.
 *
 * This class is used whenever possible instead of any library matrix class to
 * ensure libraries can be changed easily if needed. Usage of another
 * library matrix class should only be done for performance reasons (likely in
 * the \ref math module where algebraic algorithms are written) and should be
 * properly documented.
 */
class Matrix4x4
{
  public:
	/*! Default constructor
	 *
	 * Constructs the identity matrix [1 0 0 0; 0 1 0 0; 0 0 1 0; 0 0 0 1].
	 * This corresponds to the identity transform (does nothing).
	 */
	Matrix4x4();

	/*! Default copy constructor
	 *
	 * Copies all values of components.
	 * \param copiedMatrix Matrix4x4 to be copied
	 */
	Matrix4x4(Matrix4x4 const& copiedMatrix) = default;

	/*! Default move constructor
	 *
	 * Moves all values of components.
	 * \param movedMatrix Matrix4x4 to be moved
	 */
	Matrix4x4(Matrix4x4&& movedMatrix) = default;

	/*! Scalar matrix constructor
	 *
	 * Constructs the scalar matrix [scalar 0 0 0; 0 scalar 0 0; 0 0 scalar 0; 0
	 * 0 0 1].
	 * Such a matrix would serve as a scaling transform (using the same scale
	 * for every dimension).
	 * \param fromScalar scalar to construct the matrix
	 */
	Matrix4x4(double fromScalar);

	/*! Translation matrix constructor
	 *
	 * If fromTranslation = [x; y; z; w]
	 * Constructs the translation matrix [1 0 0 x; 0 1 0 y; 0 0 1 z; 0 0 0 1].
	 * Such a matrix would serve as a translation transform along vector [x; y;
	 * z].
	 * \param fromTranslation translation vector
	 */
	Matrix4x4(Vector4 const& fromTranslation);

	/*! [Rotation
	 * matrix](https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle)
	 * constructor
	 *
	 * Constructs the rotation matrix that, as a transform,
	 * rotates vectors around the aroundAxis axis with angle fromRotationAngle.
	 * Last line and column are equal to [0 0 0 1] and [0; 0; 0; 1]
	 * respectively.
	 * \param fromRotationAngle angle of the rotation
	 * \param aroundAxis
	 */
	Matrix4x4(double fromRotationAngle, Vector4 const& aroundAxis);

	/*! Constructor from coefficients
	 *
	 * Constructs the matrix from an array of coefficients for the first three
	 * lines.
	 * The outermost array contains the matrix lines which are arrays.
	 * Last line will be appended as [0 0 0 1].
	 * \param data array of coefficients
	 */
	Matrix4x4(std::array<std::array<double, 4>, 3> data);

	/*! Constructor from coefficients
	 *
	 * Constructs the matrix from an array of coefficients.
	 * The outermost array contains the matrix lines which are arrays.
	 *
	 * \param data array of coefficients
	 *
	 * \throw CriticalException if and only if the last line is different from
	 * [0 0 0 1].
	 */
	Matrix4x4(std::array<std::array<double, 4>, 4> data);

	/*! Multiplies another matrix to this matrix
	 *
	 * Classic [matrix
	 * product](https://en.wikipedia.org/wiki/Matrix_multiplication) is used.
	 * Assigns (*this) * matrixToMultiply to this. Notice this matrix is the
	 * left member of the operation.
	 * In transformation terms, this becomes the transformation (this °
	 * matrixToMultiply),
	 * with ° being the [composition
	 * operator](https://en.wikipedia.org/wiki/Function_composition).
	 * \param matrixToMultiply Matrix4x4 to be multiplied.
	 */
	Matrix4x4& operator*=(Matrix4x4 const& matrixToMultiply);

	/*! Multiplies this matrix by a scalar
	 *
	 * Classic element-wise scalar product is used.
	 * Similar to constructing a scaling transformation S by scalar scalar and
	 * apply this transformation on top of it.
	 * this becomes the transformation (this ° S)
	 * with ° being the [composition
	 * operator](https://en.wikipedia.org/wiki/Function_composition).
	 * \param scalar to be multiply with.
	 */
	Matrix4x4& operator*=(double scalar);

	/*! Divides this matrix by a scalar
	 *
	 * Classic element-wise scalar product is used.
	 * Similar to constructing a scaling transformation S by scalar 1/scalar and
	 * apply this transformation on top of it.
	 * this becomes the transformation (this ° S)
	 * with ° being the [composition
	 * operator](https://en.wikipedia.org/wiki/Function_composition).
	 * \param scalar to be divide with.
	 */
	Matrix4x4& operator/=(double scalar);

	/*! Default copy assignment operator
	 *
	 * Copies all values of components.
	 * \param copiedMatrix Matrix4x4 to be copied
	 */
	Matrix4x4& operator=(Matrix4x4 const& copiedMatrix) = default;

	/*! Default move assignment operator
	 *
	 * Moves all values of components.
	 * \param movedMatrix Matrix4x4 to be moved
	 */
	Matrix4x4& operator=(Matrix4x4&& movedMatrix) = default;

	/*! Returns a reference to a specific line of this matrix
	 *
	 * Indexes correspondance :
	 *
	 * 0 <-> first line
	 *
	 * 1 <-> second line
	 *
	 * 2 <-> third line
	 *
	 * 3 <-> fourth line
	 *
	 * \param index index of the line
	 *
	 * \throw CriticalException if and only if index > 3
	 */
	std::array<double, 4>& operator[](unsigned int index);

	/*! Returns the value of a specific line of this matrix
	 *
	 * Indexes correspondance :
	 *
	 * 0 <-> first line
	 *
	 * 1 <-> second line
	 *
	 * 2 <-> third line
	 *
	 * 3 <-> fourth line
	 *
	 * \param index index of the line
	 *
	 * \throw CriticalException if and only if index > 3
	 */
	std::array<double, 4> operator[](unsigned int index) const;

	/*! Returns the value of a specific column of this matrix
	 *
	 * Indexes correspondance :
	 *
	 * 0 <-> first column
	 *
	 * 1 <-> second column
	 *
	 * 2 <-> third column
	 *
	 * 3 <-> fourth column
	 *
	 * \param index index of the column
	 *
	 * \throw CriticalException if and only if index > 3
	 */
	std::array<double, 4> getColumn(unsigned int index) const;

	/*! Prints this matrix in a std::ostream as a nice string
	 *
	 * \param stream stream to print within
	 */
	std::ostream& printInStream(std::ostream& stream) const;

  private:
	std::array<std::array<double, 4>, 4> data;
};

/*! Product of two matrices
 * \relates Matrix4x4
 *
 * Classic [matrix
 * multiplication](https://en.wikipedia.org/wiki/Matrix_multiplication).
 *
 * Uses Matrix4x4::operator*= so that it has the same multiplication semantic.
 * \param a left matrix of the product
 * \param b right matrix of the product
 */
Matrix4x4 operator*(Matrix4x4 const& a, Matrix4x4 const& b);

/*! Product between a matrix and a vector
 * \relates Matrix4x4
 * \relates Vector4
 *
 * Implements [usual
 * definition](https://en.wikipedia.org/wiki/Matrix_multiplication) considering
 * a vector is a one-column matrix.
 * \param matrix matrix to be multiplied
 * \param vector vector to be multiplied
 */
Vector4 operator*(Matrix4x4 const& matrix, Vector4 const& vector);

/*! Product between a matrix and a vector
 * \relates Matrix4x4
 * \relates Vector3
 *
 * Implements [usual
 * definition](https://en.wikipedia.org/wiki/Matrix_multiplication) considering
 * a vector is a one-column matrix. matrix is considered to be a 3x3 matrix
 * (last line and last column are ignored).
 * \param matrix matrix to be multiplied
 * \param vector vector to be multiplied
 */
Vector3 operator*(Matrix4x4 const& matrix, Vector3 const& vector);

/*! Product between a scalar and a matrix
 * \relates Matrix4x4
 *
 * Equivalent to constructing a scalar matrix S from scalar and multiplying it
 * by matrix as follows :
 * S * matrix .
 *
 * Notice that scalar * matrix != matrix * scalar (order of scale application if
 * matrix uses translation matters !). It is equal only if matrix doesn't use
 * any translation.
 * \param scalar scalar to be multiplied
 * \param matrix matrix to be multiplied
 */
Matrix4x4 operator*(double scalar, Matrix4x4 const& matrix);

/*! Product between a matrix and a scalar
 * \relates Matrix4x4
 *
 * Uses Matrix4x4::operator*= so that it has the same product semantic.
 *
 * Notice that matrix * scalar != scalar * matrix (order of scale application if
 * matrix uses translation matters !). It is equal only if matrix doesn't use
 * any translation.
 * \param matrix matrix to be multiplied
 * \param scalar scalar to be multiplied
 */
Matrix4x4 operator*(Matrix4x4 const& matrix, double scalar);

/*! Division between a matrix and a scalar
 * \relates Matrix4x4
 *
 * Uses Matrix4x4::operator/= so that it has the same product semantic.
 * \param matrix matrix to be divided
 * \param scalar scalar which divides
 */
Matrix4x4 operator/(Matrix4x4 const& matrix, double scalar);

/*! Tests equality between two matrices
 * \relates Matrix4x4
 *
 * Each components have to be equal.
 * \param a first matrix to compare to second
 * \param b second matrix to compare to first
 */
bool operator==(Matrix4x4 const& a, Matrix4x4 const& b);

/*! Tests difference between two matrices
 * \relates Matrix4x4
 *
 * Uses == so that it has the same equality semantic.
 * \param a first matrix to compare to second
 * \param b second matrix to compare to first
 */
bool operator!=(Matrix4x4 const& a, Matrix4x4 const& b);

/*! Printing in stream operator
 * \relates Matrix4x4
 *
 * Uses Matrix4x4::printInStream so that it uses the same string representation.
 * \param stream stream to print within
 * \param matrix matrix to be printed
 */
std::ostream& operator<<(std::ostream& stream, Matrix4x4 const& matrix);

#endif // MATRIX3X3_HPP
