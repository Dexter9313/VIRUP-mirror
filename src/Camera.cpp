#include "Camera.hpp"

Camera::Camera(VRHandler const* vrHandler)
    : BasicCamera(vrHandler)
    , angle(0.f)
    , distance(1.0f)
    , targetFPS(60.0f)
    , up(QVector3D(0.0f, 0.0f, 1.0f))
{
	update();
}

void Camera::update()
{
	position
	    = QVector3D(cos(angle) * 2.0f * distance, sin(angle) * 2.0f * distance,
	                1.2f * distance * (distance / 2));
	lookDirection = (-1.0f * getPosition()).normalized();
	position += QSettings().value("misc/focuspoint").value<QVector3D>();
	setView(position, lookDirection, up);
	if(*vrHandler)
		targetFPS = 90.f; // small margin to avoid frame drops
	else
		targetFPS = 60.f;
	BasicCamera::update();
}

// inspired by
// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-boxes/
bool Camera::shouldBeCulled(BBox const& bbox) const
{
	std::vector<QVector4D> projectedCorners;
	// get all projected corners and test if any of them is inside the frustum
	// if one of them is inside, we can stop there and return false
	for(unsigned int i(0); i < 8; ++i)
	{
		projectedCorners.push_back(project(getCorner(bbox, i)));
		projectedCorners[i] /= projectedCorners[i][3];
		if(inFrustum(QVector3D(projectedCorners[i])))
			return false;
	}
	// cull too much for now
	return true;

	// if all corners are outside the frustum we still have to check if all
	// corners are at the same side of one the frustum face (not exhaustive but
	// pretty good); if there is one such face, then the cube should be culled
	/*
	    if(projectedCorners[0].x < -1)
	        for(unsigned int i(1); i < 8; ++i)
	        {
	            if(i == 7 && projectedCorners[i].x < -1)
	                return true;
	            if(projectedCorners[i].x > -1)
	                break;
	        }
	    else if(projectedCorners[0].x > 1)
	        for(unsigned int i(1); i < 8; ++i)
	        {
	            if(i == 7 && projectedCorners[i].x > 1)
	                return true;
	            if(projectedCorners[i].x < 1)
	                break;
	        }
	    if(projectedCorners[0].y < -1)
	        for(unsigned int i(1); i < 8; ++i)
	        {
	            if(i == 7 && projectedCorners[i].y < -1)
	                return true;
	            if(projectedCorners[i].y > -1)
	                break;
	        }
	    else if(projectedCorners[0].y > 1)
	        for(unsigned int i(1); i < 8; ++i)
	        {
	            if(i == 7 && projectedCorners[i].y > 1)
	                return true;
	            if(projectedCorners[i].y < 1)
	                break;
	        }
	    if(projectedCorners[0].z < -1)
	        for(unsigned int i(1); i < 8; ++i)
	        {
	            if(i == 7 && projectedCorners[i].z < -1)
	                return true;
	            if(projectedCorners[i].z > -1)
	                break;
	        }
	    else if(projectedCorners[0].z > 1)
	        for(unsigned int i(1); i < 8; ++i)
	        {
	            if(i == 7 && projectedCorners[i].z > 1)
	                return true;
	            if(projectedCorners[i].z < 1)
	                break;
	        }
	*/
	// at this stage it is not evident if the cube should be culled or not, so
	// it shouldn't be
	// return false
}

QVector3D Camera::getCorner(BBox const& bBox, unsigned int i)
{
	switch(i)
	{
		case 0:
			return QVector3D(bBox.minx, bBox.miny, bBox.minz);
		case 1:
			return QVector3D(bBox.minx, bBox.miny, bBox.maxz);
		case 2:
			return QVector3D(bBox.minx, bBox.maxy, bBox.minz);
		case 3:
			return QVector3D(bBox.minx, bBox.maxy, bBox.maxz);
		case 4:
			return QVector3D(bBox.maxx, bBox.miny, bBox.minz);
		case 5:
			return QVector3D(bBox.maxx, bBox.miny, bBox.maxz);
		case 6:
			return QVector3D(bBox.maxx, bBox.maxy, bBox.minz);
		case 7:
			return QVector3D(bBox.maxx, bBox.maxy, bBox.maxz);
		default:
			return bBox.mid;
	}
}

bool Camera::inFrustum(QVector3D const& projected)
{
	return (projected.x() > -1 && projected.x() < 1 && projected.y() > -1
	        && projected.y() < 1 && projected.z() > -1 && projected.z() < 1);
}
