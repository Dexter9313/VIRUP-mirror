/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@epfl.ch>

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

#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>

#include "BasicCamera.hpp"
#include "graphics/Utils.hpp"
#include "math/Vector3.hpp"

struct BBox
{
	float minx;
	float maxx;

	float miny;
	float maxy;

	float minz;
	float maxz;

	float diameter;
	QVector3D mid;
};

class Camera : public BasicCamera
{
	Q_OBJECT
  public:
	Camera(VRHandler const* vrHandler);
	Vector3 dataToWorldPosition(Vector3 const& data) const;
	QMatrix4x4 dataToWorldTransform() const;
	Vector3 worldToDataPosition(Vector3 const& world) const;
	QMatrix4x4 worldToDataTransform() const;
	Vector3 getHeadShift() const;
	// take head shift into account
	Vector3 getTruePosition() const;
	void updateTargetFPS();
	bool shouldBeCulled(BBox const& bbox, QMatrix4x4 const& model,
	                    bool depthClamp = false) const;

	Vector3 position = Vector3(0.0, 0.0, 0.0);
	double scale     = 1.0;

	float targetFPS = 60.f;

	float pitch = 0.f;
	float yaw   = 0.f;

	float currentFrameTiming = 0;

  public slots:
	QVector3D getLookDirection() const;
	float getTargetFPS() const { return targetFPS; };
	void setTargetFPS(float targetFPS) { this->targetFPS = targetFPS; };
	float getCurrentFrameTiming() const { return currentFrameTiming; };
};

#endif // CAMERA_H
