#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QThread>
#include <QVector2D>
#include <openvr.h>

#include "../BasicCamera.hpp"
#include "../utils.hpp"

class Controller : public QObject
{
	Q_OBJECT
  public:
	Controller(vr::IVRSystem* vr_pointer, unsigned int nDevice, Side side);
	void update(QMatrix4x4 const& model);
	void render() const;
	~Controller();

	const Side side;
	const unsigned int nDevice;

  public slots:
	Side getSide() const { return side; };
	QVector2D getPadCoords() const { return padCoords; };
	float getTriggerValue() const { return triggerValue; };
	QVector3D getPosition() const { return QVector3D(model.column(3)); };

  private:
	vr::IVRSystem* const vr_pointer;
	const int triggerid;
	const int padid;

	QVector2D padCoords;
	float triggerValue = 0.f;

	GLHandler::ShaderProgram shaderProgram;
	GLHandler::Mesh mesh;
	GLHandler::Texture tex;
	QMatrix4x4 model;
};

#endif // CONTROLLER_H
