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

#include "MovementControls.hpp"

MovementControls::MovementControls(VRHandler const& vrHandler, BBox dataBBox,
                                   OrbitalSystemCamera* cam)
    : vrHandler(vrHandler)
    , dataBBox(dataBBox)
    , cam(cam)
{
	if((dataBBox.maxx - dataBBox.minx >= dataBBox.maxy - dataBBox.miny)
	   && (dataBBox.maxx - dataBBox.minx >= dataBBox.maxz - dataBBox.minz))
	{
		cubeScale = 1.f / (dataBBox.maxx - dataBBox.minx);
	}
	else if(dataBBox.maxy - dataBBox.miny >= dataBBox.maxz - dataBBox.minz)
	{
		cubeScale = 1.f / (dataBBox.maxy - dataBBox.miny);
	}
	else
	{
		cubeScale = 1.f / (dataBBox.maxz - dataBBox.minz);
	}
	cubeTranslation[0] = -1 * dataBBox.mid.x() * cubeScale;
	cubeTranslation[1] = -1 * dataBBox.mid.y() * cubeScale;
	cubeTranslation[2] = -1 * dataBBox.mid.z() * cubeScale;
}

void MovementControls::keyPressEvent(QKeyEvent* e)
{
	keyPressEventCube(e);
	keyPressEventOrbitalSystem(e);
}

void MovementControls::keyPressEventCube(QKeyEvent* e)
{
	if(e->key() == Qt::Key_W || e->key() == Qt::Key_Up)
	{
		cubePositiveVelocity.setZ(1);
	}
	else if(e->key() == Qt::Key_A || e->key() == Qt::Key_Left)
	{
		cubePositiveVelocity.setX(1);
	}
	else if(e->key() == Qt::Key_S || e->key() == Qt::Key_Down)
	{
		cubeNegativeVelocity.setZ(-1);
	}
	else if(e->key() == Qt::Key_D || e->key() == Qt::Key_Right)
	{
		cubeNegativeVelocity.setX(-1);
	}
}

void MovementControls::keyPressEventOrbitalSystem(QKeyEvent* e)
{
	if(e->key() == Qt::Key_C)
	{
		Vector3 unitRelPos(cam->relativePosition.getUnitForm());
		cam->yaw   = atan2(unitRelPos[1], unitRelPos[0]);
		cam->pitch = -1.0 * asin(unitRelPos[2]);
	}
	else if(e->key() == Qt::Key_W || e->key() == Qt::Key_Up)
	{
		negativeVelocity.setZ(-1);
	}
	else if(e->key() == Qt::Key_A || e->key() == Qt::Key_Left)
	{
		negativeVelocity.setX(-1);
	}
	else if(e->key() == Qt::Key_S || e->key() == Qt::Key_Down)
	{
		positiveVelocity.setZ(1);
	}
	else if(e->key() == Qt::Key_D || e->key() == Qt::Key_Right)
	{
		positiveVelocity.setX(1);
	}
}

void MovementControls::keyReleaseEvent(QKeyEvent* e)
{
	keyReleaseEventCube(e);
	keyReleaseEventOrbitalSystem(e);
}

void MovementControls::keyReleaseEventCube(QKeyEvent* e)
{
	if(e->key() == Qt::Key_W || e->key() == Qt::Key_Up)
	{
		cubePositiveVelocity.setZ(0);
	}
	else if(e->key() == Qt::Key_A || e->key() == Qt::Key_Left)
	{
		cubePositiveVelocity.setX(0);
	}
	else if(e->key() == Qt::Key_S || e->key() == Qt::Key_Down)
	{
		cubeNegativeVelocity.setZ(0);
	}
	else if(e->key() == Qt::Key_D || e->key() == Qt::Key_Right)
	{
		cubeNegativeVelocity.setX(0);
	}
}

