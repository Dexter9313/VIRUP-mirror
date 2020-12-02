#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QThread>
#include <QVector2D>
#include <openvr.h>

#include "../BasicCamera.hpp"
#include "../utils.hpp"

/** @ingroup pycall
 *
 * @brief Represents a VR controller.
 *
 * Callable in Python as the "leftController" or "rightController" objects when
 * relevant (VR is enabled and controllers are available).
 *
 * This class should be only access as a constant reference as it should only be
 * modified by the @ref VRHandler class.
 */
class Controller : public QObject
{
	Q_OBJECT
	/**
	 * @brief Side of the controller (left or right).
	 *
	 * @accessors getSide()
	 */
	Q_PROPERTY(Side side READ getSide)
	/**
	 * @brief Coordinates of the thumb on the pad.
	 *
	 * Exactly (0,0) by default (no finger touches the pad).
	 * Coordinates range from -1 to 1.
	 *
	 * @accessors getPadCoords()
	 */
	Q_PROPERTY(QVector2D padcoords READ getPadCoords)
	/**
	 * @brief Value of the press on the trigger.
	 *
	 * Ranges from 0 (no press) to 1 (full press).
	 *
	 * @accessors getTriggerValue()
	 */
	Q_PROPERTY(float triggervalue READ getTriggerValue)
	/**
	 * @brief Position of the controller in seated tracked space coordinates.
	 *
	 * Multiply the transform given by BasicCamera#trackedSpaceToWorldTransform
	 * with this position to compute the position of the controller in world
	 * space coordinates for example.
	 */
	Q_PROPERTY(QVector3D position READ getPosition)

  public:
	/**
	 * @brief Constructs a @ref Controller.
	 *
	 * Don't construct a Controller yourself, @ref VRHandler will handle it. Use
	 * VRHandler#getController instead.
	 *
	 * @param vr_pointer The main IVRSystem from OpenVR. Mainly used to get
	 * information.
	 * @param nDevice OpenVR's internal device id for this controller.
	 * @param side Which side is this controller (left or right).
	 */
	Controller(vr::IVRSystem* vr_pointer, unsigned int nDevice, Side side);
	/**
	 * @brief OpenVR's internal device id for this controller.
	 */
	unsigned int getNDevice() const { return nDevice; };
	/**
	 * @getter{side}
	 */
	Side getSide() const { return side; };
	/**
	 * @getter{padcoords}
	 */
	QVector2D getPadCoords() const { return padCoords; };
	/**
	 * @getter{triggervalue}
	 */
	float getTriggerValue() const { return triggerValue; };
	/**
	 * @getter{position}
	 */
	QVector3D getPosition() const { return QVector3D(model.column(3)); };
	/**
	 * @brief Updates every property of the controller and its internal state.
	 *
	 * @ref VRHandler will do it, don't use it yourself.
	 *
	 * @param model New model matrix of the controller.
	 */
	void update(QMatrix4x4 const& model, unsigned int nDevice);
	/**
	 * @brief Renders the controller.
	 */
	void render() const;

	/**
	 * @brief Read-only direct access to the @ref side property.
	 */
	const Side side;

	~Controller() { delete tex; };

  private:
	unsigned int nDevice;

	vr::IVRSystem* const vr_pointer;
	const int triggerid;
	const int padid;

	QVector2D padCoords;
	float triggerValue = 0.f;

	GLShaderProgram shaderProgram;
	GLMesh mesh;
	GLTexture* tex = nullptr;
	QMatrix4x4 model;
};

#endif // CONTROLLER_H
