#include "Camera.hpp"

Camera::Camera(VRHandler const* vrHandler)
    : BasicCamera(vrHandler)
{
	setView(QMatrix4x4());
}

void Camera::update()
{
	updateTargetFPS();
	BasicCamera::update();
}

void Camera::update2D()
{
	updateTargetFPS();
	BasicCamera::update2D();
}

void Camera::updateTargetFPS()
{
	if(*vrHandler)
	{
		targetFPS = 100.f; // small margin to avoid frame drops
	}
	else
	{
		targetFPS = 60.f;
	}
}

// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-boxes/
// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-boxes-ii/
bool Camera::shouldBeCulled(BBox const& bbox, QMatrix4x4 const& model,
                            bool depthClamp) const
{
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