void MovementControls::keyReleaseEventOrbitalSystem(QKeyEvent* e)
{
	if(e->key() == Qt::Key_W || e->key() == Qt::Key_Up)
	{
		negativeVelocity.setZ(0);
	}
	else if(e->key() == Qt::Key_A || e->key() == Qt::Key_Left)
	{
		negativeVelocity.setX(0);
	}
	else if(e->key() == Qt::Key_S || e->key() == Qt::Key_Down)
	{
		positiveVelocity.setZ(0);
	}
	else if(e->key() == Qt::Key_D || e->key() == Qt::Key_Right)
	{
		positiveVelocity.setX(0);
	}
}

void MovementControls::wheelEvent(QWheelEvent* e)
{
	rescaleCube(cubeScale * (1.f + e->angleDelta().y() / 1000.f));
	CelestialBodyRenderer::overridenScale
	    *= (1.f + e->angleDelta().y() / 1000.f);
}

void MovementControls::vrEvent(VRHandler::Event const& e,
                               QMatrix4x4 const& trackedSpaceToWorldTransform)
{
	vrEventCube(e, trackedSpaceToWorldTransform);
	if(OctreeLOD::renderPlanetarySystem)
	{
		vrEventOrbitalSystem(e);
	}
}

void MovementControls::vrEventCube(
    VRHandler::Event const& e, QMatrix4x4 const& trackedSpaceToWorldTransform)
{
	switch(e.type)
	{
		case VRHandler::EventType::BUTTON_PRESSED:
			switch(e.button)
			{
				case VRHandler::Button::GRIP:
				{
					Controller const* left(vrHandler.getController(Side::LEFT));
					Controller const* right(
					    vrHandler.getController(Side::RIGHT));
					if(e.side == Side::LEFT && left != nullptr)
					{
						leftGripPressedCube = true;
						initControllerPosInCube
						    = Utils::fromQt(trackedSpaceToWorldTransform
						                    * left->getPosition())
						      - cubeTranslation;
					}
					else if(e.side == Side::RIGHT && right != nullptr)
					{
						rightGripPressedCube = true;
						initControllerPosInCube
						    = Utils::fromQt(trackedSpaceToWorldTransform
						                    * right->getPosition())
						      - cubeTranslation;
					}
					else
					{
						break;
					}
					if(leftGripPressedCube && rightGripPressedCube
					   && left != nullptr && right != nullptr)
					{
						initControllersDistance
						    = left->getPosition().distanceToPoint(
						        right->getPosition());
						initScaleCube = cubeScale;
						Vector3 controllersMidPoint(
						    Utils::fromQt(left->getPosition()
						                  + right->getPosition())
						    / 2.0);

						controllersMidPoint
						    = Utils::fromQt(trackedSpaceToWorldTransform
						                    * Utils::toQt(controllersMidPoint));
						scaleCenterCube = controllersMidPoint;

						Vector3 controllersMidPointInCube;

						controllersMidPointInCube
						    = (controllersMidPoint - cubeTranslation)
						      / cubeScale;

						if(controllersMidPointInCube[0] < dataBBox.minx
						   || controllersMidPointInCube[0] > dataBBox.maxx
						   || controllersMidPointInCube[1] < dataBBox.miny
						   || controllersMidPointInCube[1] > dataBBox.maxy
						   || controllersMidPointInCube[2] < dataBBox.minz
						   || controllersMidPointInCube[2] > dataBBox.maxz)
						{
							scaleCenterCube = cubeTranslation;
						}
					}
					break;
				}
				default:
					break;
			}
			break;
		case VRHandler::EventType::BUTTON_UNPRESSED:
			switch(e.button)
			{
				case VRHandler::Button::GRIP:
				{
					Controller const* left(vrHandler.getController(Side::LEFT));
					Controller const* right(
					    vrHandler.getController(Side::RIGHT));
					if(e.side == Side::LEFT)
					{
						leftGripPressedCube = false;
						if(right != nullptr && rightGripPressedCube)
						{
							initControllerPosInCube
							    = Utils::fromQt(trackedSpaceToWorldTransform
							                    * right->getPosition())
							      - cubeTranslation;
						}
					}
					else if(e.side == Side::RIGHT)
					{
						rightGripPressedCube = false;
						if(left != nullptr && leftGripPressedCube)
						{
							initControllerPosInCube
							    = Utils::fromQt(trackedSpaceToWorldTransform
							                    * left->getPosition())
							      - cubeTranslation;
						}
					}
					break;
				}
				default:
					break;
			}
			break;
		default:
			break;
	}
}

