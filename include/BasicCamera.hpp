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

/**
 * @brief Represents a simple camera that can be used by the engine.
 *
 * This camera will handle both 2D and VR transformations and will switch from
 * one mode to the other automatically is VR is enabled or disabled.
 *
 * You can implement your own camera by inheriting from this class and use
 * AbstractMainWin#setCamera.
 *
 * To have a better overview of all the transforms performed by the camera,
 * please read the TRANSFORMS file.
 */
class BasicCamera : public QObject
{
	Q_OBJECT
	/**
	 * @brief Transformation matrix from world space to camera space.
	 *
	 * @accessors getView(), setView()
	 */
	Q_PROPERTY(QMatrix4x4 viewmatrix READ getView WRITE setView)
	/**
	 * @brief Transformation matrix from camera space to clip space.
	 *
	 * Will only get used for 2D projection. If VR is enabled, @ref vrHandler's
	 * projection matrices will be used.
	 *
	 * @accessors getProj(), setProj()
	 */
	Q_PROPERTY(QMatrix4x4 projectionmatrix READ getProj WRITE setProj)
	/**
	 * @brief Factor to change the eye separation distance in VR.
	 *
	 * Eye separation distance is expressed in meters and is roughly equal to
	 * 0.07 meters (computed more precisely by OpenVR). The eye distance factor
	 * is multiplied to the distance to change the user's percieved size of the
	 * world. The lower this factor is, the bigger the world will look and
	 * vice-versa. Another way of seing the effect of this factor is that it's
	 * the scaling factor of the user's head.
	 *
	 * @accessors getEyeDistanceFactor(), setEyeDistanceFactor()
	 */
	Q_PROPERTY(float eyedistancefactor READ getEyeDistanceFactor WRITE
	               setEyeDistanceFactor)
	/**
	 * @brief Transformation matrix from camera space to world space.
	 *
	 * @accessors cameraSpaceToWorldTransform()
	 */
	Q_PROPERTY(
	    QMatrix4x4 cameraspacetoworldtransform READ cameraSpaceToWorldTransform)
	/**
	 * @brief Transformation matrix from tracked space to world space.
	 *
	 * @accessors trackedSpaceToWorldTransform()
	 */
	Q_PROPERTY(QMatrix4x4 trackedspacetoworldtransform READ
	               trackedSpaceToWorldTransform)
	/**
	 * @brief Transformation matrix from hmd space to world space.
	 *
	 * @accessors hmdSpaceToWorldTransform()
	 */
	Q_PROPERTY(
	    QMatrix4x4 hmdspacetoworldtransform READ hmdSpaceToWorldTransform)
	/**
	 * @brief Transformation matrix from scaled hmd space to world space.
	 *
	 * @accessors hmdScaledSpaceToWorldTransform()
	 */
	Q_PROPERTY(QMatrix4x4 hmdscaledspacetoworldtransform READ
	               hmdScaledSpaceToWorldTransform)
	/**
	 * @brief Transformation matrix from screen space (or clip space) to world
	 * space.
	 *
	 * @accessors screenToWorldTransform()
	 */
	Q_PROPERTY(QMatrix4x4 screentoworldtransform READ screenToWorldTransform)

  protected: // protected typedefs
	/**
	 * @brief Identifiers for the six clipping planes.
	 */
	enum ClippingPlane
	{
		LEFT_PLANE   = 0,
		RIGHT_PLANE  = 1,
		BOTTOM_PLANE = 2,
		TOP_PLANE    = 3,
		NEAR_PLANE   = 4,
		FAR_PLANE    = 5,
	};
	/**
	 * @brief Planes are represented by their equations stored in a QVector4D.
	 *
	 * The plane Ax+By+Cz+D=0 is stored as QVector4D(A,B,C,D).
	 */
	typedef QVector4D Plane;

