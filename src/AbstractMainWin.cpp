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
}

void AbstractMainWin::setCamera(BasicCamera* newCamera)
{
	delete camera;
	camera = newCamera;
}

void AbstractMainWin::keyPressEvent(QKeyEvent* event)
{
	if(event->key() == Qt::Key_F1)
	{
		QSettings().setValue(
		    "debugcamera/enabled",
		    !QSettings().value("debugcamera/enabled").toBool());
	}
	if(event->key() == Qt::Key_F11)
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
	else if(event->key() == Qt::Key_Escape)
	{
		close();
	}
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

	// let user init
	initScene();

	frameTimer.start();
}

void AbstractMainWin::paintGL()
{
	frameTiming_ = frameTimer.restart() / 1000.f;

	setTitle(QString(PROJECT_NAME) + " - "
	         + QString::number(1.f / frameTiming) + " FPS");

	if(vrHandler)
		frameTiming_ = vrHandler.getFrameTiming() / 1000.f;

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

		vrHandler.beginRendering(Side::LEFT);
		camera->update();
		dbgCamera->update();
		renderingCam->uploadMatrices();
		vrHandler.renderControllers();
		vrHandler.renderHands();
		// render scene
		renderScene(*camera);
		if(debug && debugInHeadset)
			dbgCamera->renderCamera(camera);
		vrHandler.submitRendering(Side::LEFT);

		vrHandler.beginRendering(Side::RIGHT);
		camera->update();
		dbgCamera->update();
		renderingCam->uploadMatrices();
		vrHandler.renderControllers();
		vrHandler.renderHands();
		// render scene
		renderScene(*camera);
		if(debug && debugInHeadset)
			dbgCamera->renderCamera(camera);
		vrHandler.submitRendering(Side::RIGHT);

		if(!debug || debugInHeadset)
			vrHandler.displayOnCompanion(width(), height());
		else
			renderingCam = dbgCamera;
	}
	// if no VR or debug not in headset, render 2D
	if(!vrHandler || (debug && !debugInHeadset))
	{
		GLHandler::beginRendering();
		camera->update();
		dbgCamera->update();
		renderingCam->uploadMatrices();
		// render scene
		renderScene(*camera);
		if(debug)
			dbgCamera->renderCamera(camera);
	}

	// Trigger a repaint immediatly
	update();
}

AbstractMainWin::~AbstractMainWin()
{
	delete camera;
	delete dbgCamera;
}
