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
                                   Camera* cosmoCam,
                                   OrbitalSystemCamera* planetCam)
    : vrHandler(vrHandler)
    , dataBBox(dataBBox)
    , cosmoCam(cosmoCam)
    , planetCam(planetCam)
{
	if((dataBBox.maxx - dataBBox.minx >= dataBBox.maxy - dataBBox.miny)
	   && (dataBBox.maxx - dataBBox.minx >= dataBBox.maxz - dataBBox.minz))
	{
		cosmoCam->scale = 1.f / (dataBBox.maxx - dataBBox.minx);
	}
	else if(dataBBox.maxy - dataBBox.miny >= dataBBox.maxz - dataBBox.minz)
	{
		cosmoCam->scale = 1.f / (dataBBox.maxy - dataBBox.miny);
	}
	else
	{
		cosmoCam->scale = 1.f / (dataBBox.maxz - dataBBox.minz);
	}
	cosmoCam->position[0] = dataBBox.mid.x();
	cosmoCam->position[1] = dataBBox.mid.y();
	cosmoCam->position[2] = dataBBox.mid.z();
}

void MovementControls::keyPressEvent(QKeyEvent* e)
{
	if(e->key() == Qt::Key_C)
	{
		Vector3 unitRelPos(planetCam->relativePosition.getUnitForm());
		planetCam->yaw   = atan2(unitRelPos[1], unitRelPos[0]);
		planetCam->pitch = -1.0 * asin(unitRelPos[2]);
	}
	else if(e->key() == Qt::Key_W || e->key() == Qt::Key_Up)
	{
		negVel.setZ(-1);
	}
	else if(e->key() == Qt::Key_A || e->key() == Qt::Key_Left)
	{
		negVel.setX(-1);
	}
	else if(e->key() == Qt::Key_S || e->key() == Qt::Key_Down)
	{
		posVel.setZ(1);
	}
	else if(e->key() == Qt::Key_D || e->key() == Qt::Key_Right)
	{
		posVel.setX(1);
	}
}

void MovementControls::keyReleaseEvent(QKeyEvent* e)
{
	if(e->key() == Qt::Key_W || e->key() == Qt::Key_Up)
	{
		negVel.setZ(0);
	}
	else if(e->key() == Qt::Key_A || e->key() == Qt::Key_Left)
	{
		negVel.setX(0);
	}
	else if(e->key() == Qt::Key_S || e->key() == Qt::Key_Down)
	{
		posVel.setZ(0);
	}
	else if(e->key() == Qt::Key_D || e->key() == Qt::Key_Right)
	{
		posVel.setX(0);
	}
}