  public:
	/**
	 * @brief Constructs a @ref BasicCamera.
	 *
	 * The camera's full transformation will be the identity transform by
	 * default.
	 *
	 * @param vrHandler The engine's @ref VRHandler, wether it is active or not.
	 * It is mostly used to get VR transformations.
	 */
	explicit BasicCamera(VRHandler const* vrHandler);
	/**
	 * @getter{viewmatrix}
	 */
	QMatrix4x4 getView() const { return view; };
	/**
	 * @setter{viewmatrix, view}
	 */
	void setView(QMatrix4x4 const& view) { this->view = view; };
	/**
	 * @getter{projectionmatrix}
	 */
	QMatrix4x4 getProj() const { return proj; };
	/**
	 * @setter{projectionmatrix, proj}
	 */
	void setProj(QMatrix4x4 const& proj) { this->proj = proj; };
	/**
	 * @getter{eyedistancefactor}
	 */
	float getEyeDistanceFactor() const { return eyeDistanceFactor; };
	/**
	 * @setter{eyedistancefactor, eyeDistanceFactor}
	 */
	void setEyeDistanceFactor(float eyeDistanceFactor);
	/**
	 * @getter{cameraspacetoworldtransform}
	 */
	QMatrix4x4 cameraSpaceToWorldTransform() const;
	/**
	 * @getter{trackedspacetoworldtransform}
	 */
	QMatrix4x4 trackedSpaceToWorldTransform() const;
	/**
	 * @getter{hmdspacetoworldtransform}
	 */
	QMatrix4x4 hmdSpaceToWorldTransform() const;
	/**
	 * @getter{hmdscaledspacetoworldtransform}
	 */
	QMatrix4x4 hmdScaledSpaceToWorldTransform() const;
	/**
	 * @getter{screentoworldtransform}
	 */
	QMatrix4x4 screenToWorldTransform() const;
	/**
	 * @brief Destroys a @ref BasicCamera instance, freeing its resources.
	 */
	virtual ~BasicCamera(){};

  public slots:
	/**
	 * @brief Convenient setter for the @ref viewmatrix property.
	 *
	 * This will compute a view matrix based three high level parameters.
	 * All parameters are in world space coordinates.
	 *
	 * @param position Position of the camera.
	 * @param lookDirection Direction followed by the camera's look vector.
	 * @param up Which direction is "up" for the camera.
	 */
	void setView(QVector3D const& position, QVector3D const& lookDirection,
	             QVector3D const& up);
	/**
	 * @brief Convenient setter for the @ref viewmatrix property.
	 *
	 * This will compute a view matrix based three high level parameters.
	 * All parameters are in world space coordinates.
	 *
	 * @param position Position of the camera.
	 * @param center 3D point at the center of the view, or on which the camera
	 * is focused.
	 * @param up Which direction is "up" for the camera.
	 */
	void lookAt(QVector3D const& position, QVector3D const& center,
	            QVector3D const& up);
	/**
	 * @brief Convenient setter for the @ref projectionmatrix property.
	 *
	 * This will compute a projection matrix based four high level parameters.
	 * All parameters are in world space coordinates.
	 *
	 * @param fov Field of view of the camera.
	 * @param aspectratio Aspect ratio of the render target on which the camera
	 * will render (width / height).
	 * @param nearPlan Distance from the camera below which nothing is rendered.
	 * @param farPlan Distance from the camera above which nothing is rendered.
	 */
	void setPerspectiveProj(float fov, float aspectratio, float nearPlan = 0.1f,
	                        float farPlan = 10000.f);
	/**
	 * @brief Returns the transformation matrix from eye-related screen space
	 * (or clip space) to world space.
	 *
	 * @param side Which eye (left or right) to consider.
	 */
	QMatrix4x4 hmdScreenToWorldTransform(Side side) const;
	/**
	 * @brief Takes a world space vertex and transforms it to clip space
	 * coordinates.
	 *
	 * @param vertex Vertex in world space coordinates to project to clip space.
	 */
	QVector4D project(QVector3D const& vertex) const;
	/**
	 * @brief Takes a world space vertex and transforms it to clip space
	 * coordinates.
	 *
	 * @param vertex Vertex in world space coordinates to project to clip space.
	 */
	QVector4D project(QVector4D const& vertex) const;

  public:
	/**
	 * @brief Updates all the camera transformation matrices.
	 *
	 * If VR is enabled, the VR transformations will be applied.
	 */
	virtual void update();
	/**
	 * @brief Updates all the camera transformation matrices and forces 2D
	 * transformations.
	 *
	 * The 2D transformations will be applied wether VR is enabled or not.
	 * Useful for rendering on the companion window for example.
	 */
	virtual void update2D();
	/**
	 * @brief Calls GLHandler#setUpTransforms with this camera's transformation
	 * matrices.
	 *
	 * From then on, any call to GLHandler#render will use this camera's
	 * transformation matrices in vertex shaders.
	 */
	void uploadMatrices() const;

  protected:
	/**
	 * @brief Read-only pointer on @ref AbstractMainWin#vrHandler.
	 */
	VRHandler const* vrHandler;
	/**
	 * @brief Direct access to the @ref eyedistancefactor property.
	 */
	float eyeDistanceFactor;
	/**
	 * @brief Updates @ref clippingPlanes taking into account the current @ref
	 * fullTransform.
	 */
	void updateClippingPlanes();

