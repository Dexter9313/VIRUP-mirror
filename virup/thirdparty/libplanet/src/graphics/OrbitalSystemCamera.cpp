#include "graphics/OrbitalSystemCamera.hpp"
#include "graphics/Utils.hpp"

OrbitalSystemCamera::OrbitalSystemCamera(VRHandler const* vrHandler)
    : BasicCamera(vrHandler)
{
	lookAt(QVector3D(0.f, 0.f, 0.f), QVector3D(1.f, 0.f, 0.f),
	       QVector3D(0.f, 0.f, 1.f));
}

Vector3 OrbitalSystemCamera::getRelativePositionTo(CelestialBody const* body,
                                                   UniversalTime uT) const
{
	Vector3 targetRelPosToBody(
	    CelestialBody::getRelativePositionAtUt(target, body, uT));

	return targetRelPosToBody - relativePosition;
}

void OrbitalSystemCamera::updateUT(UniversalTime /*uT*/)
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
	/*angleAboveXY = angleAboveXY > 3.1415f / 2.f ? 3.1415f / 2.f :
	angleAboveXY; angleAboveXY = angleAboveXY < -3.1415f / 2.f ? -3.1415f / 2.f
	: angleAboveXY; relativePosition.setXYZ( distance +
	target->getCelestialBodyParameters().radius, 0, 0);
	relativePosition.rotateAlongY(angleAboveXY);
	relativePosition.rotateAlongZ(angleAroundZ);*/

	lookDirection
	    = {-cosf(yaw) * cosf(pitch), -sinf(yaw) * cosf(pitch), sinf(pitch)};
	lookAt(QVector3D(0.f, 0.f, 0.f), lookDirection, up);
}

// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-points-and-spheres/
bool OrbitalSystemCamera::shouldBeCulled(QVector3D const& spherePosition,
                                         float radius) const
{
	for(unsigned int i(0); i < 6; ++i)
	{
		if(QVector4D::dotProduct(clippingPlanes[i],
		                         QVector4D(spherePosition, 1.f))
		   < -radius)
		{
			return true;
		}
	}
	return false;
}
