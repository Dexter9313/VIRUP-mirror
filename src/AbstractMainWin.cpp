#include "AbstractMainWin.hpp"

AbstractMainWin::AbstractMainWin()
    : QOpenGLWindow()
{
	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	format.setVersion(4, 0);
	format.setSwapInterval(0);
	setFormat(format);

	if(hdr)
		GLHandler::defaultRenderTargetFormat = GL_RGBA16F;
}

void AbstractMainWin::keyPressEvent(QKeyEvent* e)
{
	if(e->key() == Qt::Key_F1)
	{
		QSettings().setValue(
		    "debugcamera/enabled",
		    !QSettings().value("debugcamera/enabled").toBool());
	}
	if(e->key() == Qt::Key_F11)
	{
		if(!vrHandler)
		{
			if(vrHandler.init())
				vrHandler.resetPos();
		}
		else
		{
			vrHandler.close();
		}
	}
	else if(e->key() == Qt::Key_Escape)
	{
		close();
	}
}

void AbstractMainWin::setCamera(BasicCamera* newCamera)
{
	delete camera;
	camera = newCamera;
}

void AbstractMainWin::appendPostProcessingShader(QString const& id,
                                                 QString const& fragment)
{
	postProcessingPipeline_.append(QPair<QString, GLHandler::ShaderProgram>(
	    id, GLHandler::newShader("postprocess", fragment)));
}

void AbstractMainWin::insertPostProcessingShader(QString const& id,
                                                 QString const& fragment,
                                                 unsigned int pos)
{
	postProcessingPipeline_.insert(
	    pos, QPair<QString, GLHandler::ShaderProgram>(
	             id, GLHandler::newShader("postprocess", fragment)));
}

void AbstractMainWin::removePostProcessingShader(QString const& id)
{
	for(int i(0); i < postProcessingPipeline_.size(); ++i)
	{
		if(postProcessingPipeline_[i].first == id)
		{
			postProcessingPipeline_.removeAt(i);
			break;
		}
	}
}

void AbstractMainWin::applyPostProcShaderParams(
    QString const& id, GLHandler::ShaderProgram shader) const
{
	if(id == "colors")
	{
		GLHandler::setShaderParam(shader, "gamma", gamma);
		GLHandler::setShaderParam(shader, "hdr", hdr);
	}
}

void AbstractMainWin::reloadPostProcessingTargets()
{
	GLHandler::defaultRenderTargetFormat = hdr ? GL_RGBA16F : GL_RGBA;

	GLHandler::deleteRenderTarget(postProcessingTargets[0]);
	GLHandler::deleteRenderTarget(postProcessingTargets[1]);
	postProcessingTargets[0] = GLHandler::newRenderTarget(width(), height());
	postProcessingTargets[1] = GLHandler::newRenderTarget(width(), height());

}

void AbstractMainWin::setHDR(bool hdr)
{
	this->hdr = hdr;
	reloadPostProcessingTargets();
}

void AbstractMainWin::initializeGL()
{
	// Init GL
	GLHandler::init();
	// Init VR
	if(QSettings().value("vr/enabled").toBool())
		vrHandler.init();

	qDebug() << "Using OpenGL " << format().majorVersion() << "."
	         << format().minorVersion() << '\n';
	resize(QSettings().value("window/width").toUInt(),
	       QSettings().value("window/height").toUInt());

	dbgCamera = new DebugCamera(&vrHandler);
	dbgCamera->lookAt({2, 0, 2}, {0, 0, 0}, {0, 0, 1});
	dbgCamera->setPerspectiveProj(70.0f, (float) width() / (float) height());

	camera = new BasicCamera(&vrHandler);
	camera->lookAt({1, 1, 1}, {0, 0, 0}, {0, 0, 1});
	camera->setPerspectiveProj(70.0f, (float) width() / (float) height());
	if(vrHandler)
		vrHandler.resetPos();

	postProcessingTargets[0] = GLHandler::newRenderTarget(width(), height());
	postProcessingTargets[1] = GLHandler::newRenderTarget(width(), height());

	// let user init
	initScene();

	// make sure gamma correction is applied last
	appendPostProcessingShader("colors", "colors");

	frameTimer.start();
}

