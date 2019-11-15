#include "AbstractMainWin.hpp"

AbstractMainWin::AbstractMainWin()
{
	setSurfaceType(QSurface::OpenGLSurface);

	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	format.setVersion(4, 0);
	format.setSwapInterval(0);
	setFormat(format);

	m_context.setFormat(format);
	m_context.create();

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

void AbstractMainWin::takeScreenshot(QString path) const
{
	QImage screenshot(GLHandler::generateScreenshot());
	if(path == "")
	{
		path = QFileDialog::getSaveFileName(
		    nullptr, tr("Save Screenshot"),
		    QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
		    tr("Images (*.png *.xpm *.jpg)"));
	}
	screenshot.mirrored(false, true).save(path);
}

bool AbstractMainWin::event(QEvent* e)
{
	if(e->type() == QEvent::UpdateRequest)
	{
		paintGL();
		return true;
	}
	if(e->type() == QEvent::Type::Close)
	{
		PythonQtHandler::closeConsole();
	}
	return QWindow::event(e);
}

void AbstractMainWin::keyPressEvent(QKeyEvent* e)
{
	QString modifier;
	QString key;

	if((e->modifiers() & Qt::ShiftModifier) != 0u)
	{
		modifier += "Shift+";
	}
	if((e->modifiers() & Qt::ControlModifier) != 0u)
	{
		modifier += "Ctrl+";
	}
	if((e->modifiers() & Qt::AltModifier) != 0u)
	{
		modifier += "Alt+";
	}
	if((e->modifiers() & Qt::MetaModifier) != 0u)
	{
		modifier += "Meta+";
	}

	key = QKeySequence(e->key()).toString();

	QKeySequence ks(modifier + key);
	actionEvent(inputManager[ks], true);

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
	QString modifier;
	QString key;

	if((e->modifiers() & Qt::ShiftModifier) != 0u)
	{
		modifier += "Shift+";
	}
	if((e->modifiers() & Qt::ControlModifier) != 0u)
	{
		modifier += "Ctrl+";
	}
	if((e->modifiers() & Qt::AltModifier) != 0u)
	{
		modifier += "Alt+";
	}
	if((e->modifiers() & Qt::MetaModifier) != 0u)
	{
		modifier += "Meta+";
	}

	key = QKeySequence(e->key()).toString();

	QKeySequence ks(modifier + key);
	actionEvent(inputManager[ks], false);

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

void AbstractMainWin::actionEvent(BaseInputManager::Action a, bool pressed)
{
	if(!pressed)
	{
		return;
	}

	if(a.id == "toggledbgcam")
	{
		dbgCamera->toggle();
	}
	else if(a.id == "togglewireframe")
	{
		toggleWireframe();
	}
	else if(a.id == "togglepyconsole")
	{
		PythonQtHandler::toggleConsole();
	}
	else if(a.id == "togglevr")
	{
		toggleVR();
	}
	else if(a.id == "screenshot")
	{
		takeScreenshot();
	}
	else if(a.id == "togglefullscreen")
	{
		toggleFullscreen();
	}
	else if(a.id == "quit")
	{
		close();
	}
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

void AbstractMainWin::renderVRControls() const
{
	if(vrHandler)
	{
		vrHandler.renderControllers();
		vrHandler.renderHands();
	}
}

BasicCamera const& AbstractMainWin::getCamera(QString const& pathId) const
{
	for(auto const& pair : sceneRenderPipeline)
	{
		if(pair.first == pathId)
		{
			return *(pair.second.camera);
		}
	}
	throw(std::domain_error(std::string("Path id doesn't exist.")
	                        + pathId.toStdString()));
}

BasicCamera& AbstractMainWin::getCamera(QString const& pathId)
{
	for(auto const& pair : sceneRenderPipeline)
	{
		if(pair.first == pathId)
		{
			return *(pair.second.camera);
		}
	}
	throw(std::domain_error(std::string("Path id doesn't exist.")
	                        + pathId.toStdString()));
}

void AbstractMainWin::appendSceneRenderPath(QString const& id, RenderPath path)
{
	sceneRenderPipeline_.append(QPair<QString, RenderPath>(id, path));
}

void AbstractMainWin::insertSceneRenderPath(QString const& id, RenderPath path,
                                            unsigned int pos)
{
	sceneRenderPipeline_.insert(pos, QPair<QString, RenderPath>(id, path));
}

void AbstractMainWin::removeSceneRenderPath(QString const& id)
{
	for(int i(0); i < sceneRenderPipeline_.size(); ++i)
	{
		if(sceneRenderPipeline_[i].first == id)
		{
			delete sceneRenderPipeline_[i].second.camera;
			sceneRenderPipeline_.removeAt(i);
			break;
		}
	}
}

void AbstractMainWin::appendPostProcessingShader(
    QString const& id, QString const& fragment,
    QMap<QString, QString> const& defines)
{
	postProcessingPipeline_.append(QPair<QString, GLHandler::ShaderProgram>(
	    id, GLHandler::newShader("postprocess", fragment, defines)));
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

std::vector<GLHandler::Texture>
    AbstractMainWin::getPostProcessingUniformTextures(
        QString const& /*id*/, GLHandler::ShaderProgram /*shader*/) const
{
	return {};
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
	reloadPostProcessingTargets();
}

void AbstractMainWin::toggleHDR()
{
	setHDR(!getHDR());
}

void AbstractMainWin::toggleWireframe()
{
	setWireframe(!getWireframe());
}

void AbstractMainWin::initializeGL()
{
	m_context.makeCurrent(this);
	// Init GL
	GLHandler::init();
	// Init VR
	setVR(QSettings().value("vr/enabled").toBool());
	// Init Python API
	setupPythonAPI();
	// Init libraries
	initLibraries();

	// NOLINTNEXTLINE(hicpp-no-array-decay)
	qDebug() << "Using OpenGL " << format().majorVersion() << "."
	         << format().minorVersion() << '\n';

	dbgCamera = new DebugCamera(&vrHandler);
	dbgCamera->lookAt({2, 0, 2}, {0, 0, 0}, {0, 0, 1});
	dbgCamera->setPerspectiveProj(70.0f, static_cast<float>(width())
	                                         / static_cast<float>(height()));

	auto defaultCam = new BasicCamera(&vrHandler);
	defaultCam->lookAt({1, 1, 1}, {0, 0, 0}, {0, 0, 1});
	defaultCam->setPerspectiveProj(70.0f, static_cast<float>(width())
	                                          / static_cast<float>(height()));
	appendSceneRenderPath("default", RenderPath(defaultCam));

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
	if(QSettings().value("graphics/dithering").toBool())
	{
		appendPostProcessingShader("colors", "colors", {{"DITHERING", "0"}});
	}
	else
	{
		appendPostProcessingShader("colors", "colors");
	}

	frameTimer.start();
	initialized = true;
}

void AbstractMainWin::vrRenderSinglePath(RenderPath& renderPath,
                                         QString const& pathId, bool debug,
                                         bool debugInHeadset)
{
	GLHandler::glf().glClear(renderPath.clearMask);
	renderPath.camera->update();
	dbgCamera->update();

	if(debug && debugInHeadset)
	{
		dbgCamera->uploadMatrices();
	}
	else
	{
		renderPath.camera->uploadMatrices();
	}
	if(pathIdRenderingControllers == pathId && renderControllersBeforeScene)
	{
		renderVRControls();
	}
	// render scene
	if(wireframe)
	{
		GLHandler::beginWireframe();
	}
	renderScene(*renderPath.camera, pathId);
	if(pathIdRenderingControllers == pathId && !renderControllersBeforeScene)
	{
		renderVRControls();
	}
	PythonQtHandler::evalScript(
	    "if \"renderScene\" in dir():\n\trenderScene()");
	if(debug && debugInHeadset)
	{
		dbgCamera->renderCamera(renderPath.camera);
	}
	if(wireframe)
	{
		GLHandler::endWireframe();
	}
}

void AbstractMainWin::vrRender(Side side, bool debug, bool debugInHeadset)
{
	vrHandler.beginRendering(side);

	for(auto pair : sceneRenderPipeline_)
	{
		vrRenderSinglePath(pair.second, pair.first, debug, debugInHeadset);
	}

	// do all postprocesses including last one
	int i(0);
	for(; i < postProcessingPipeline_.size(); ++i)
	{
		applyPostProcShaderParams(postProcessingPipeline_[i].first,
		                          postProcessingPipeline_[i].second);
		auto texs = getPostProcessingUniformTextures(
		    postProcessingPipeline_[i].first,
		    postProcessingPipeline_[i].second);
		GLHandler::postProcess(
		    postProcessingPipeline_[i].second,
		    vrHandler.getPostProcessingTarget(i % 2, side),
		    vrHandler.getPostProcessingTarget((i + 1) % 2, side), texs);
	}

	vrHandler.submitRendering(side, i % 2);
}

void AbstractMainWin::paintGL()
{
	m_context.makeCurrent(this);
	if(!initialized)
	{
		initializeGL();
	}

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
	for(auto const& pair : sceneRenderPipeline_)
	{
		updateScene(*pair.second.camera, pair.first);
	}
	PythonQtHandler::evalScript(
	    "if \"updateScene\" in dir():\n\tupdateScene()");

	bool debug(dbgCamera->isEnabled());
	bool debugInHeadset(dbgCamera->debugInHeadset());
	bool renderingCamIsDebug(debug
	                         && ((debugInHeadset && vrHandler) || !vrHandler));
	bool thirdRender(QSettings().value("vr/thirdrender").toBool());

	// main render logic
	if(vrHandler)
	{
		vrHandler.prepareRendering();

		vrRender(Side::LEFT, debug, debugInHeadset);
		vrRender(Side::RIGHT, debug, debugInHeadset);

		if(!thirdRender && (!debug || debugInHeadset))
		{
			vrHandler.displayOnCompanion(width(), height());
		}
		else if(debug && !debugInHeadset)
		{
			renderingCamIsDebug = true;
		}
	}
	// if no VR or debug not in headset, render 2D
	if((!vrHandler || thirdRender) || (debug && !debugInHeadset))
	{
		if(postProcessingPipeline_.empty())
		{
			GLHandler::beginRendering();
		}
		else
		{
			GLHandler::beginRendering(postProcessingTargets[0]);
		}

		for(auto pair : sceneRenderPipeline_)
		{
			pair.second.camera->update2D();
			dbgCamera->update();
			if(renderingCamIsDebug)
			{
				dbgCamera->uploadMatrices();
			}
			else
			{
				pair.second.camera->uploadMatrices();
			}
			// render scene
			if(wireframe)
			{
				GLHandler::beginWireframe();
			}
			renderScene(*pair.second.camera, pair.first);
			PythonQtHandler::evalScript(
			    "if \"renderScene\" in dir():\n\trenderScene()");
			if(debug)
			{
				dbgCamera->renderCamera(pair.second.camera);
			}
			if(wireframe)
			{
				GLHandler::endWireframe();
			}
		}

		// do all postprocesses except last one
		for(int i(0); i < postProcessingPipeline_.size() - 1; ++i)
		{
			applyPostProcShaderParams(postProcessingPipeline_[i].first,
			                          postProcessingPipeline_[i].second);
			auto texs = getPostProcessingUniformTextures(
			    postProcessingPipeline_[i].first,
			    postProcessingPipeline_[i].second);
			GLHandler::postProcess(postProcessingPipeline_[i].second,
			                       postProcessingTargets.at(i % 2),
			                       postProcessingTargets.at((i + 1) % 2), texs);
		}
		// render last one on screen target
		if(!postProcessingPipeline_.empty())
		{
			int i = postProcessingPipeline_.size() - 1;
			applyPostProcShaderParams(postProcessingPipeline_[i].first,
			                          postProcessingPipeline_[i].second);
			auto texs = getPostProcessingUniformTextures(
			    postProcessingPipeline_[i].first,
			    postProcessingPipeline_[i].second);
			GLHandler::postProcess(postProcessingPipeline_[i].second,
			                       postProcessingTargets.at(i % 2),
			                       GLHandler::getScreenRenderTarget(), texs);
		}
	}

	// garbage collect some resources
	AsyncTexture::garbageCollect();
	AsyncMesh::garbageCollect();

	// Trigger a repaint immediatly
	m_context.swapBuffers(this);
	// animate continuously: schedule an update
	QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
}

void AbstractMainWin::resizeGL(int w, int h)
{
	QSettings().setValue("window/width", w);
	QSettings().setValue("window/height", h);
	for(auto pair : sceneRenderPipeline_)
	{
		pair.second.camera->setPerspectiveProj(
		    70.0f, static_cast<float>(width()) / static_cast<float>(height()));
	}
	dbgCamera->setPerspectiveProj(70.0f, static_cast<float>(width())
	                                         / static_cast<float>(height()));
}

AbstractMainWin::~AbstractMainWin()
{
	// force garbage collect some resources
	AsyncTexture::garbageCollect(true);
	AsyncMesh::garbageCollect(true);

	PythonQtHandler::evalScript(
	    "if \"cleanUpScene\" in dir():\n\tcleanUpScene()");
	for(const QPair<QString, GLHandler::ShaderProgram>& p :
	    postProcessingPipeline_)
	{
		GLHandler::deleteShader(p.second);
	}
	for(auto const& pair : sceneRenderPipeline_)
	{
		delete pair.second.camera;
	}
	delete dbgCamera;

	GLHandler::deleteRenderTarget(postProcessingTargets[0]);
	GLHandler::deleteRenderTarget(postProcessingTargets[1]);

	vrHandler.close();
	PythonQtHandler::clean();
}
