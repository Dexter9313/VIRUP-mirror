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

#include "math/Vector3.hpp"

Vector3::Vector3()
{
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
}

Vector3::Vector3(std::array<double, 3> data)
    : data(std::move(data))
{
}

Vector3::Vector3(double x, double y, double z)
{
	data[0] = x;
	data[1] = y;
	data[2] = z;
}

Vector3::Vector3(QJsonObject const& json, Vector3 const& defaultValue)
    : Vector3(json["x"].toDouble(defaultValue[0]),
              json["y"].toDouble(defaultValue[1]),
              json["z"].toDouble(defaultValue[2]))
{
}

void Vector3::setXYZ(double x, double y, double z)
{
	data[0] = x;
	data[1] = y;
	data[2] = z;
}

double Vector3::length() const
{
	return sqrt((data[0] * data[0]) + (data[1] * data[1])
	            + (data[2] * data[2]));
}

Vector3& Vector3::operator+=(Vector3 const& vectorToAdd)
{
	data[0] += vectorToAdd[0];
	data[1] += vectorToAdd[1];
	data[2] += vectorToAdd[2];

	return *this;
}

Vector3& Vector3::operator-=(Vector3 const& vectorToSubtract)
{
	data[0] -= vectorToSubtract[0];
	data[1] -= vectorToSubtract[1];
	data[2] -= vectorToSubtract[2];

	return *this;
}

Vector3& Vector3::operator*=(double scalar)
{
	data[0] *= scalar;
	data[1] *= scalar;
	data[2] *= scalar;

	return *this;
}

Vector3& Vector3::operator/=(double scalar)
{
	data[0] /= scalar;
	data[1] /= scalar;
	data[2] /= scalar;

	return *this;
}

double& Vector3::operator[](unsigned int index)
{
	if(index > 2)
		CRITICAL("Index out of bound.");
	return data[index];
}

double Vector3::operator[](unsigned int index) const
{
	if(index > 2)
		CRITICAL("Index out of bound.");
	return data[index];
}

Vector3 Vector3::getUnitForm() const
{
	double vLength(length());
	return Vector3(data[0] / vLength, data[1] / vLength, data[2] / vLength);
}

Vector3& Vector3::rotateAlongX(double angle)
{
	double oldY(data[1]);
	data[1] = cos(angle) * data[1] - sin(angle) * data[2];
	data[2] = sin(angle) * oldY + cos(angle) * data[2];

	return *this;
}

Vector3& Vector3::rotateAlongY(double angle)
{
	double oldX(data[0]);
	data[0] = cos(angle) * data[0] + sin(angle) * data[2];
	data[2] = -sin(angle) * oldX + cos(angle) * data[2];

	return *this;
}

Vector3& Vector3::rotateAlongZ(double angle)
{
	double oldX(data[0]);
	data[0] = cos(angle) * data[0] - sin(angle) * data[1];
	data[1] = sin(angle) * oldX + cos(angle) * data[1];

	return *this;
}

double Vector3::getProjectionLengthOn(Vector3 const& base) const
{
	return dotProduct(*this, base) / (length() * base.length());
}

QJsonObject Vector3::getJSONRepresentation() const
{
	return QJsonObject({{"x", data[0]}, {"y", data[1]}, {"z", data[2]}});
}

std::ostream& Vector3::printInStream(std::ostream& stream) const
{
	stream << "[ " << data[0] << ", " << data[1] << ", " << data[2] << " ]";
	return stream;
}

Vector3 operator+(Vector3 const& a, Vector3 const& b)
{
	Vector3 c(a);
	return c += b;
}

Vector3 operator-(Vector3 const& a, Vector3 const& b)
{
	Vector3 c(a);
	return c -= b;
}

double dotProduct(Vector3 const& a, Vector3 const& b)
{
	return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}

Vector3 crossProduct(Vector3 const& a, Vector3 const& b)
{
	return Vector3((a[1] * b[2]) - (a[2] * b[1]), (a[2] * b[0]) - (a[0] * b[2]),
	               (a[0] * b[1]) - (a[1] * b[0]));
}

Vector3 operator*(Vector3 const& vector, double scalar)
{
	Vector3 copy(vector);
	return copy *= scalar;
}

Vector3 operator*(double scalar, Vector3 const& vector)
{
	Vector3 copy(vector);
	return copy *= scalar;
}

Vector3 operator/(Vector3 const& vector, double scalar)
{
	Vector3 copy(vector);
	return copy /= scalar;
}

bool operator==(Vector3 const& a, Vector3 const& b)
{
	return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
}

bool operator!=(Vector3 const& a, Vector3 const& b)
{
	return !(a == b);
}

std::ostream& operator<<(std::ostream& stream, Vector3 const& vector)
{
	return vector.printInStream(stream);
}

double distance(Vector3 const& pointA, Vector3 const& pointB)
{
	return (pointB - pointA).length();
}
