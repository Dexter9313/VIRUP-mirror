#include "BasicCamera.hpp"

BasicCamera::BasicCamera(VRHandler const* vrHandler)
    : vrHandler(vrHandler)
    , eyeDistanceFactor(1.0f)
    , view()
    , proj()
    , fullTransform()
{
}

void BasicCamera::setView(QVector3D const& position,
                          QVector3D const& lookDirection, QVector3D const& up)
{
	view = QMatrix4x4();
	view.lookAt(position, position + lookDirection, up);
}

void BasicCamera::lookAt(QVector3D const& position, QVector3D const& center,
                         QVector3D const& up)
{
	view = QMatrix4x4();
	view.lookAt(position, center, up);
}

void BasicCamera::setPerspectiveProj(float fov, float aspectratio,
                                     float nearPlan, float farPlan)
{
	proj = QMatrix4x4();
	proj.perspective(fov, aspectratio, nearPlan, farPlan);
}

void BasicCamera::setEyeDistanceFactor(float eyeDistanceFactor)
{
	this->eyeDistanceFactor = eyeDistanceFactor;
	eyeDistanceCorrection(0, 0) = eyeDistanceFactor;
	eyeDistanceCorrection(1, 1) = eyeDistanceFactor;
	eyeDistanceCorrection(2, 2) = eyeDistanceFactor;
}

QVector4D BasicCamera::project(QVector3D const& vertex) const
{
	return project(QVector4D(vertex, 1.f));
}

QVector4D BasicCamera::project(QVector4D const& vertex) const
{
	return fullTransform * vertex;
}

void BasicCamera::update(bool force2D)
{
	if(*vrHandler && !force2D)
	{
		// proj
		projLeft = vrHandler->getProjectionMatrix(
		    Side::LEFT, 0.1f * eyeDistanceFactor, 10000.f * eyeDistanceFactor);
		projRight = vrHandler->getProjectionMatrix(
		    Side::RIGHT, 0.1f * eyeDistanceFactor, 10000.f * eyeDistanceFactor);

		projLeft = projLeft * eyeDist(vrHandler->getEyeViewMatrix(Side::LEFT),
		                              eyeDistanceFactor);
		projRight
		    = projRight * eyeDist(vrHandler->getEyeViewMatrix(Side::RIGHT),
		                          eyeDistanceFactor);

		QMatrix4x4* projEye
		    = (vrHandler->getCurrentRenderingEye() == Side::LEFT) ? &projLeft
		                                                          : &projRight;

		// prog * view
		QMatrix4x4 hmdMat(vrHandler->getHMDPosMatrix().inverted());

		fullTrackedSpaceTransform = *projEye * eyeDistanceCorrection * hmdMat;
		fullHmdSpaceTransform     = *projEye * eyeDistanceCorrection;

		hmdMat = eyeDist(hmdMat, eyeDistanceFactor);
		// not true ! it holds its inverse for now
		hmdScaledToWorld = hmdMat * view;
		fullTransform    = *projEye * hmdScaledToWorld;
		// now it's the right value :)
		hmdScaledToWorld = hmdScaledToWorld.inverted();

		fullCameraSpaceTransform = *projEye * hmdMat;
	}
	else
	{
		fullTransform             = proj * view;
		fullCameraSpaceTransform  = proj;
		fullTrackedSpaceTransform = proj * eyeDistanceCorrection;
		fullHmdSpaceTransform     = fullTrackedSpaceTransform;
	}

	// update clipping planes
	// Gribb, G., & Hartmann, K. (2001). Fast extraction of viewing frustum
	// planes from the world-view-projection matrix.
	// http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf
	clippingPlanes[LEFT_PLANE]  = fullTransform.row(3) + fullTransform.row(0);
	clippingPlanes[RIGHT_PLANE] = fullTransform.row(3) - fullTransform.row(0);

	clippingPlanes[BOTTOM_PLANE] = fullTransform.row(3) + fullTransform.row(1);
	clippingPlanes[TOP_PLANE]    = fullTransform.row(3) - fullTransform.row(1);

	clippingPlanes[NEAR_PLANE] = fullTransform.row(3) + fullTransform.row(2);
	clippingPlanes[FAR_PLANE]  = fullTransform.row(3) - fullTransform.row(2);
}

void BasicCamera::uploadMatrices() const
{
	GLHandler::setUpTransforms(fullTransform, fullCameraSpaceTransform,
	                           fullTrackedSpaceTransform,
	                           fullHmdSpaceTransform);
}

QMatrix4x4 BasicCamera::cameraSpaceToWorldTransform() const
{
	// see TRANSFORMS
	return view.inverted();
}

QMatrix4x4 BasicCamera::trackedSpaceToWorldTransform() const
{
	// see TRANSFORMS
	return fullTransform.inverted() * fullTrackedSpaceTransform;
}

QMatrix4x4 BasicCamera::hmdSpaceToWorldTransform() const
{
	// see TRANSFORMS
	return fullTransform.inverted() * fullHmdSpaceTransform;
}

QMatrix4x4 BasicCamera::hmdScaledSpaceToWorldTransform() const
{
	if(*vrHandler)
		return hmdScaledToWorld;
	else
		return view.inverted();
}

QMatrix4x4 BasicCamera::screenToWorldTransform() const
{
	return view.inverted() * proj.inverted();
}

QMatrix4x4 BasicCamera::hmdScreenToWorldTransform(Side side) const
{
	if(side == Side::LEFT)
		return hmdScaledToWorld * projLeft.inverted();
	else
		return hmdScaledToWorld * projRight.inverted();
}

QMatrix4x4 BasicCamera::eyeDist(QMatrix4x4 const& matrix,
                                float eyeDistanceFactor)
{
	QMatrix4x4 result(matrix);
	result.setColumn(
	    3, QVector4D(eyeDistanceFactor * QVector3D(matrix.column(3)), 1.0f));

	return result;
}
