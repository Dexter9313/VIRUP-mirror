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

#include "methods/OctreeLOD.hpp"

#include "graphics/OrbitalSystemCamera.hpp"
#include "graphics/renderers/CelestialBodyRenderer.hpp"
#include "graphics/renderers/OrbitalSystemRenderer.hpp"
#include "math/Vector3.hpp"

// 3261.57860404 ly = 1kpc
// so we limit scale to 1 real meter = 0.01ly
// or 10 real meters = 0.1 ly
// 0.15ly is the closest two stars have been observed in ED
// #define SCALE_LIMIT 326157.860404

class MovementControls
{
  public:
	MovementControls(VRHandler const& vrHandler, BBox dataBBox,
	                 Camera* cosmoCam, OrbitalSystemCamera* cam);
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);
	void wheelEvent(QWheelEvent* e);
	void vrEvent(VRHandler::Event const& e,
	             QMatrix4x4 const& trackedSpaceToWorldTransform);
	void update(double frameTiming);

  private:
	void vrEventCube(VRHandler::Event const& e,
	                 QMatrix4x4 const& trackedSpaceToWorldTransform);
	void vrEventOrbitalSystem(VRHandler::Event const& e);
	void updateCube(double frameTiming);
	void updateOrbitalSystem(double frameTiming);

	VRHandler const& vrHandler;

	/* COMMON */
	QVector3D posVel;
	QVector3D negVel;
	float initControllersDistance;

	static void rescale(double newScale, Vector3 const& scaleCenter,
	                    Vector3& position, double& scale);

	/* CUBE */
	BBox dataBBox = {};
	Camera* cosmoCam;

	// scaling/translation controls variables
	bool leftGripPressedCube        = false;
	bool rightGripPressedCube       = false;
	Vector3 initControllerPosInCube = Vector3();
	Vector3 scaleCenterCube         = Vector3();
	double initScaleCube            = 1.0;

	/* ORBITAL SYSTEM */

	OrbitalSystemCamera* planetCam;

	// scaling/translation controls variables
	bool leftGripPressedOrb      = false;
	bool rightGripPressedOrb     = false;
	Vector3 initControllerRelPos = Vector3();
	Vector3 scaleCenterOrb       = Vector3();
	double initScaleOrb          = 0.0;
};

#endif // MOVEMENTCONTROLS_H