void MovementControls::vrEventOrbitalSystem(VRHandler::Event const& e)
{
	QMatrix4x4 trackedSpaceToWorldTransform(
	    cam->trackedSpaceToWorldTransform());
	switch(e.type)
	{
		case VRHandler::EventType::BUTTON_PRESSED:
			switch(e.button)
			{
				case VRHandler::Button::GRIP:
				{
					OrbitalSystemRenderer::autoCameraTarget = false;
					Controller const* left(vrHandler.getController(Side::LEFT));
					Controller const* right(
					    vrHandler.getController(Side::RIGHT));
					if(e.side == Side::LEFT && left != nullptr)
					{
						leftGripPressedOrb = true;
						initControllerRelPos
						    = Utils::fromQt(trackedSpaceToWorldTransform
						                    * left->getPosition())
						          / CelestialBodyRenderer::overridenScale
						      + cam->relativePosition;
					}
					else if(e.side == Side::RIGHT && right != nullptr)
					{
						rightGripPressedOrb = true;
						initControllerRelPos
						    = Utils::fromQt(trackedSpaceToWorldTransform
						                    * right->getPosition())
						          / CelestialBodyRenderer::overridenScale
						      + cam->relativePosition;
					}
					else
					{
						break;
					}
					if(leftGripPressedOrb && rightGripPressedOrb
					   && left != nullptr && right != nullptr)
					{
						initControllersDistance
						    = left->getPosition().distanceToPoint(
						        right->getPosition());
						initScaleOrb = CelestialBodyRenderer::overridenScale;

						QVector3D controllersMidPoint;
						controllersMidPoint
						    = left->getPosition() + right->getPosition();
						controllersMidPoint /= 2.f;

						controllersMidPoint = trackedSpaceToWorldTransform
						                      * controllersMidPoint;
						scaleCenterOrb
						    = Utils::fromQt(controllersMidPoint)
						          / CelestialBodyRenderer::overridenScale
						      + cam->relativePosition;
					}
					break;
				}
				default:
					break;
			}
			break;
		case VRHandler::EventType::BUTTON_UNPRESSED:
			switch(e.button)
			{
				case VRHandler::Button::GRIP:
				{
					Controller const* left(vrHandler.getController(Side::LEFT));
					Controller const* right(
					    vrHandler.getController(Side::RIGHT));
					if(e.side == Side::LEFT)
					{
						leftGripPressedOrb = false;
						if(right != nullptr && rightGripPressedOrb)
						{
							initControllerRelPos
							    = Utils::fromQt(trackedSpaceToWorldTransform
							                    * right->getPosition())
							          / CelestialBodyRenderer::overridenScale
							      + cam->relativePosition;
						}
						else
						{
							OrbitalSystemRenderer::autoCameraTarget = true;
						}
					}
					else if(e.side == Side::RIGHT)
					{
						rightGripPressedOrb = false;
						if(left != nullptr && leftGripPressedOrb)
						{
							initControllerRelPos
							    = Utils::fromQt(trackedSpaceToWorldTransform
							                    * left->getPosition())
							          / CelestialBodyRenderer::overridenScale
							      + cam->relativePosition;
						}
						else
						{
							OrbitalSystemRenderer::autoCameraTarget = true;
						}
					}
					break;
				}
				default:
					break;
			}
			break;
		default:
			break;
	}
}

