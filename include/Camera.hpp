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
  public:
	Camera(VRHandler const* vrHandler);
	QVector3D getPosition() const { return position; };
	QVector3D getLookDirection() const { return lookDirection; };
	QVector3D getUp() const { return up; };
	virtual void update() override;
	bool shouldBeCulled(BBox const& bbox) const;

	float angle;
	float distance;
	float targetFPS;

	float currentFrameTiming = 0;

  private:
	QVector3D position;
	QVector3D lookDirection;
	QVector3D up;

	static QVector3D getCorner(BBox const& bBox, unsigned int i);
	static bool inFrustum(QVector3D const& projected);
};

#endif // CAMERA_H
