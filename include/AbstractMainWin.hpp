#ifndef ABSTRACTMAINWIN_H
#define ABSTRACTMAINWIN_H

#include <QDebug>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QOpenGLWindow>
#include <QProcess>
#include <vector>

#include "BasicCamera.hpp"
#include "DebugCamera.hpp"
#include "GLHandler.hpp"
#include "PythonQtHandler.hpp"
#include "vr/VRHandler.hpp"

class AbstractMainWin : public QOpenGLWindow
{
	Q_OBJECT
  public:
	AbstractMainWin();
	virtual ~AbstractMainWin();

  public slots:
	bool isFullscreen() const;
	void setFullscreen(bool fullscreen);
	void toggleFullscreen();
	bool getHDR() const { return hdr; };
	void setHDR(bool hdr);
	bool vrIsEnabled() const;
	void setVR(bool vr);
	void toggleVR();
	void appendPostProcessingShader(QString const& id, QString const& fragment);
	void insertPostProcessingShader(QString const& id, QString const& fragment,
	                                unsigned int pos);
	void removePostProcessingShader(QString const& id);

  protected:
	virtual bool event(QEvent* e) override;
	virtual void keyPressEvent(QKeyEvent* e) override;
	virtual void keyReleaseEvent(QKeyEvent* e) override;
	virtual void vrEvent(VRHandler::Event const& e);
	// declare drawn resources
	virtual void initScene() = 0;

	// update physics/controls/meshes, etc...
	// prepare for rendering
	virtual void updateScene(BasicCamera& camera) = 0;

	// render user scene on camera
	// (no controllers or hands)
	virtual void renderScene(BasicCamera const& camera) = 0;

	BasicCamera& getCamera() { return *camera; };
	DebugCamera& getDebugCamera() { return *dbgCamera; };
	void setCamera(BasicCamera* newCamera);

	virtual void
	    applyPostProcShaderParams(QString const& id,
	                              GLHandler::ShaderProgram shader) const;
	void reloadPostProcessingTargets();

	VRHandler vrHandler;
	float const& frameTiming = frameTiming_;

	// pair.first := shader custom id
	// pair.second := shader
	// frames will be postprocessed with effects in the same order as in this
	// list
	QList<QPair<QString, GLHandler::ShaderProgram>> const&
	    postProcessingPipeline
	    = postProcessingPipeline_;

	float gamma = 2.2f;

  private:
	void initializeGL() override;
	void vrRender(Side side, BasicCamera* renderingCam, bool debug,
	              bool debugInHeadset);
	void paintGL() override;
	void resizeGL(int w, int h) override;

	float frameTiming_ = 0.f;
	QElapsedTimer frameTimer;

	BasicCamera* camera    = nullptr;
	DebugCamera* dbgCamera = nullptr;

	bool hdr = QSettings().value("window/hdr").toBool();

	QList<QPair<QString, GLHandler::ShaderProgram>> postProcessingPipeline_;
	std::array<GLHandler::RenderTarget, 2> postProcessingTargets = {{{}, {}}};
};

#endif // ABSTRACTMAINWIN_H