void AbstractMainWin::vrRender(Side side, BasicCamera* renderingCam, bool debug,
                               bool debugInHeadset)
{
	vrHandler.beginRendering(side, postProcessingPipeline_.size() > 0);
	camera->update();
	dbgCamera->update();
	renderingCam->uploadMatrices();
	vrHandler.renderControllers();
	vrHandler.renderHands();
	// render scene
	renderScene(*camera);
	if(debug && debugInHeadset)
		dbgCamera->renderCamera(camera);

	// do all postprocesses except last one
	for(int i(0); i < postProcessingPipeline_.size() - 1; ++i)
	{
		applyPostProcShaderParams(postProcessingPipeline_[i].first,
		                          postProcessingPipeline_[i].second);
		GLHandler::postProcess(
		    postProcessingPipeline_[i].second,
		    vrHandler.getPostProcessingTarget(i % 2),
		    vrHandler.getPostProcessingTarget((i + 1) % 2));
	}
	// render last one on true target
	if(postProcessingPipeline_.size() != 0)
	{
		int i = postProcessingPipeline_.size() - 1;
		applyPostProcShaderParams(postProcessingPipeline_[i].first,
		                          postProcessingPipeline_[i].second);
		GLHandler::postProcess(
		    postProcessingPipeline_[i].second,
		    vrHandler.getPostProcessingTarget(i % 2),
		    vrHandler.getEyeTarget(side));
	}

	vrHandler.submitRendering(side);
}

void AbstractMainWin::paintGL()
{
	frameTiming_ = frameTimer.restart() / 1000.f;

	setTitle(QString(PROJECT_NAME) + " - " + QString::number(1.f / frameTiming)
	         + " FPS");

	if(vrHandler)
		frameTiming_ = vrHandler.getFrameTiming() / 1000.f;

	// handle VR events if any
	if(vrHandler)
	{
		VRHandler::Event e;
		while(vrHandler.pollEvent(&e))
			vrEvent(e);
	}
	// let user update before rendering
	updateScene(*camera);

	bool debug(QSettings().value("debugcamera/enabled").toBool());
	bool debugInHeadset(
	    QSettings().value("debugcamera/debuginheadset").toBool());
	BasicCamera* renderingCam(
	    debug && ((debugInHeadset && vrHandler) || !vrHandler) ? dbgCamera
	                                                           : camera);

	// main render logic
	if(vrHandler)
	{
		vrHandler.prepareRendering();

		vrRender(Side::LEFT, renderingCam, debug, debugInHeadset);
		vrRender(Side::RIGHT, renderingCam, debug, debugInHeadset);

		if(!debug || debugInHeadset)
			vrHandler.displayOnCompanion(width(), height());
		else
			renderingCam = dbgCamera;
	}
	// if no VR or debug not in headset, render 2D
	if(!vrHandler || (debug && !debugInHeadset))
	{
		if(postProcessingPipeline_.size() == 0)
			GLHandler::beginRendering();
		else
			GLHandler::beginRendering(postProcessingTargets[0]);
		camera->update();
		dbgCamera->update();
		renderingCam->uploadMatrices();
		// render scene
		renderScene(*camera);
		if(debug)
			dbgCamera->renderCamera(camera);

		// do all postprocesses except last one
		for(int i(0); i < postProcessingPipeline_.size() - 1; ++i)
		{
			applyPostProcShaderParams(postProcessingPipeline_[i].first,
			                          postProcessingPipeline_[i].second);
			GLHandler::postProcess(postProcessingPipeline_[i].second,
			                       postProcessingTargets[i % 2],
			                       postProcessingTargets[(i + 1) % 2]);
		}
		// render last one on screen target
		if(postProcessingPipeline_.size() != 0)
		{
			int i = postProcessingPipeline_.size() - 1;
			applyPostProcShaderParams(postProcessingPipeline_[i].first,
			                          postProcessingPipeline_[i].second);
			GLHandler::postProcess(postProcessingPipeline_[i].second,
			                       postProcessingTargets[i % 2]);
		}
	}

	// Trigger a repaint immediatly
	update();
}

AbstractMainWin::~AbstractMainWin()
{
	for(QPair<QString, GLHandler::ShaderProgram> p : postProcessingPipeline_)
		GLHandler::deleteShader(p.second);
	delete camera;
	delete dbgCamera;

	GLHandler::deleteRenderTarget(postProcessingTargets[0]);
	GLHandler::deleteRenderTarget(postProcessingTargets[1]);
}