void MovementControls::wheelEvent(QWheelEvent* e)
{
	cosmoCam->scale *= (1.f + e->angleDelta().y() / 1000.f);
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
						leftGripPressedCube     = true;
						initControllerPosInCube = cosmoCam->worldToDataPosition(
						    Utils::fromQt(trackedSpaceToWorldTransform
						                  * left->getPosition()));
					}
					else if(e.side == Side::RIGHT && right != nullptr)
					{
						rightGripPressedCube    = true;
						initControllerPosInCube = cosmoCam->worldToDataPosition(
						    Utils::fromQt(trackedSpaceToWorldTransform
						                  * right->getPosition()));
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
						initScaleCube = cosmoCam->scale;

						QVector3D controllersMidPoint(
						    (left->getPosition() + right->getPosition()) / 2.0);

						controllersMidPoint = trackedSpaceToWorldTransform
						                      * controllersMidPoint;

						scaleCenterCube = cosmoCam->worldToDataPosition(
						    Utils::fromQt(controllersMidPoint));
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
							    = cosmoCam->worldToDataPosition(
							        Utils::fromQt(trackedSpaceToWorldTransform
							                      * right->getPosition()));
						}
					}
					else if(e.side == Side::RIGHT)
					{
						rightGripPressedCube = false;
						if(left != nullptr && leftGripPressedCube)
						{
							initControllerPosInCube
							    = cosmoCam->worldToDataPosition(
							        Utils::fromQt(trackedSpaceToWorldTransform
							                      * left->getPosition()));
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
	    planetCam->trackedSpaceToWorldTransform());
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
						      + planetCam->relativePosition;
					}
					else if(e.side == Side::RIGHT && right != nullptr)
					{
						rightGripPressedOrb = true;
						initControllerRelPos
						    = Utils::fromQt(trackedSpaceToWorldTransform
						                    * right->getPosition())
						          / CelestialBodyRenderer::overridenScale
						      + planetCam->relativePosition;
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
						      + planetCam->relativePosition;
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
							      + planetCam->relativePosition;
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
							      + planetCam->relativePosition;
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

void MovementControls::update(double frameTiming)
{
	updateCube(frameTiming);
	if(OctreeLOD::renderPlanetarySystem)
	{
		updateOrbitalSystem(frameTiming);
	}
}

void MovementControls::updateCube(double frameTiming)
{
	Controller const* left(vrHandler.getController(Side::LEFT));
	Controller const* right(vrHandler.getController(Side::RIGHT));

	// single grip = translation
	if(leftGripPressedCube != rightGripPressedCube)
	{
		Vector3 controllerPosInCube = {};
		if(leftGripPressedCube && left != nullptr)
		{
			controllerPosInCube = cosmoCam->worldToDataPosition(
			    Utils::fromQt(cosmoCam->trackedSpaceToWorldTransform()
			                  * left->getPosition()));
		}
		else if(rightGripPressedCube && right != nullptr)
		{
			controllerPosInCube = cosmoCam->worldToDataPosition(
			    Utils::fromQt(cosmoCam->trackedSpaceToWorldTransform()
			                  * right->getPosition()));
		}
		cosmoCam->position += initControllerPosInCube - controllerPosInCube;
	}
	// double grip = scale
	if(leftGripPressedCube && rightGripPressedCube && left != nullptr
	   && right != nullptr)
	{
		rescale(initScaleCube
		            * left->getPosition().distanceToPoint(right->getPosition())
		            / initControllersDistance,
		        scaleCenterCube, cosmoCam->position, cosmoCam->scale);
	}

	// apply keyboard controls
	if(!vrHandler)
	{
		cosmoCam->position += frameTiming
		                      * Utils::fromQt(cosmoCam->getView().inverted()
		                                      * (posVel + negVel))
		                      / cosmoCam->scale;
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
			controllerRelPos
			    = Utils::fromQt(planetCam->trackedSpaceToWorldTransform()
			                    * left->getPosition())
			          / CelestialBodyRenderer::overridenScale
			      + planetCam->relativePosition;
		}
		else if(rightGripPressedOrb && right != nullptr)
		{
			controllerRelPos
			    = Utils::fromQt(planetCam->trackedSpaceToWorldTransform()
			                    * right->getPosition())
			          / CelestialBodyRenderer::overridenScale
			      + planetCam->relativePosition;
		}
		planetCam->relativePosition -= controllerRelPos - initControllerRelPos;
	}
	// double grip = scale
	if(leftGripPressedOrb && rightGripPressedOrb && left != nullptr
	   && right != nullptr)
	{
		rescale(initScaleOrb
		            * left->getPosition().distanceToPoint(right->getPosition())
		            / initControllersDistance,
		        scaleCenterOrb, planetCam->relativePosition,
		        CelestialBodyRenderer::overridenScale);
	}

	// apply keyboard controls
	for(unsigned int i(0); i < 3; ++i)
	{
		planetCam->relativePosition[i]
		    += frameTiming
		       * (planetCam->getView().inverted() * (negVel + posVel))[i]
		       / CelestialBodyRenderer::overridenScale;
	}
}

void MovementControls::rescale(double newScale, Vector3 const& scaleCenter,
                               Vector3& position, double& scale)
{
	Vector3 diff(position - scaleCenter);
	diff *= scale / newScale;
	position = scaleCenter + diff;
	scale    = newScale;
}
