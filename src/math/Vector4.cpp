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

#include "math/Vector4.hpp"

Vector4::Vector4()
{
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 0;
}

Vector4::Vector4(std::array<double, 4> data)
    : data(std::move(data))
{
}

Vector4::Vector4(double x, double y, double z, double w)
{
	data[0] = x;
	data[1] = y;
	data[2] = z;
	data[3] = w;
}

Vector4::Vector4(Vector3 const& vec3, double w)
{
	data[0] = vec3[0];
	data[1] = vec3[1];
	data[2] = vec3[2];
	data[3] = w;
}

void Vector4::setXYZ(double x, double y, double z)
{
	data[0] = x;
	data[1] = y;
	data[2] = z;
}

void Vector4::setXYZW(double x, double y, double z, double w)
{
	data[0] = x;
	data[1] = y;
	data[2] = z;
	data[3] = w;
}

double Vector4::length() const
{
	return sqrt((data[0] * data[0]) + (data[1] * data[1])
	            + (data[2] * data[2]));
}

Vector4& Vector4::operator+=(Vector4 const& vectorToAdd)
{
	data[0] += vectorToAdd[0];
	data[1] += vectorToAdd[1];
	data[2] += vectorToAdd[2];
	if(data[3] == 0)
	{
		data[3] = vectorToAdd[3];
	}

	return *this;
}

Vector4& Vector4::operator-=(Vector4 const& vectorToSubtract)
{
	data[0] -= vectorToSubtract[0];
	data[1] -= vectorToSubtract[1];
	data[2] -= vectorToSubtract[2];
	if(data[3] == 0)
	{
		data[3] = vectorToSubtract[3];
	}

	return *this;
}

Vector4& Vector4::operator*=(double scalar)
{
	data[0] *= scalar;
	data[1] *= scalar;
	data[2] *= scalar;

	return *this;
}

Vector4& Vector4::operator/=(double scalar)
{
	data[0] /= scalar;
	data[1] /= scalar;
	data[2] /= scalar;

	return *this;
}

double& Vector4::operator[](unsigned int index)
{
	if(index > 3)
		CRITICAL("Index out of bound.");
	return data[index];
}

double Vector4::operator[](unsigned int index) const
{
	if(index > 3)
		CRITICAL("Index out of bound.");
	return data[index];
}

Vector4 Vector4::getUnitForm() const
{
	double vLength(length());
	return Vector4(data[0] / vLength, data[1] / vLength, data[2] / vLength,
	               data[3]);
}

Vector4& Vector4::rotateAlongX(double angle)
{
	double oldY(data[1]);
	data[1] = cos(angle) * data[1] - sin(angle) * data[2];
	data[2] = sin(angle) * oldY + cos(angle) * data[2];

	return *this;
}

Vector4& Vector4::rotateAlongY(double angle)
{
	double oldX(data[0]);
	data[0] = cos(angle) * data[0] + sin(angle) * data[2];
	data[2] = -sin(angle) * oldX + cos(angle) * data[2];

	return *this;
}

Vector4& Vector4::rotateAlongZ(double angle)
{
	double oldX(data[0]);
	data[0] = cos(angle) * data[0] - sin(angle) * data[1];
	data[1] = sin(angle) * oldX + cos(angle) * data[1];

	return *this;
}

double Vector4::getProjectionLengthOn(Vector4 const& base) const
{
	return dotProduct(*this, base) / (length() * base.length());
}

std::ostream& Vector4::printInStream(std::ostream& stream) const
{
	stream << "[ " << data[0] << ", " << data[1] << ", " << data[2] << ", "
	       << data[3] << " ]";
	return stream;
}

Vector4 operator+(Vector4 const& a, Vector4 const& b)
{
	Vector4 c(a);
	return c += b;
}

Vector4 operator-(Vector4 const& a, Vector4 const& b)
{
	Vector4 c(a);
	return c -= b;
}

double dotProduct(Vector4 const& a, Vector4 const& b)
{
	return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}

Vector4 crossProduct(Vector4 const& a, Vector4 const& b)
{
	return Vector4((a[1] * b[2]) - (a[2] * b[1]), (a[2] * b[0]) - (a[0] * b[2]),
	               (a[0] * b[1]) - (a[1] * b[0]),
	               (a[3] == 1 && b[3] == 1) ? 1 : 0);
}

Vector4 operator*(Vector4 const& vector, double scalar)
{
	Vector4 copy(vector);
	return copy *= scalar;
}

Vector4 operator*(double scalar, Vector4 const& vector)
{
	Vector4 copy(vector);
	return copy *= scalar;
}

Vector4 operator/(Vector4 const& vector, double scalar)
{
	Vector4 copy(vector);
	return copy /= scalar;
}

bool operator==(Vector4 const& a, Vector4 const& b)
{
	return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3];
}

bool operator!=(Vector4 const& a, Vector4 const& b)
{
	return !(a == b);
}

std::ostream& operator<<(std::ostream& stream, Vector4 const& vector)
{
	return vector.printInStream(stream);
}