	// See TRANSFORMS beyond here

	/**
	 * @brief Applies eyeDist on a copy of a matrix.
	 *
	 * The three first components of the fourth column of a copy of @p matrix
	 * will get multiplied by @p eyeDistanceFactor then returned.
	 */
	static QMatrix4x4 eyeDist(QMatrix4x4 const& matrix,
	                          float eyeDistanceFactor);
	/**
	 * @brief Applies noTrans on a copy of a matrix.
	 *
	 * The three first components of the fourth column of a copy of @p matrix
	 * will be set to zero then returned.
	 */
	static QMatrix4x4 noTrans(QMatrix4x4 const& matrix);
	/**
	 * @brief Direct access to the @ref viewmatrix property.
	 */
	QMatrix4x4 view;
	/**
	 * @brief Direct access to the @ref projectionmatrix property.
	 *
	 * Only used for 2D.
	 */
	QMatrix4x4 proj;
	/**
	 * @brief Projection matrix used by the left eye if VR is enabled.
	 *
	 * Transforms from HMD scaled space to left screen space (or clip space).
	 *
	 * See VRHandler#getProjectionMatrix.
	 */
	QMatrix4x4 projLeft;
	/**
	 * @brief Projection matrix used by the right eye if VR is enabled.
	 *
	 * Transforms from HMD scaled space to right screen space (or clip space).
	 *
	 * See VRHandler#getProjectionMatrix.
	 */
	QMatrix4x4 projRight;
	/**
	 * @brief Transformation matrix from HMD scaled space to world space.
	 *
	 * Relevant wether VR is enabled or not. If VR is disabled, this is
	 * equivalent to the transformation matrix from camera space to world space.
	 */
	QMatrix4x4 hmdScaledToWorld;
	/**
	 * @brief Correction matrix used to account for eyeDistanceFactor in several
	 * transforms.
	 */
	QMatrix4x4 eyeDistanceCorrection;

	/**
	 * @brief Transformation matrix from world space to screen space (or clip
	 * space).
	 *
	 * One of the four matrices that transform from one space to screen space
	 * (or clip space).
	 *
	 * See GLHandler#setUpTransforms and GLHandler::setUpRender's space
	 * parameter.
	 */
	QMatrix4x4 fullTransform;
	/**
	 * @brief Transformation matrix from camera space to screen space (or clip
	 * space).
	 *
	 * One of the four matrices that transform from one space to screen space
	 * (or clip space).
	 *
	 * See GLHandler#setUpTransforms and GLHandler::setUpRender's space
	 * parameter.
	 */
	QMatrix4x4 fullCameraSpaceTransform;
	/**
	 * @brief Transformation matrix from tracked space to screen space (or clip
	 * space).
	 *
	 * One of the four matrices that transform from one space to screen space
	 * (or clip space).
	 *
	 * See GLHandler#setUpTransforms and GLHandler::setUpRender's space
	 * parameter.
	 */
	QMatrix4x4 fullTrackedSpaceTransform;
	/**
	 * @brief Transformation matrix from hmd space to screen space (or clip
	 * space).
	 *
	 * One of the four matrices that transform from one space to screen space
	 * (or clip space).
	 *
	 * See GLHandler#setUpTransforms and GLHandler::setUpRender's space
	 * parameter.
	 */
	QMatrix4x4 fullHmdSpaceTransform;
	/**
	 * @brief Transformation matrix from skybox space to screen space (or clip
	 * space).
	 *
	 * One of the four matrices that transform from one space to screen space
	 * (or clip space).
	 *
	 * See GLHandler#setUpTransforms and GLHandler::setUpRender's space
	 * parameter.
	 */
	QMatrix4x4 fullSkyboxSpaceTransform;

	// For culling. Normals point Inside i.e. clippingPlanes[i]*v >= 0 <=>
	// v is at the inner side of clippingPlanes[i]
	/**
	 * @brief Static array holding the six clipping planes of the camera.
	 *
	 * For consistency, please use @ref ClippingPlane values as access indices.
	 *
	 * Mainly used for frustum culling. The normal of any plane points inside
	 * the frustum volume (let v be a QVector4D, <code>clippingPlanes[i] * v >=
	 * 0</code> if and only if v is at the inner side of clippingPlanes[i]).
	 */
	std::array<Plane, 6> clippingPlanes;
};

#include "vr/VRHandler.hpp"
#endif // BASICCAMERA_H