void MovementControls::update(Camera const& camera, double frameTiming)
{
	updateCube(camera, frameTiming);
	if(OctreeLOD::renderPlanetarySystem)
	{
		updateOrbitalSystem(frameTiming);
	}
}

void MovementControls::updateCube(Camera const& camera, double frameTiming)
{
	Controller const* left(vrHandler.getController(Side::LEFT));
	Controller const* right(vrHandler.getController(Side::RIGHT));

	// single grip = translation
	if(leftGripPressedCube != rightGripPressedCube)
	{
		Vector3 controllerPosInCube = {};
		if(leftGripPressedCube && left != nullptr)
		{
			controllerPosInCube
			    = Utils::fromQt(camera.trackedSpaceToWorldTransform()
			                    * left->getPosition())
			      - cubeTranslation;
		}
		else if(rightGripPressedCube && right != nullptr)
		{
			controllerPosInCube
			    = Utils::fromQt(camera.trackedSpaceToWorldTransform()
			                    * right->getPosition())
			      - cubeTranslation;
		}
		cubeTranslation += controllerPosInCube - initControllerPosInCube;
	}
	// double grip = scale
	if(leftGripPressedCube && rightGripPressedCube && left != nullptr
	   && right != nullptr)
	{
		rescaleCube(
		    initScaleCube
		        * left->getPosition().distanceToPoint(right->getPosition())
		        / initControllersDistance,
		    scaleCenterCube);
	}

	// apply keyboard controls
	if(!vrHandler)
	{
		cubeTranslation
		    += frameTiming
		       * Utils::fromQt(camera.getView().inverted()
		                       * (cubePositiveVelocity + cubeNegativeVelocity));
	}
}

void MovementControls::updateOrbitalSystem(double frameTiming)
{
	Controller const* left(vrHandler.getController(Side::LEFT));
	Controller const* right(vrHandler.getController(Side::RIGHT));

	// single grip = translation
	if(leftGripPressedOrb != rightGripPressedOrb)
	{
		Vector3 controllerRelPos;
		if(leftGripPressedOrb && left != nullptr)
		{
			controllerRelPos = Utils::fromQt(cam->trackedSpaceToWorldTransform()
			                                 * left->getPosition())
			                       / CelestialBodyRenderer::overridenScale
			                   + cam->relativePosition;
		}
		else if(rightGripPressedOrb && right != nullptr)
		{
			controllerRelPos = Utils::fromQt(cam->trackedSpaceToWorldTransform()
			                                 * right->getPosition())
			                       / CelestialBodyRenderer::overridenScale
			                   + cam->relativePosition;
		}
		cam->relativePosition -= controllerRelPos - initControllerRelPos;
	}
	// double grip = scale
	if(leftGripPressedOrb && rightGripPressedOrb && left != nullptr
	   && right != nullptr)
	{
		rescale(initScaleOrb
		            * left->getPosition().distanceToPoint(right->getPosition())
		            / initControllersDistance,
		        scaleCenterOrb);
	}

	// apply keyboard controls
	for(unsigned int i(0); i < 3; ++i)
	{
		cam->relativePosition[i]
		    += frameTiming
		       * (cam->getView().inverted()
		          * (negativeVelocity + positiveVelocity))[i]
		       / CelestialBodyRenderer::overridenScale;
	}
}

void MovementControls::rescaleCube(double newScale, Vector3 const& scaleCenter)
{
	Vector3 scaleCenterPosInCube(scaleCenter - cubeTranslation);
	scaleCenterPosInCube *= newScale / cubeScale;
	cubeTranslation = scaleCenter - scaleCenterPosInCube;
	cubeScale       = newScale;
}

void MovementControls::rescale(double newScale, Vector3 const& scaleCenter)
{
	Vector3 diff(cam->relativePosition - scaleCenter);
	diff /= newScale / CelestialBodyRenderer::overridenScale;
	cam->relativePosition                 = scaleCenter + diff;
	CelestialBodyRenderer::overridenScale = newScale;
}
