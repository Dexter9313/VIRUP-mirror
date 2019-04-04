#include "AbstractMainWin.hpp"

AbstractMainWin::AbstractMainWin()
{
	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	format.setVersion(4, 0);
	format.setSwapInterval(0);
	setFormat(format);

	if(hdr)
	{
		GLHandler::defaultRenderTargetFormat() = GL_RGBA16F;
	}

	PythonQtHandler::init();
	PythonQtHandler::addClass<int>("Side");
	PythonQtHandler::addObject("Side", new PySide);
	PythonQtHandler::addObject("GLHandler", new GLHandler);
}

bool AbstractMainWin::isFullscreen() const
{
	return QSettings().value("window/fullscreen").toBool();
}

void AbstractMainWin::setFullscreen(bool fullscreen)
{
	QSettings().setValue("window/fullscreen", fullscreen);
	if(fullscreen)
	{
		QRect screenGeometry(QApplication::desktop()->screenGeometry());
		resize(screenGeometry.width(), screenGeometry.height());
	}
	else
	{
		resize(QSettings().value("window/width").toUInt(),
		       QSettings().value("window/height").toUInt());
	}
	if(fullscreen)
	{
		showFullScreen();
	}
	else
	{
		show();
	}
}

void AbstractMainWin::toggleFullscreen()
{
	setFullscreen(!isFullscreen());
}

bool AbstractMainWin::vrIsEnabled() const
{
	return static_cast<bool>(vrHandler);
}

void AbstractMainWin::setVR(bool vr)
{
	if(vrHandler && !vr)
	{
		vrHandler.close();
	}
	else if(!vrHandler && vr)
	{
		if(vrHandler.init())
		{
			vrHandler.resetPos();
		}
	}
	if(vrIsEnabled())
	{
		PythonQtHandler::addObject("VRHandler", &vrHandler);
	}
	else
	{
		PythonQtHandler::evalScript(
		    "if \"VRHandler\" in dir():\n\tdel VRHandler");
	}
	QSettings().setValue("vr/enabled", vrIsEnabled());
}

void AbstractMainWin::toggleVR()
{
	setVR(!vrIsEnabled());
}

bool AbstractMainWin::event(QEvent* e)
{
	if(e->type() == QEvent::Type::Close)
	{
		PythonQtHandler::closeConsole();
	}
	return QOpenGLWindow::event(e);
}

void AbstractMainWin::keyPressEvent(QKeyEvent* e)
{
	if(e->key() == Qt::Key_F1)
	{
		dbgCamera->toggle();
	}
	if(e->key() == Qt::Key_F8)
	{
		PythonQtHandler::toggleConsole();
	}
	if(e->key() == Qt::Key_F11)
	{
		toggleVR();
	}
	else if(e->key() == Qt::Key_Return
	        && (e->modifiers() & Qt::AltModifier) != 0)
	{
		toggleFullscreen();
	}
	else if(e->key() == Qt::Key_Escape)
	{
		close();
	}

	if(!PythonQtHandler::isSupported())
	{
		return;
	}

	QString pyKeyEvent("QKeyEvent(");
	pyKeyEvent += QString::number(e->type()) + ",";
	pyKeyEvent += QString::number(e->key()) + ",";
	pyKeyEvent += QString::number(e->modifiers()) + ",";
	pyKeyEvent += QString::number(e->nativeScanCode()) + ",";
	pyKeyEvent += QString::number(e->nativeVirtualKey()) + ",";
	pyKeyEvent += QString::number(e->nativeModifiers()) + ",";
	if(e->key() != Qt::Key_Return && e->key() != Qt::Key_Enter)
	{
		pyKeyEvent += "\"" + e->text().replace('"', "\\\"") + "\",";
	}
	else
	{
		pyKeyEvent += R"("\n",)";
	}
	pyKeyEvent += e->isAutoRepeat() ? "True," : "False,";
	pyKeyEvent += QString::number(e->count()) + ")";

	PythonQtHandler::evalScript(
	    "if \"keyPressEvent\" in dir():\n\tkeyPressEvent(" + pyKeyEvent + ")");
}

