#include "CameraTreeLOD.hpp"

CameraTreeLOD::CameraTreeLOD(VRHandler const* vrHandler)
    : BasicCamera(vrHandler)
{
}

void CameraTreeLOD::update()
{
	updateView();
	BasicCamera::update();
}

void CameraTreeLOD::update2D()
{
	updateView();
	BasicCamera::update2D();
}

void CameraTreeLOD::updateView()
{
	Controller const* left(vrHandler->getController(Side::LEFT));
	Controller const* right(vrHandler->getController(Side::RIGHT));

	// single grip = translation
	if(leftGripPressed != rightGripPressed)
	{
		QVector3D controllerPosInCube;
		if(leftGripPressed && left != nullptr)
		{
			controllerPosInCube
			    = trackedSpaceToWorldTransform() * left->getPosition();
		}
		else if(rightGripPressed && right != nullptr)
		{
			controllerPosInCube
			    = trackedSpaceToWorldTransform() * right->getPosition();
		}
		position -= controllerPosInCube - initControllerPosInCube;
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

	setView(position, lookDirection, up);
	view.scale(scale);
	if(*vrHandler)
	{
		targetFPS = 90.f; // small margin to avoid frame drops
	}
	else
	{
		targetFPS = 60.f;
	}
}

// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-boxes/
// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-boxes-ii/
bool CameraTreeLOD::shouldBeCulled(BBox const& bbox, QMatrix4x4 const& model,
                                   bool depthClamp) const
{
	/* OLD WAY
	std::vector<QVector4D> projectedCorners;
	// get all projected corners and test if any of them is inside the frustum
	// if one of them is inside, we can stop there and return false
	for(unsigned int i(0); i < 8; ++i)
	{
	    projectedCorners.push_back(project(model*getCorner(bbox, i)));
	    projectedCorners[i] /= projectedCorners[i][3];
	    if(inFrustum(QVector3D(projectedCorners[i])))
	        return false;
	}
	// cull too much for now
	return true;
	END OLD WAY */

	/* NAIVE WAY (geometric approach testing boxes I)
	bool result = false;
	unsigned int out, in;

	// for each plane do ...
	for(unsigned int i = 0; i < 6; i++)
	{
	    // reset counters for corners in and out
	    out = 0;
	    in  = 0;
	    // for each corner of the box do ...
	    // get out of the cycle as soon as a box as corners
	    // both inside and out of the frustum
	    for(unsigned int k = 0; k < 8 && (in == 0 || out == 0); k++)
	    {
	        // is the corner outside or inside
	        if(QVector4D::dotProduct(clippingPlanes[i],
	                                 model * getCorner(bbox, k))
	           < 0)
	            out++;
	        else
	            in++;
	    }
	    // if all corners are out
	    if(!in)
	        return true;
	}
	return result;
	END NAIVE WAY */

	// OPTIMIZED WAY (using p-vertex)
	bool result = false;
	// for each plane do ...
	for(unsigned int i(0); i < 6; ++i)
	{
		if(depthClamp && (i == NEAR_PLANE || i == FAR_PLANE))
		{
			continue;
		}
		QVector4D pVertex = QVector4D(bbox.minx, bbox.miny, bbox.minz, 1.f);
		if(clippingPlanes.at(i).x() >= 0)
		{
			pVertex.setX(bbox.maxx);
		}
		if(clippingPlanes.at(i).y() >= 0)
		{
			pVertex.setY(bbox.maxy);
		}
		if(clippingPlanes.at(i).z() >= 0)
		{
			pVertex.setZ(bbox.maxz);
		}
		// p-vertex is conserved by model, because there are no rotations
		pVertex = model * pVertex;
		// is the p-vertex outside?
		if(QVector4D::dotProduct(clippingPlanes.at(i), pVertex) < 0)
		{
			return true;
		}
	}
	return result;
}

/*
void MainWin::mousePressEvent(QMouseEvent* e)
{
    if(e->button() != Qt::MouseButton::RightButton)
    {
        return;
    }

    lastCursorPos = QCursor::pos();
    QCursor::setPos(width() / 2, height() / 2);
    QCursor c(cursor());
    c.setShape(Qt::CursorShape::BlankCursor);
    setCursor(c);
    trackballEnabled = true;
}

void MainWin::mouseReleaseEvent(QMouseEvent* e)
{
    if(e->button() != Qt::MouseButton::RightButton)
    {
        return;
    }

    trackballEnabled = false;
    QCursor c(cursor());
    c.setShape(Qt::CursorShape::ArrowCursor);
    setCursor(c);
    QCursor::setPos(lastCursorPos);
}

void MainWin::mouseMoveEvent(QMouseEvent* e)
{
    if(!trackballEnabled)
    {
        return;
    }
    auto cam(dynamic_cast<Camera*>(&getCamera()));
    float dx = (static_cast<float>(width()) / 2 - e->globalX()) / width();
    float dy = (static_cast<float>(height()) / 2 - e->globalY()) / height();
    cam->angleAroundZ += dx * 3.14f / 3.f;
    cam->angleAboveXY += dy * 3.14f / 3.f;
    QCursor::setPos(width() / 2, height() / 2);
}

void MainWin::wheelEvent(QWheelEvent* e)
{
    cubeScale += cubeScale * e->angleDelta().y() / 1000.f;
    AbstractMainWin::wheelEvent(e);
}
*/
void CameraTreeLOD::vrEvent(VRHandler::Event const& e)
{
	switch(e.type)
	{
		case VRHandler::EventType::BUTTON_PRESSED:
			switch(e.button)
			{
				case VRHandler::Button::GRIP:
				{
					Controller const* left(
					    vrHandler->getController(Side::LEFT));
					Controller const* right(
					    vrHandler->getController(Side::RIGHT));
					if(e.side == Side::LEFT && left != nullptr)
					{
						leftGripPressed         = true;
						initControllerPosInCube = trackedSpaceToWorldTransform()
						                          * left->getPosition();
					}
					else if(e.side == Side::RIGHT && right != nullptr)
					{
						rightGripPressed        = true;
						initControllerPosInCube = trackedSpaceToWorldTransform()
						                          * right->getPosition();
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
						initScale = scale;
						QVector3D controllersMidPoint
						    = left->getPosition() + right->getPosition();
						controllersMidPoint /= 2.f;
						controllersMidPoint = trackedSpaceToWorldTransform()
						                      * controllersMidPoint;
						scaleCenter = controllersMidPoint;

						if(controllersMidPoint.x() < -1
						   || controllersMidPoint.x() > 1
						   || controllersMidPoint.y() < -1
						   || controllersMidPoint.y() > 1
						   || controllersMidPoint.z() < -1
						   || controllersMidPoint.z() > 1)
						{
							scaleCenter = QVector3D();
						}
					}
					break;
				}
			}
			break;
		case VRHandler::EventType::BUTTON_UNPRESSED:
			switch(e.button)
			{
				case VRHandler::Button::GRIP:
				{
					Controller const* left(
					    vrHandler->getController(Side::LEFT));
					Controller const* right(
					    vrHandler->getController(Side::RIGHT));
					if(e.side == Side::LEFT)
					{
						leftGripPressed = false;
						if(right != nullptr && rightGripPressed)
						{
							initControllerPosInCube
							    = trackedSpaceToWorldTransform()
							      * right->getPosition();
						}
					}
					else if(e.side == Side::RIGHT)
					{
						rightGripPressed = false;
						if(left != nullptr && leftGripPressed)
						{
							initControllerPosInCube
							    = trackedSpaceToWorldTransform()
							      * left->getPosition();
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

void CameraTreeLOD::rescaleCube(float newScale, QVector3D const& scaleCenter)
{
	QVector3D scaleCenterPosInCube = scaleCenter + position;
	scaleCenterPosInCube /= newScale / scale;
	position = scaleCenterPosInCube - scaleCenter;
	scale    = newScale;
}
