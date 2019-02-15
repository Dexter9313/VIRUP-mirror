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
	virtual void keyPressEvent(QKeyEvent* e) override;
	virtual void vrEvent(VRHandler::Event const& e) { Q_UNUSED(e); };
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

	void appendPostProcessingShader(QString const& id, QString const& fragment);
	void insertPostProcessingShader(QString const& id, QString const& fragment,
	                                unsigned int pos);
	void removePostProcessingShader(QString const& id);
	virtual void applyPostProcShaderParams(QString const& id, GLHandler::ShaderProgram shader) const;
	void reloadPostProcessingTargets();
	bool getHDR() const { return hdr; };
	void setHDR(bool hdr);

	VRHandler vrHandler;
	float const& frameTiming = frameTiming_;

	// pair.first := shader custom id
	// pair.second := shader
	// frames will be postprocessed with effects in the same order as in this list
	QList<QPair<QString, GLHandler::ShaderProgram>> const&
	    postProcessingPipeline
	    = postProcessingPipeline_;

	float gamma = 2.2f;

  private:
	void initializeGL() override;
	void vrRender(Side side, BasicCamera* renderingCam, bool debug, bool debugInHeadset);
	void paintGL() override;

	float frameTiming_;
	QElapsedTimer frameTimer;

	BasicCamera* camera;
	DebugCamera* dbgCamera;

	bool hdr = QSettings().value("window/hdr").toBool();

	QList<QPair<QString, GLHandler::ShaderProgram>> postProcessingPipeline_;
	GLHandler::RenderTarget postProcessingTargets[2];
};

#endif // ABSTRACTMAINWIN_H