void AbstractMainWin::keyReleaseEvent(QKeyEvent* e)
{
	if(!PythonQtHandler::isSupported())
	{
		return;
	}

	QString pyKeyEvent("QKeyEvent(");
	pyKeyEvent += QString::number(e->type()) + ",";
	pyKeyEvent += QString::number(e->key()) + ",";
	pyKeyEvent += QString::number(e->modifiers()) + ",";
	pyKeyEvent += QString::number(e->nativeScanCode()) + ",";
	pyKeyEvent += QString::number(e->nativeVirtualKey()) + ",";
	pyKeyEvent += QString::number(e->nativeModifiers()) + ",";
	if(e->key() != Qt::Key_Return && e->key() != Qt::Key_Enter)
	{
		pyKeyEvent += "\"" + e->text().replace('"', "\\\"") + "\",";
	}
	else
	{
		pyKeyEvent += R"("\n",)";
	}
	pyKeyEvent += e->isAutoRepeat() ? "True," : "False,";
	pyKeyEvent += QString::number(e->count()) + ")";

	PythonQtHandler::evalScript(
	    "if \"keyReleaseEvent\" in dir():\n\tkeyReleaseEvent(" + pyKeyEvent
	    + ")");
}

void AbstractMainWin::vrEvent(VRHandler::Event const& e)
{
	PythonQtHandler::evalScript(
	    "if \"vrEvent\" in dir():\n\tvrEvent("
	    + QString::number(static_cast<int>(e.type)) + ","
	    + QString::number(static_cast<int>(e.side)) + ", "
	    + QString::number(static_cast<int>(e.button)) + ")");
}

void AbstractMainWin::setupPythonAPI()
{
	PythonQtHandler::addObject("HydrogenVR", this);
}

void AbstractMainWin::setCamera(BasicCamera* newCamera)
{
	delete camera;
	camera = newCamera;
	PythonQtHandler::addObject("camera", camera);
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
		GLHandler::setShaderParam(shader, "hdr", static_cast<float>(hdr));
	}
	else
	{
		QString pyCmd("if \"applyPostProcShaderParams\" in "
		              "dir():\n\tapplyPostProcShaderParams(\""
		              + id + "\"," + QString::number(shader) + ")");
		PythonQtHandler::evalScript(pyCmd);
	}
}

void AbstractMainWin::reloadPostProcessingTargets()
{
	GLHandler::defaultRenderTargetFormat() = hdr ? GL_RGBA16F : GL_RGBA;

	GLHandler::deleteRenderTarget(postProcessingTargets[0]);
	GLHandler::deleteRenderTarget(postProcessingTargets[1]);
	postProcessingTargets[0] = GLHandler::newRenderTarget(width(), height());
	postProcessingTargets[1] = GLHandler::newRenderTarget(width(), height());

	if(vrHandler)
	{
		vrHandler.reloadPostProcessingTargets();
	}
}

void AbstractMainWin::setHDR(bool hdr)
{
	this->hdr = hdr;
	QSettings().setValue("window/hdr", hdr);
	reloadPostProcessingTargets();
}

void AbstractMainWin::toggleHDR()
{
	setHDR(!getHDR());
}

void AbstractMainWin::initializeGL()
{
	// Init GL
	GLHandler::init();
	// Init VR
	setVR(QSettings().value("vr/enabled").toBool());
	// Init Python API
	setupPythonAPI();

	// NOLINTNEXTLINE(hicpp-no-array-decay)
	qDebug() << "Using OpenGL " << format().majorVersion() << "."
	         << format().minorVersion() << '\n';

	dbgCamera = new DebugCamera(&vrHandler);
	dbgCamera->lookAt({2, 0, 2}, {0, 0, 0}, {0, 0, 1});
	dbgCamera->setPerspectiveProj(70.0f, static_cast<float>(width())
	                                         / static_cast<float>(height()));

	camera = new BasicCamera(&vrHandler);
	camera->lookAt({1, 1, 1}, {0, 0, 0}, {0, 0, 1});
	camera->setPerspectiveProj(70.0f, static_cast<float>(width())
	                                      / static_cast<float>(height()));

	PythonQtHandler::addObject("camera", camera);
	PythonQtHandler::addObject("dbgcamera", dbgCamera);

	if(vrHandler)
	{
		vrHandler.resetPos();
	}

	postProcessingTargets[0] = GLHandler::newRenderTarget(width(), height());
	postProcessingTargets[1] = GLHandler::newRenderTarget(width(), height());

	// let user init
	initScene();

	QString mainScriptPath(QSettings().value("scripting/rootdir").toString()
	                       + "/main.py");
	if(QFile(mainScriptPath).exists())
	{
		PythonQtHandler::evalFile(mainScriptPath);
	}

	PythonQtHandler::evalScript("if \"initScene\" in dir():\n\tinitScene()");

	// make sure gamma correction is applied last
	appendPostProcessingShader("colors", "colors");

	frameTimer.start();
}

