#ifndef BASICCAMERA_H
#define BASICCAMERA_H

#include <QApplication>
#include <QDesktopWidget>
#include <QMatrix4x4>
#include <QVector3D>
#include <QVector4D>
#include <array>

#include "GLHandler.hpp"

class VRHandler;

class BasicCamera : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QMatrix4x4 viewmatrix READ getView WRITE setView)
	Q_PROPERTY(QMatrix4x4 projectionmatrix READ getProj WRITE setProj)
	Q_PROPERTY(float eyedistancefactor READ getEyeDistanceFactor WRITE
	               setEyeDistanceFactor)
	Q_PROPERTY(
	    QMatrix4x4 cameraspacetoworldtransform READ cameraSpaceToWorldTransform)
	Q_PROPERTY(QMatrix4x4 trackedspacetoworldtransform READ
	               trackedSpaceToWorldTransform)
	Q_PROPERTY(
	    QMatrix4x4 hmdspacetoworldtransform READ hmdSpaceToWorldTransform)
	Q_PROPERTY(QMatrix4x4 hmdscaledspacetoworldtransform READ
	               hmdScaledSpaceToWorldTransform)
	Q_PROPERTY(QMatrix4x4 screentoworldtransform READ screenToWorldTransform)

  protected: // protected typedefs
	enum ClippingPlane
	{
		LEFT_PLANE   = 0,
		RIGHT_PLANE  = 1,
		BOTTOM_PLANE = 2,
		TOP_PLANE    = 3,
		NEAR_PLANE   = 4,
		FAR_PLANE    = 5,
	};
	typedef QVector4D Plane; // Ax+By+Cz+D=0 => [A,B,C,D]
  public:
	BasicCamera(VRHandler const* vrHandler);
	QMatrix4x4 getView() const { return view; };
	void setView(QMatrix4x4 const& view) { this->view = view; };
	QMatrix4x4 getProj() const { return proj; };
	void setProj(QMatrix4x4 const& proj) { this->proj = proj; };
	float getEyeDistanceFactor() const { return eyeDistanceFactor; };
	void setEyeDistanceFactor(float eyeDistanceFactor);
	QMatrix4x4 cameraSpaceToWorldTransform() const;
	QMatrix4x4 trackedSpaceToWorldTransform() const;
	QMatrix4x4 hmdSpaceToWorldTransform() const;
	QMatrix4x4 hmdScaledSpaceToWorldTransform() const;
	QMatrix4x4 screenToWorldTransform() const;
	virtual ~BasicCamera(){};

  public slots:
	void setView(QVector3D const& position, QVector3D const& lookDirection,
	             QVector3D const& up);
	void lookAt(QVector3D const& position, QVector3D const& center,
	            QVector3D const& up);
	void setPerspectiveProj(float fov, float aspectratio, float nearPlan = 0.1f,
	                        float farPlan = 10000.f);
	QMatrix4x4 hmdScreenToWorldTransform(Side side) const;
	QVector4D project(QVector3D const& vertex) const;
	QVector4D project(QVector4D const& vertex) const;

  public:
	virtual void update();
	virtual void update2D();
	void uploadMatrices() const;

  protected:
	VRHandler const* vrHandler;
	float eyeDistanceFactor;

	void updateClippingPlanes();

	// See TRANSFORMS beyond here
	static QMatrix4x4 eyeDist(QMatrix4x4 const& matrix,
	                          float eyeDistanceFactor);

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

	// For culling. Normals point Inside i.e. clippingPlanes[i]*v >= 0 <=>
	// v is at the inner side of clippingPlanes[i]
	std::array<Plane, 6> clippingPlanes;
};

#include "vr/VRHandler.hpp"
#endif // BASICCAMERA_H
