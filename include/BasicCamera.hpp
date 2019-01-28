#ifndef BASICCAMERA_H
#define BASICCAMERA_H

#include <QMatrix4x4>
#include <QVector3D>
#include <QVector4D>

#include "GLHandler.hpp"

class VRHandler;

class BasicCamera
{
  public:
	BasicCamera(VRHandler const* vrHandler);
	QMatrix4x4 getView() const { return view; };
	void setView(QMatrix4x4 const& view) { this->view = view; };
	void setView(QVector3D const& position, QVector3D const& lookDirection,
	             QVector3D const& up);
	void lookAt(QVector3D const& position, QVector3D const& center,
	            QVector3D const& up);
	QMatrix4x4 getProj() const { return proj; };
	void setProj(QMatrix4x4 const& proj) { this->proj = proj; };
	void setPerspectiveProj(float fov, float aspectratio, float nearPlan = 0.1f,
	                        float farPlan = 10000.f);
	float getEyeDistanceFactor() const { return eyeDistanceFactor; };
	void setEyeDistanceFactor(float EyeDistanceFactor);
	QVector4D project(QVector3D const& vertex) const;
	void update();
	void uploadMatrices() const;
	QMatrix4x4 cameraSpaceToWorldTransform() const;
	QMatrix4x4 trackedSpaceToWorldTransform() const;
	QMatrix4x4 hmdSpaceToWorldTransform() const;
	QMatrix4x4 screenToWorldTransform() const;
	QMatrix4x4 hmdScreenToWorldTransform(Side side) const;
	virtual ~BasicCamera(){};

  protected:
	VRHandler const* vrHandler;
	float eyeDistanceFactor;

	// See TRANSFORMS beyond here
	static QMatrix4x4 eyeDist(QMatrix4x4 const& matrix, float eyeDistanceFactor);

	QMatrix4x4 view;

	// 2D
	QMatrix4x4 proj;
	// VR : from hmdscaled to screen
	QMatrix4x4 projLeft;
	QMatrix4x4 projRight;
	// VR : from hmdscaled to world
	QMatrix4x4 hmdScaledToWorld;

	QMatrix4x4 eyeDistanceCorrection;

	// object to screen transforms
	// transform for any world object
	QMatrix4x4 fullTransform;
	// transform for any Camera space object (follows Camera)
	QMatrix4x4 fullCameraSpaceTransform;
	// transform for any Tracked space object
	QMatrix4x4 fullTrackedSpaceTransform;
	// transform for any HMD space object (follows HMD)
	QMatrix4x4 fullHmdSpaceTransform;
};

#include "vr/VRHandler.hpp"
#endif // BASICCAMERA_H
