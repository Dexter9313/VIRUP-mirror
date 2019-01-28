#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QThread>
#include <QVector2D>
#include <openvr.h>

#include "../BasicCamera.hpp"
#include "../utils.hpp"

class Controller
{
  public:
	Controller(vr::IVRSystem* vr_pointer, unsigned int nDevice, Side side);
	void update(QMatrix4x4 const& model);
	QVector2D const& getPadCoords() const { return padCoords; };
	float getTriggerValue() const { return triggerValue; };
	QVector3D getPosition() const { return QVector3D(model.column(3)); };
	void render() const;
	~Controller();

	const Side side;
	const unsigned int nDevice;

  private:
	vr::IVRSystem* const vr_pointer;
	const int triggerid;
	const int padid;

	QVector2D padCoords;
	float triggerValue;

	GLHandler::ShaderProgram shaderProgram;
	GLHandler::Mesh mesh;
	GLHandler::Texture tex;
	QMatrix4x4 model;
};

#endif // CONTROLLER_H
