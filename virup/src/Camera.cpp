#include "Camera.hpp"

Camera::Camera(VRHandler const* vrHandler)
    : BasicCamera(vrHandler)
    , angleAroundZ(0.f)
    , angleAboveXY(0.f)
    , distance(1.0f)
    , targetFPS(60.0f)
    , up(QVector3D(0.0f, 0.0f, 1.0f))
{
	update();
}

void Camera::update()
{
	updateView();
	BasicCamera::update();
}

void Camera::update2D()
{
	updateView();
	BasicCamera::update2D();
}

void Camera::updateView()
{
	angleAboveXY = angleAboveXY > 3.1415f / 2.f ? 3.1415f / 2.f : angleAboveXY;
	angleAboveXY
	    = angleAboveXY < -3.1415f / 2.f ? -3.1415f / 2.f : angleAboveXY;
	position
	    = QVector3D(2.0f * distance * cos(angleAroundZ) * cos(angleAboveXY),
	                2.0f * distance * sin(angleAroundZ) * cos(angleAboveXY),
	                2.0f * distance * sin(angleAboveXY));
	lookDirection = (-1.0f * getPosition()).normalized();
	position += QSettings().value("misc/focuspoint").value<QVector3D>();
	setView(position, lookDirection, up);
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
bool Camera::shouldBeCulled(BBox const& bbox, QMatrix4x4 const& model) const
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
	int result = false;
	// for each plane do ...
	for(int i = 0; i < 6; i++)
	{
		QVector4D pVertex = QVector4D(bbox.minx, bbox.miny, bbox.minz, 1.f);
		if(clippingPlanes[i].x() >= 0)
		{
			pVertex.setX(bbox.maxx);
		}
		if(clippingPlanes[i].y() >= 0)
		{
			pVertex.setY(bbox.maxy);
		}
		if(clippingPlanes[i].z() >= 0)
		{
			pVertex.setZ(bbox.maxz);
		}
		// p-vertex is conserved by model, because there are no rotations
		pVertex = model * pVertex;
		// is the p-vertex outside?
		if(QVector4D::dotProduct(clippingPlanes[i], pVertex) < 0)
		{
			return true;
		}
	}
	return (result);
}

/* Not useful anymore
QVector4D Camera::getCorner(BBox const& bBox, unsigned int i)
{
    switch(i)
    {
        case 0:
            return QVector4D(bBox.minx, bBox.miny, bBox.minz, 1.f);
        case 1:
            return QVector4D(bBox.minx, bBox.miny, bBox.maxz, 1.f);
        case 2:
            return QVector4D(bBox.minx, bBox.maxy, bBox.minz, 1.f);
        case 3:
            return QVector4D(bBox.minx, bBox.maxy, bBox.maxz, 1.f);
        case 4:
            return QVector4D(bBox.maxx, bBox.miny, bBox.minz, 1.f);
        case 5:
            return QVector4D(bBox.maxx, bBox.miny, bBox.maxz, 1.f);
        case 6:
            return QVector4D(bBox.maxx, bBox.maxy, bBox.minz, 1.f);
        case 7:
            return QVector4D(bBox.maxx, bBox.maxy, bBox.maxz, 1.f);
        default:
            return QVector4D(bBox.mid, 1.f);
    }
}

bool Camera::inFrustum(QVector3D const& projected)
{
    return (projected.x() > -1 && projected.x() < 1 && projected.y() > -1
            && projected.y() < 1 && projected.z() > -1 && projected.z() < 1);
}
*/
