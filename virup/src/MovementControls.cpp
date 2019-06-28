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

MovementControls::MovementControls(VRHandler const& vrHandler, BBox dataBBox)
    : vrHandler(vrHandler)
    , dataBBox(dataBBox)
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

void MovementControls::keyReleaseEvent(QKeyEvent* e)
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

void MovementControls::wheelEvent(QWheelEvent* e)
{
	rescaleCube(cubeScale * (1.f + e->angleDelta().y() / 1000.f));
}

void MovementControls::vrEvent(VRHandler::Event const& e,
                               QMatrix4x4 trackedSpaceToWorldTransform)
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
						leftGripPressed = true;
						for(unsigned int i(0); i < 3; ++i)
						{
							initControllerPosInCube.at(i)
							    = (trackedSpaceToWorldTransform
							       * left->getPosition())[i]
							      - cubeTranslation.at(i);
						}
					}
					else if(e.side == Side::RIGHT && right != nullptr)
					{
						rightGripPressed = true;
						for(unsigned int i(0); i < 3; ++i)
						{
							initControllerPosInCube.at(i)
							    = (trackedSpaceToWorldTransform
							       * right->getPosition())[i]
							      - cubeTranslation.at(i);
						}
					}
					else
					{
						break;
					}
					if(leftGripPressed && rightGripPressed && left != nullptr
					   && right != nullptr)
					{
						initControllersDistance
						    = left->getPosition().distanceToPoint(
						        right->getPosition());
						initScale                                 = cubeScale;
						std::array<double, 3> controllersMidPoint = {};
						for(unsigned int i(0); i < 3; ++i)
						{
							controllersMidPoint.at(i)
							    = left->getPosition()[i]
							      + right->getPosition()[i];
							controllersMidPoint.at(i) /= 2.f;
						}
						QVector3D controllersMidPointFloat(
						    controllersMidPoint[0], controllersMidPoint[1],
						    controllersMidPoint[2]);
						for(unsigned int i(0); i < 3; ++i)
						{
							controllersMidPoint.at(i)
							    = (trackedSpaceToWorldTransform
							       * controllersMidPointFloat)[i];
							scaleCenter.at(i) = controllersMidPoint.at(i);
						}

						std::array<double, 3> controllersMidPointInCube = {};

						for(unsigned int i(0); i < 3; ++i)
						{
							controllersMidPointInCube.at(i)
							    = (controllersMidPoint.at(i)
							       - cubeTranslation.at(i))
							      / cubeScale;
						}

						if(controllersMidPointInCube[0] < dataBBox.minx
						   || controllersMidPointInCube[0] > dataBBox.maxx
						   || controllersMidPointInCube[1] < dataBBox.miny
						   || controllersMidPointInCube[1] > dataBBox.maxy
						   || controllersMidPointInCube[2] < dataBBox.minz
						   || controllersMidPointInCube[2] > dataBBox.maxz)
						{
							for(unsigned int i(0); i < 3; ++i)
							{
								scaleCenter.at(i) = cubeTranslation.at(i);
							}
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
						leftGripPressed = false;
						if(right != nullptr && rightGripPressed)
						{
							for(unsigned int i(0); i < 3; ++i)
							{
								initControllerPosInCube.at(i)
								    = (trackedSpaceToWorldTransform
								       * right->getPosition())[i]
								      - cubeTranslation.at(i);
							}
						}
					}
					else if(e.side == Side::RIGHT)
					{
						rightGripPressed = false;
						if(left != nullptr && leftGripPressed)
						{
							for(unsigned int i(0); i < 3; ++i)
							{
								initControllerPosInCube.at(i)
								    = (trackedSpaceToWorldTransform
								       * left->getPosition())[i]
								      - cubeTranslation.at(i);
							}
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
	Controller const* left(vrHandler.getController(Side::LEFT));
	Controller const* right(vrHandler.getController(Side::RIGHT));

	// single grip = translation
	if(leftGripPressed != rightGripPressed)
	{
		std::array<double, 3> controllerPosInCube = {};
		if(leftGripPressed && left != nullptr)
		{
			for(unsigned int i(0); i < 3; ++i)
			{
				controllerPosInCube.at(i)
				    = (camera.trackedSpaceToWorldTransform()
				       * left->getPosition())[i]
				      - cubeTranslation.at(i);
			}
		}
		else if(rightGripPressed && right != nullptr)
		{
			for(unsigned int i(0); i < 3; ++i)
			{
				controllerPosInCube.at(i)
				    = (camera.trackedSpaceToWorldTransform()
				       * right->getPosition())[i]
				      - cubeTranslation.at(i);
			}
		}
		for(unsigned int i(0); i < 3; ++i)
		{
			cubeTranslation.at(i)
			    += controllerPosInCube.at(i) - initControllerPosInCube.at(i);
		}
	}
	// double grip = scale
	if(leftGripPressed && rightGripPressed && left != nullptr
	   && right != nullptr)
	{
		rescaleCube(
		    initScale
		        * left->getPosition().distanceToPoint(right->getPosition())
		        / initControllersDistance,
		    scaleCenter);
	}

	// apply keyboard controls
	if(!vrHandler)
	{
		for(unsigned int i(0); i < 3; ++i)
		{
			cubeTranslation.at(i)
			    += frameTiming
			       * (camera.getView().inverted()
			          * (cubePositiveVelocity + cubeNegativeVelocity))[i];
		}
	}
}

void MovementControls::rescaleCube(double newScale,
                                   std::array<double, 3> const& scaleCenter)
{
	std::array<double, 3> scaleCenterPosInCube = {};
	for(unsigned int i(0); i < 3; ++i)
	{
		scaleCenterPosInCube.at(i) = scaleCenter.at(i) - cubeTranslation.at(i);
		scaleCenterPosInCube.at(i) *= newScale / cubeScale;
		cubeTranslation.at(i) = scaleCenter.at(i) - scaleCenterPosInCube.at(i);
	}
	cubeScale = newScale;
}
