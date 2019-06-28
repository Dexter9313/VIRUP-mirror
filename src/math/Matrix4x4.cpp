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

#include "../../include/math/Matrix4x4.hpp"

Matrix4x4::Matrix4x4()
{
	data[0][0] = 1;
	data[0][1] = 0;
	data[0][2] = 0;
	data[0][3] = 0;

	data[1][0] = 0;
	data[1][1] = 1;
	data[1][2] = 0;
	data[1][3] = 0;

	data[2][0] = 0;
	data[2][1] = 0;
	data[2][2] = 1;
	data[2][3] = 0;

	data[3][0] = 0;
	data[3][1] = 0;
	data[3][2] = 0;
	data[3][3] = 1;
}

Matrix4x4::Matrix4x4(double fromScalar)
    : Matrix4x4()
{
	data[0][0] = fromScalar;
	data[0][1] = 0;
	data[0][2] = 0;
	data[0][3] = 0;

	data[1][0] = 0;
	data[1][1] = fromScalar;
	data[1][2] = 0;
	data[1][3] = 0;

	data[2][0] = 0;
	data[2][1] = 0;
	data[2][2] = fromScalar;
	data[2][3] = 0;

	data[3][0] = 0;
	data[3][1] = 0;
	data[3][2] = 0;
	data[3][3] = 1;
}

Matrix4x4::Matrix4x4(Vector4 const& fromTranslation)
    : Matrix4x4()
{
	data[3][0] = fromTranslation[0];
	data[3][0] = fromTranslation[1];
	data[3][0] = fromTranslation[2];
}

Matrix4x4::Matrix4x4(double fromRotationAngle, Vector4 const& aroundAxis)
    : Matrix4x4()
{
	Vector4 unitAxis = aroundAxis.getUnitForm();
	double cosAngle(cos(fromRotationAngle)), sinAngle(sin(fromRotationAngle));

	double x(unitAxis[0]), y(unitAxis[1]), z(unitAxis[2]);

	data[0][0] = cosAngle + x * x * (1 - cosAngle);
	data[0][1] = x * y * (1 - cosAngle) - z * sinAngle;
	data[0][2] = x * z * (1 - cosAngle) + y * sinAngle;

	data[1][0] = x * y * (1 - cosAngle) + z * sinAngle;
	data[1][1] = cosAngle + y * y * (1 - cosAngle);
	data[1][2] = y * z * (1 - cosAngle) - x * sinAngle;

	data[2][0] = x * z * (1 - cosAngle) - y * sinAngle;
	data[2][1] = y * z * (1 - cosAngle) + x * sinAngle;
	data[2][2] = cosAngle + z * z * (1 - cosAngle);
}

Matrix4x4::Matrix4x4(std::array<std::array<double, 4>, 3> data)
{
	this->data[0] = data[0];
	this->data[1] = data[1];
	this->data[2] = data[2];

	this->data[3][0] = 0;
	this->data[3][1] = 0;
	this->data[3][2] = 0;
	this->data[3][3] = 1;
}

Matrix4x4::Matrix4x4(std::array<std::array<double, 4>, 4> data)
    : data(std::move(data))
{
	if(data[3][0] != 0 || data[3][1] != 0 || data[3][2] != 0 || data[3][3] != 1)
	{
		CRITICAL("Invalid matrix : last line isn't [0; 0; 0; 1].");
	}
}

Matrix4x4& Matrix4x4::operator*=(Matrix4x4 const& matrixToMultiply)
{
	std::array<std::array<double, 4>, 4> oldData(data);

	for(unsigned int i(0); i < 4; ++i)
	{
		for(unsigned int j(0); j < 4; ++j)
		{
			data[i][j] = oldData[i][0] * matrixToMultiply[0][j]
			             + oldData[i][1] * matrixToMultiply[1][j]
			             + oldData[i][2] * matrixToMultiply[2][j]
			             + oldData[i][3] * matrixToMultiply[3][j];
		}
	}

	return *this;
}

