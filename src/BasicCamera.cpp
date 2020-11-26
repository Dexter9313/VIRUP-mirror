#include "BasicCamera.hpp"

BasicCamera::BasicCamera(VRHandler const& vrHandler)
    : vrHandler(vrHandler)
    , eyeDistanceFactor(1.0f)
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
	this->eyeDistanceFactor     = eyeDistanceFactor;
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

void BasicCamera::update(QMatrix4x4 const& angleShiftMat)
{
	const QMatrix4x4 shiftedView(angleShiftMat * view);
	if(vrHandler.isEnabled())
	{
		// proj
		projLeft = vrHandler.getProjectionMatrix(
		    Side::LEFT, 0.1f * eyeDistanceFactor, 10000.f * eyeDistanceFactor);
		projRight = vrHandler.getProjectionMatrix(
		    Side::RIGHT, 0.1f * eyeDistanceFactor, 10000.f * eyeDistanceFactor);

		projLeft = projLeft
		           * eyeDist(vrHandler.getEyeViewMatrix(Side::LEFT),
		                     eyeDistanceFactor);
		projRight = projRight
		            * eyeDist(vrHandler.getEyeViewMatrix(Side::RIGHT),
		                      eyeDistanceFactor);

		Side currentRenderingEye(vrHandler.getCurrentRenderingEye());

		QMatrix4x4* projEye
		    = (currentRenderingEye == Side::LEFT) ? &projLeft : &projRight;

		// prog * shiftedView
		QMatrix4x4 hmdMat(vrHandler.getHMDPosMatrix().inverted());

		fullSeatedTrackedSpaceTransform
		    = *projEye * eyeDistanceCorrection * hmdMat;
		fullStandingTrackedSpaceTransform
		    = fullSeatedTrackedSpaceTransform
		      * vrHandler.getSeatedToStandingAbsoluteTrackingPos().inverted();
		fullHmdSpaceTransform = *projEye * eyeDistanceCorrection;

		if(!seatedVROrigin)
		{
			hmdMat = hmdMat
			         * vrHandler.getSeatedToStandingAbsoluteTrackingPos()
			               .inverted();
		}

		hmdMat = eyeDist(hmdMat, eyeDistanceFactor);
		// not true ! it holds its inverse for now
		hmdScaledToWorld = hmdMat * shiftedView;
		fullTransform    = *projEye * hmdScaledToWorld;
		// now it's the right value :)
		hmdScaledToWorld = hmdScaledToWorld.inverted();

		fullCameraSpaceTransform = *projEye * hmdMat;

		fullSkyboxSpaceTransform
		    = vrHandler.getProjectionMatrix(currentRenderingEye, 0.1f, 10000.f)
		      * noTrans(vrHandler.getEyeViewMatrix(currentRenderingEye))
		      * noTrans(vrHandler.getHMDPosMatrix().inverted())
		      * noTrans(shiftedView);

		updateClippingPlanes();

		return;
	}

	update2D(angleShiftMat);
}

void BasicCamera::update2D(QMatrix4x4 const& angleShiftMat)
{
	const QMatrix4x4 shiftedView(angleShiftMat * view);
	fullTransform                     = proj * shiftedView;
	fullCameraSpaceTransform          = proj;
	fullSeatedTrackedSpaceTransform   = proj * eyeDistanceCorrection;
	fullStandingTrackedSpaceTransform = fullSeatedTrackedSpaceTransform;
	fullHmdSpaceTransform             = fullSeatedTrackedSpaceTransform;
	fullSkyboxSpaceTransform          = proj * noTrans(shiftedView);

	updateClippingPlanes();
}

void BasicCamera::uploadMatrices() const
{
	GLHandler::setUpTransforms(fullTransform, fullCameraSpaceTransform,
	                           fullSeatedTrackedSpaceTransform,
	                           fullStandingTrackedSpaceTransform,
	                           fullHmdSpaceTransform, fullSkyboxSpaceTransform);
}

