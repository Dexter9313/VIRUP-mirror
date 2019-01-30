#ifndef ABSTRACTMAINWIN_H
#define ABSTRACTMAINWIN_H

#include <QDebug>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QOpenGLWindow>
#include <vector>

#include "BasicCamera.hpp"
#include "DebugCamera.hpp"
#include "GLHandler.hpp"
#include "vr/VRHandler.hpp"

class AbstractMainWin : public QOpenGLWindow
{
	Q_OBJECT
  public:
	AbstractMainWin();
	virtual ~AbstractMainWin();

  protected:
	// declare drawn resources
	virtual void initScene() = 0;

	// update physics/controls/meshes, etc...
	// prepare for rendering
	virtual void updateScene(BasicCamera& camera) = 0;

	// render user scene on camera
	// (no controllers or hands)
	virtual void renderScene(BasicCamera const& camera) = 0;

	BasicCamera& getCamera() { return *camera; };
	void setCamera(BasicCamera* newCamera);

	void keyPressEvent(QKeyEvent* event) override;

	VRHandler vrHandler;
	float const& frameTiming = frameTiming_;

  private:
	void initializeGL() override;
	void paintGL() override;

	float frameTiming_;
	QElapsedTimer frameTimer;

	BasicCamera* camera;
	DebugCamera* dbgCamera;
};

#endif // ABSTRACTMAINWIN_H
