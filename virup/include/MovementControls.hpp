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

#ifndef MOVEMENTCONTROLS_H
#define MOVEMENTCONTROLS_H

#include <QKeyEvent>

#include "Camera.hpp"
#include "vr/VRHandler.hpp"

class MovementControls
{
  public:
	MovementControls(VRHandler const& vrHandler, BBox dataBBox);
	double getCubeScale() const { return cubeScale; };
	std::array<double, 3> getCubeTranslation() const
	{
		return cubeTranslation;
	};
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);
	void wheelEvent(QWheelEvent* e);
	void vrEvent(VRHandler::Event const& e,
	             QMatrix4x4 trackedSpaceToWorldTransform);
	void update(Camera const& camera, double frameTiming);

  private:
	VRHandler const& vrHandler;

	BBox dataBBox                         = {};
	double cubeScale                      = 1.f;
	std::array<double, 3> cubeTranslation = {{0.f, 0.f, 0.f}};

	// scaling/translation controls variables
	bool leftGripPressed                          = false;
	bool rightGripPressed                         = false;
	float initControllersDistance                 = 1.f;
	std::array<double, 3> scaleCenter             = {{}};
	std::array<double, 3> initControllerPosInCube = {{}};
	double initScale                              = 1.0;

	// keyboard controls variables
	QVector3D cubePositiveVelocity;
	QVector3D cubeNegativeVelocity;

	void rescaleCube(double newScale, std::array<double, 3> const& scaleCenter
	                                  = {{0.0, 0.0, 0.0}});
};

#endif // MOVEMENTCONTROLS_H
