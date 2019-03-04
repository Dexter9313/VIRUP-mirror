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
	virtual void update() override;
	virtual void update2D() override;
	bool shouldBeCulled(BBox const& bbox, QMatrix4x4 const& model) const;

	float angleAroundZ = 0.f;
	float angleAboveXY = 0.f;
	float distance     = 1.f;
	float targetFPS    = 60.f;

	float currentFrameTiming = 0;

  public slots:
	QVector3D getPosition() const { return position; };
	QVector3D getLookDirection() const { return lookDirection; };
	QVector3D getUp() const { return up; };

	float getAngleAroundZ() const { return angleAroundZ; };
	void setAngleAroundZ(float angleAroundZ)
	{
		this->angleAroundZ = angleAroundZ;
	};
	float getAngleAboveXY() const { return angleAboveXY; };
	void setAngleAboveXY(float angleAboveXY)
	{
		this->angleAboveXY = angleAboveXY;
	};
	float getDistance() const { return distance; };
	void setDistance(float distance) { this->distance = distance; };
	float getTargetFPS() const { return targetFPS; };
	void setTargetFPS(float targetFPS) { this->targetFPS = targetFPS; };
	float getCurrentFrameTiming() const { return currentFrameTiming; };

  private:
	QVector3D position      = {1.f, 0.f, 0.f};
	QVector3D lookDirection = {-1.f, 0.f, 0.f};
	QVector3D up            = {0.f, 0.f, 1.f};

	void updateView();

	/* Not useful anymore
	static QVector4D getCorner(BBox const& bBox, unsigned int i);
	static bool inFrustum(QVector3D const& projected);
	*/
};

#endif // CAMERA_H