void BasicCamera::updateClippingPlanes()
{
	// update clipping planes
	// Gribb, G., & Hartmann, K. (2001). Fast extraction of viewing frustum
	// planes from the world-view-projection matrix.
	// http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf
	clippingPlanes[LEFT_PLANE]
	    = (fullTransform.row(3) + fullTransform.row(0)).normalized();
	clippingPlanes[RIGHT_PLANE]
	    = (fullTransform.row(3) - fullTransform.row(0)).normalized();

	clippingPlanes[BOTTOM_PLANE]
	    = (fullTransform.row(3) + fullTransform.row(1)).normalized();
	clippingPlanes[TOP_PLANE]
	    = (fullTransform.row(3) - fullTransform.row(1)).normalized();

	clippingPlanes[NEAR_PLANE]
	    = (fullTransform.row(3) + fullTransform.row(2)).normalized();
	clippingPlanes[FAR_PLANE]
	    = (fullTransform.row(3) - fullTransform.row(2)).normalized();
}

QVector3D BasicCamera::getWorldSpacePosition() const
{
	QMatrix4x4 eyeViewMatrix;
	if(vrHandler.isEnabled())
	{
		eyeViewMatrix
		    = vrHandler.getEyeViewMatrix(vrHandler.getCurrentRenderingEye());
	}

	return QVector3D((hmdScaledToWorld * eyeViewMatrix.inverted()).column(3));
}

QMatrix4x4 BasicCamera::cameraSpaceToWorldTransform() const
{
	// see TRANSFORMS
	return view.inverted();
}

QMatrix4x4 BasicCamera::seatedTrackedSpaceToWorldTransform() const
{
	// see TRANSFORMS
	return fullTransform.inverted() * fullSeatedTrackedSpaceTransform;
}

QMatrix4x4 BasicCamera::standingTrackedSpaceToWorldTransform() const
{
	// see TRANSFORMS
	return fullTransform.inverted() * fullStandingTrackedSpaceTransform;
}

QMatrix4x4 BasicCamera::hmdSpaceToWorldTransform() const
{
	// see TRANSFORMS
	return fullTransform.inverted() * fullHmdSpaceTransform;
}

QMatrix4x4 BasicCamera::hmdScaledSpaceToWorldTransform() const
{
	if(vrHandler.isEnabled())
	{
		return hmdScaledToWorld;
	}
	return view.inverted();
}

QMatrix4x4 BasicCamera::screenToWorldTransform() const
{
	return view.inverted() * proj.inverted();
}

float BasicCamera::pixelSolidAngle() const
{
	QMatrix4x4 p(vrHandler.isEnabled()
	                 ? vrHandler.getProjectionMatrix(
	                       Side::LEFT, 0.1f * eyeDistanceFactor,
	                       10000.f * eyeDistanceFactor)
	                 : proj);
	double radPerPix = atan(1.0f / p.column(1)[1]) * 2.0 / windowSize.height();
	// https://en.wikipedia.org/wiki/Solid_angle#Pyramid
	return 4.0 * asin(sin(radPerPix / 2.0) * sin(radPerPix / 2.0));
}

QMatrix4x4 BasicCamera::hmdScreenToWorldTransform(Side side) const
{
	if(side == Side::LEFT)
	{
		return hmdScaledToWorld * projLeft.inverted();
	}
	return hmdScaledToWorld * projRight.inverted();
}

QMatrix4x4 BasicCamera::noTrans(QMatrix4x4 const& matrix)
{
	QMatrix4x4 result(matrix);
	result.setColumn(3, QVector4D(0.f, 0.f, 0.f, 1.f));

	return result;
}

QMatrix4x4 BasicCamera::eyeDist(QMatrix4x4 const& matrix,
                                float eyeDistanceFactor)
{
	QMatrix4x4 result(matrix);
	result.setColumn(
	    3, QVector4D(eyeDistanceFactor * QVector3D(matrix.column(3)), 1.0f));

	return result;
}