void AbstractMainWin::vrRender(Side side, BasicCamera* renderingCam, bool debug,
                               bool debugInHeadset)
{
	vrHandler.beginRendering(side, !postProcessingPipeline_.empty());
	camera->update();
	dbgCamera->update();
	renderingCam->uploadMatrices();
	vrHandler.renderControllers();
	vrHandler.renderHands();
	// render scene
	renderScene(*camera);
	PythonQtHandler::evalScript(
	    "if \"renderScene\" in dir():\n\trenderScene()");
	if(debug && debugInHeadset)
	{
		dbgCamera->renderCamera(camera);
	}

	// do all postprocesses except last one
	for(int i(0); i < postProcessingPipeline_.size() - 1; ++i)
	{
		applyPostProcShaderParams(postProcessingPipeline_[i].first,
		                          postProcessingPipeline_[i].second);
		GLHandler::postProcess(postProcessingPipeline_[i].second,
		                       vrHandler.getPostProcessingTarget(i % 2),
		                       vrHandler.getPostProcessingTarget((i + 1) % 2));
	}
	// render last one on true target
	if(!postProcessingPipeline_.empty())
	{
		int i = postProcessingPipeline_.size() - 1;
		applyPostProcShaderParams(postProcessingPipeline_[i].first,
		                          postProcessingPipeline_[i].second);
		GLHandler::postProcess(postProcessingPipeline_[i].second,
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
	{
		frameTiming_ = vrHandler.getFrameTiming() / 1000.f;
	}

	// handle VR events if any
	if(vrHandler)
	{
		auto e = new VRHandler::Event;
		while(vrHandler.pollEvent(e))
		{
			vrEvent(*e);
		}
		delete e;
	}
	// let user update before rendering
	updateScene(*camera);
	PythonQtHandler::evalScript(
	    "if \"updateScene\" in dir():\n\tupdateScene()");

	bool debug(dbgCamera->isEnabled());
	bool debugInHeadset(dbgCamera->debugInHeadset());
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
		{
			vrHandler.displayOnCompanion(width(), height());
		}
		else
		{
			renderingCam = dbgCamera;
		}
	}
	// if no VR or debug not in headset, render 2D
	if(!vrHandler || (debug && !debugInHeadset))
	{
		if(postProcessingPipeline_.empty())
		{
			GLHandler::beginRendering();
		}
		else
		{
			GLHandler::beginRendering(postProcessingTargets[0]);
		}
		camera->update();
		dbgCamera->update();
		renderingCam->uploadMatrices();
		// render scene
		renderScene(*camera);
		PythonQtHandler::evalScript(
		    "if \"renderScene\" in dir():\n\trenderScene()");
		if(debug)
		{
			dbgCamera->renderCamera(camera);
		}

		// do all postprocesses except last one
		for(int i(0); i < postProcessingPipeline_.size() - 1; ++i)
		{
			applyPostProcShaderParams(postProcessingPipeline_[i].first,
			                          postProcessingPipeline_[i].second);
			GLHandler::postProcess(postProcessingPipeline_[i].second,
			                       postProcessingTargets.at(i % 2),
			                       postProcessingTargets.at((i + 1) % 2));
		}
		// render last one on screen target
		if(!postProcessingPipeline_.empty())
		{
			int i = postProcessingPipeline_.size() - 1;
			applyPostProcShaderParams(postProcessingPipeline_[i].first,
			                          postProcessingPipeline_[i].second);
			GLHandler::postProcess(postProcessingPipeline_[i].second,
			                       postProcessingTargets.at(i % 2));
		}
	}

	// Trigger a repaint immediatly
	update();
}

void AbstractMainWin::resizeGL(int w, int h)
{
	QSettings().setValue("window/width", w);
	QSettings().setValue("window/height", h);
	camera->setPerspectiveProj(70.0f, static_cast<float>(width())
	                                      / static_cast<float>(height()));
	dbgCamera->setPerspectiveProj(70.0f, static_cast<float>(width())
	                                         / static_cast<float>(height()));
}

AbstractMainWin::~AbstractMainWin()
{
	PythonQtHandler::evalScript(
	    "if \"cleanUpScene\" in dir():\n\tcleanUpScene()");
	for(const QPair<QString, GLHandler::ShaderProgram>& p :
	    postProcessingPipeline_)
	{
		GLHandler::deleteShader(p.second);
	}
	delete camera;
	delete dbgCamera;

	GLHandler::deleteRenderTarget(postProcessingTargets[0]);
	GLHandler::deleteRenderTarget(postProcessingTargets[1]);

	vrHandler.close();
	PythonQtHandler::clean();
}
