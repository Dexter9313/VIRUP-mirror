#include "Camera.hpp"

Camera::Camera(VRHandler const* vrHandler)
    : BasicCamera(vrHandler)
{
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

Vector3 Camera::dataToWorldPosition(Vector3 const& data) const
{
	Vector3 result(data);
	result -= position;
	result *= scale;
	return result;
}

QMatrix4x4 Camera::dataToWorldTransform() const
{
	QMatrix4x4 result;
	result.scale(scale);
	result.translate(Utils::toQt(-1.0 * position));
	return result;
}

Vector3 Camera::worldToDataPosition(Vector3 const& world) const
{
	Vector3 result(world);
	result /= scale;
	result += position;
	return result;
}

QMatrix4x4 Camera::worldToDataTransform() const
{
	QMatrix4x4 result;
	result.translate(Utils::toQt(position));
	result.scale(1.0 / scale);
	return result;
}

void Camera::updateTargetFPS()
{
	while(yaw < 0.f)
	{
		yaw += 2.f * M_PI;
	}
	while(yaw >= 2.f * M_PI)
	{
		yaw -= 2.f * M_PI;
	}
	if(pitch > M_PI_2 - 0.01)
	{
		pitch = M_PI_2 - 0.01;
	}
	if(pitch < -1.f * M_PI_2 + 0.01)
	{
		pitch = -1.f * M_PI_2 + 0.01;
	}
	setView(QVector3D(0.f, 0.f, 0.f), getLookDirection(),
	        QVector3D(0.f, 0.f, 1.f));

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

QVector3D Camera::getLookDirection() const
{
	return {-cosf(yaw) * cosf(pitch), -sinf(yaw) * cosf(pitch), sinf(pitch)};
}