Matrix4x4& Matrix4x4::operator*=(double scalar)
{
	data[0][0] *= scalar;
	data[0][1] *= scalar;
	data[0][2] *= scalar;

	data[1][0] *= scalar;
	data[1][1] *= scalar;
	data[1][2] *= scalar;

	data[2][0] *= scalar;
	data[2][1] *= scalar;
	data[2][2] *= scalar;

	return *this;
}

Matrix4x4& Matrix4x4::operator/=(double scalar)
{
	data[0][0] /= scalar;
	data[0][1] /= scalar;
	data[0][2] /= scalar;

	data[1][0] /= scalar;
	data[1][1] /= scalar;
	data[1][2] /= scalar;

	data[2][0] /= scalar;
	data[2][1] /= scalar;
	data[2][2] /= scalar;

	return *this;
}

std::array<double, 4>& Matrix4x4::operator[](unsigned int index)
{
	if(index > 3)
	{
		CRITICAL("Index out of bounds.");
	}
	return data[index];
}

std::array<double, 4> Matrix4x4::operator[](unsigned int index) const
{
	if(index > 3)
	{
		CRITICAL("Index out of bounds.");
	}
	return data[index];
}

std::array<double, 4> Matrix4x4::getColumn(unsigned int index) const
{
	if(index > 3)
	{
		CRITICAL("Index out of bounds.");
	}
	return {{data[0][index], data[1][index], data[2][index], data[3][index]}};
}

std::ostream& Matrix4x4::printInStream(std::ostream& stream) const
{
	stream << "[ " << data[0][0] << ", " << data[0][1] << ", " << data[0][2]
	       << ", " << data[0][3] << std::endl;
	stream << data[1][0] << ", " << data[1][1] << ", " << data[1][2] << ", "
	       << data[1][3] << std::endl;
	stream << data[2][0] << ", " << data[2][1] << ", " << data[2][2] << ", "
	       << data[2][3] << std::endl;
	stream << data[3][0] << ", " << data[3][1] << ", " << data[3][2] << ", "
	       << data[3][3] << "]";
	return stream;
}

Matrix4x4 operator*(Matrix4x4 const& a, Matrix4x4 const& b)
{
	Matrix4x4 c(a);
	c *= b;
	return c;
}

Vector4 operator*(Matrix4x4 const& matrix, Vector4 const& vector)
{
	Vector4 result(vector);

	// w is invariant
	for(unsigned int i(0); i < 3; ++i)
	{
		result[i] = matrix[i][0] * vector[0] + matrix[i][1] * vector[1]
		            + matrix[i][2] * vector[2] + matrix[i][3] * vector[3];
	}

	return result;
}

Vector3 operator*(Matrix4x4 const& matrix, Vector3 const& vector)
{
	Vector3 result(vector);

	for(unsigned int i(0); i < 3; ++i)
	{
		result[i] = matrix[i][0] * vector[0] + matrix[i][1] * vector[1]
		            + matrix[i][2] * vector[2];
	}

	return result;
}

Matrix4x4 operator*(double scalar, Matrix4x4 const& matrix)
{
	return Matrix4x4(scalar) * matrix;
}

Matrix4x4 operator*(Matrix4x4 const& matrix, double scalar)
{
	Matrix4x4 copy(matrix);
	copy *= scalar;
	return copy;
}

Matrix4x4 operator/(Matrix4x4 const& matrix, double scalar)
{
	Matrix4x4 copy(matrix);
	copy /= scalar;
	return copy;
}

bool operator==(Matrix4x4 const& a, Matrix4x4 const& b)
{
	for(unsigned int i(0); i < 4; ++i)
	{
		for(unsigned int j(0); j < 4; ++j)
		{
			if(a[i][j] != b[i][j])
			{
				return false;
			}
		}
	}

	return true;
}

bool operator!=(Matrix4x4 const& a, Matrix4x4 const& b)
{
	return !(a == b);
}

std::ostream& operator<<(std::ostream& stream, Matrix4x4 const& matrix)
{
	return matrix.printInStream(stream);
}
