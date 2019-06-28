/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@hotmail.fr>

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

#ifndef ORBITALSYSTEMCAMERA_H
#define ORBITALSYSTEMCAMERA_H

#include <cmath>

#include "BasicCamera.hpp"
#include "physics/CelestialBody.hpp"

class OrbitalSystemCamera : public BasicCamera
{
	Q_OBJECT
  public:
	OrbitalSystemCamera(VRHandler const* vrHandler);
	void updateUT(UniversalTime uT);
	bool shouldBeCulled(QVector3D const& spherePosition, float radius) const;
	Vector3 getRelativePositionTo(CelestialBody const* body,
	                              UniversalTime uT) const;

	Vector3 relativePosition = Vector3(10000000.0, 0.0, 0.0);

	float pitch = 0.f;
	float yaw   = 0.f;

	CelestialBody const* target;

  public slots:
	QVector3D getLookDirection() const { return lookDirection; };
	QVector3D getUp() const { return up; };

  private:
	QVector3D lookDirection = {-1.f, 0.f, 0.f};
	QVector3D up            = {0.f, 0.f, 1.f};

	void updateView();
};

#endif // ORBITALSYSTEMCAMERA_H
