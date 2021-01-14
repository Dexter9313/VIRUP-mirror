#include "MainWin.hpp"

void MainWin::actionEvent(BaseInputManager::Action a, bool pressed)
{
	if(!pressed)
	{
		AbstractMainWin::actionEvent(a, pressed);
		return;
	}

	if(a.id == "barrelup")
	{
		barrelPower = 1.f + (barrelPower - 1.f) * 1.2f;
	}
	else if(a.id == "barreldown")
	{
		barrelPower = 1.f + (barrelPower - 1.f) / 1.2f;
	}
	else if(a.id == "togglevrorigin")
	{
		renderer.getCamera("default").seatedVROrigin
		    = !renderer.getCamera("default").seatedVROrigin;
	}
	AbstractMainWin::actionEvent(a, pressed);
}

void MainWin::mousePressEvent(QMouseEvent* e)
{
	if(e->button() == Qt::MouseButton::LeftButton)
	{
		moveView = true;
		QCursor c(cursor());
		c.setShape(Qt::CursorShape::BlankCursor);
		cursorPosBackup = QCursor::pos();
		QCursor::setPos(width() / 2, height() / 2);
		setCursor(c);
	}
}

void MainWin::mouseReleaseEvent(QMouseEvent* e)
{
	if(e->button() == Qt::MouseButton::LeftButton)
	{
		moveView = false;
		QCursor c(cursor());
		c.setShape(Qt::CursorShape::ArrowCursor);
		QCursor::setPos(cursorPosBackup);
		setCursor(c);
	}
}

void MainWin::mouseMoveEvent(QMouseEvent* e)
{
	if(!isActive() || /*vrHandler->isEnabled() ||*/ !moveView)
	{
		return;
	}
	float dx = (static_cast<float>(width()) / 2 - e->globalX()) / width();
	float dy = (static_cast<float>(height()) / 2 - e->globalY()) / height();
	yaw += dx * 3.14f / 3.f;
	pitch += dy * 3.14f / 3.f;
	QCursor::setPos(width() / 2, height() / 2);
}

void MainWin::initScene()
{
	// SKYBOX
	sbShader.load("skybox");
	skybox = new GLMesh;
	Primitives::setAsUnitCube(*skybox, sbShader.toGLShaderProgram());

	std::array<const char*, 6> paths = {};
	paths.at(static_cast<unsigned int>(GLTexture::CubemapFace::BACK))
	    = "data/example/images/ame_ash/ashcanyon_bk.tga";
	paths.at(static_cast<unsigned int>(GLTexture::CubemapFace::BOTTOM))
	    = "data/example/images/ame_ash/ashcanyon_dn.tga";
	paths.at(static_cast<unsigned int>(GLTexture::CubemapFace::FRONT))
	    = "data/example/images/ame_ash/ashcanyon_ft.tga";
	paths.at(static_cast<unsigned int>(GLTexture::CubemapFace::LEFT))
	    = "data/example/images/ame_ash/ashcanyon_lf.tga";
	paths.at(static_cast<unsigned int>(GLTexture::CubemapFace::RIGHT))
	    = "data/example/images/ame_ash/ashcanyon_rt.tga";
	paths.at(static_cast<unsigned int>(GLTexture::CubemapFace::TOP))
	    = "data/example/images/ame_ash/ashcanyon_up.tga";
	sbTexture = new GLTexture(paths);

	shaderProgram.load("colorpervert");
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	std::vector<float> vertices = {
	    0.5f,  0.5f,  0.0f, // top right
	    0.5f,  -0.5f, 0.0f, // bottom right
	    -0.5f, -0.5f, 0.0f, // bottom left
	    -0.5f, 0.5f,  0.0f, // top left
	};
	/*std::vector<float> vertices = {
	    0.5f,  0.5f,  0.0f, 1.f, 0.f, 0.f, // top right
	    0.5f,  -0.5f, 0.0f, 0.f, 1.f, 0.f, // bottom right
	    -0.5f, -0.5f, 0.0f, 0.f, 0.f, 1.f, // bottom left
	    -0.5f, 0.5f,  0.0f, 0.f, 0.f, 0.f, // top left
	};*/
	std::vector<unsigned int> indices = {
	    // note that we start from 0!
	    0, 1, 3, // first Triangle
	    1, 2, 3  // second Triangle
	};
	mesh = new GLMesh;
	mesh->setVertexShaderMapping(shaderProgram.toGLShaderProgram(),
	                             {{"position", 3}});
	mesh->setVertices(vertices, indices);
	shaderProgram.toGLShaderProgram().setUnusedAttributesValues(
	    {{"color", {1.0, 1.0, 0.0}}});

	// create cube
	movingCube = new MovingCube;

	// create points
	pointsMesh = new GLMesh;
	pointsShader.load("default");
	pointsShader.setUniform("alpha", 1.0f);
	pointsShader.setUniform("color", QColor::fromRgbF(1.0f, 1.0f, 1.0f));
	std::vector<float> points = {0, 0, 0};
	pointsMesh->setVertexShaderMapping(pointsShader.toGLShaderProgram(),
	                                   {{"position", 3}});
	pointsMesh->setVertices(points);

	sphereShader.load("default");
	sphereShader.setUniform("alpha", 1.0f);
	sphereShader.setUniform("color", QColor::fromRgbF(0.5f, 0.5f, 1.0f));
	sphere = new GLMesh;
	Primitives::setAsUnitSphere(*sphere, sphereShader.toGLShaderProgram(), 100,
	                            100);

	playareaShader.load("default");
	playareaShader.setUniform("color", QColor(255, 0, 0));
	playareaShader.setUniform("alpha", 1.f);
	playarea = new GLMesh;
	if(vrHandler->isEnabled())
	{
		auto playareaquad(vrHandler->getPlayAreaQuad());
		vertices = {
		    playareaquad[0].x(), playareaquad[0].y(), playareaquad[0].z(),
		    playareaquad[1].x(), playareaquad[1].y(), playareaquad[1].z(),
		    playareaquad[2].x(), playareaquad[2].y(), playareaquad[2].z(),
		    playareaquad[3].x(), playareaquad[3].y(), playareaquad[3].z(),
		};
		indices = {0, 1, 1, 2, 2, 3, 3, 0};
		playarea->setVertexShaderMapping(playareaShader.toGLShaderProgram(),
		                                 {{"position", 3}});
		playarea->setVertices(vertices, indices);
	}

	model                = new Model("models/drone/scene.gltf");
	light                = new Light;
	light->ambiantFactor = 0.05f;

	bill           = new Billboard("data/example/images/cc.png");
	bill->position = QVector3D(0.f, 0.f, 0.8f);

	text = new Text3D(200, 40);
	text->setText(tr("Hello World !\nLet's draw some text !"));
	text->setColor(QColor(0, 0, 0, 255));
	text->setBackgroundColor(QColor(255, 0, 0, 127));
	text->setRectangle(QRect(50, 0, 150, 40));
	text->setSuperSampling(2.f);
	text->setFlags(Qt::AlignRight);

	text->getModel().rotate(135.f, 0.f, 0.f, 1.f);
	text->getModel().rotate(45.f, 1.f, 0.f);
	text->getModel().translate(-0.6f, 0.f, 0.5f);

	widget3d = new Widget3D(new QCalendarWidget);

	widget3d->getModel().rotate(135.f, 0.f, 0.f, 1.f);
	widget3d->getModel().rotate(45.f, 1.f, 0.f);
	widget3d->getModel().translate(0.6f, 0.f, 0.5f);

	renderer.getCamera("default").setEyeDistanceFactor(1.0f);

	renderer.appendPostProcessingShader("distort", "distort");

	timer.start();
}

void MainWin::updateScene(BasicCamera& camera, QString const& /*pathId*/)
{
	QVector3D lookDir(-cosf(yaw) * cosf(pitch), -sinf(yaw) * cosf(pitch),
	                  sinf(pitch));
	camera.setView({1, 1, 1}, lookDir, {0, 0, 1});

	Controller const* cont(vrHandler->getController(Side::LEFT));
	if(cont == nullptr)
	{
		cont = vrHandler->getController(Side::RIGHT);
	}
	if(cont != nullptr)
	{
		if(cont->getTriggerValue() > 0.5)
		{
			QVector4D pos(camera.seatedTrackedSpaceToWorldTransform()
			              * cont->getPosition());
			std::vector<float> points(3);
			points[0] = pos[0];
			points[1] = pos[1];
			points[2] = pos[2];
			pointsMesh->setVertices(points);
		}
	}

	Hand const* leftHand(vrHandler->getHand(Side::LEFT));
	if(leftHand != nullptr)
	{
		if(leftHand->isClosed())
		{
			QVector4D pos(camera.hmdSpaceToWorldTransform()
			              * leftHand->palmPosition());
			std::vector<float> points(3);
			points[0] = pos[0];
			points[1] = pos[1];
			points[2] = pos[2];
			pointsMesh->setVertices(points);
		}
	}

	movingCube->update();

	modelModel = QMatrix4x4();
	modelModel.scale(0.5 / model->getBoundingSphereRadius());
	float secs(timer.elapsed() / 5000.f);
	light->color
	    = QColor(128 + 127 * cos(secs / 2.0), 128 + 127 * sin(secs / 2.0), 0);
	light->color = QColor(255, 255, 255);
	if(vrHandler->isEnabled())
	{
		modelModel.translate(0.f, 1.4f * model->getBoundingSphereRadius(), 0.f);
		modelModel.rotate(180.f, QVector3D(0.f, 1.f, 0.f));
		// light->direction = QVector3D(cos(secs), 0.f, sin(secs));
	}
	else
	{
		modelModel.translate(0.f, 0.f, -50.f);
		modelModel.rotate(180.f, QVector3D(0.f, 0.f, 1.f));
		modelModel.rotate(120.f, QVector3D(1.f, 1.f, 1.f).normalized());
		modelModel.scale(0.3);
		// light->direction = QVector3D(sin(secs), cos(secs), 0.f);
	}
	modelModel.rotate(100.f * secs, QVector3D(0.f, 1.f, 0.f));
	model->generateShadowMap(modelModel, *light);
}

void MainWin::renderScene(BasicCamera const& camera, QString const& /*pathId*/)
{
	GLHandler::useTextures({sbTexture});
	GLHandler::setBackfaceCulling(false);
	GLHandler::setUpRender(sbShader.toGLShaderProgram(), QMatrix4x4(),
	                       GLHandler::GeometricSpace::SKYBOX);
	skybox->render(PrimitiveType::TRIANGLE_STRIP);
	GLHandler::setBackfaceCulling(true);
	GLHandler::clearDepthBuffer();

	QMatrix4x4 modelSphere;
	modelSphere.translate(-1.5, 0, 0);
	GLHandler::setUpRender(sphereShader.toGLShaderProgram(), modelSphere,
	                       GLHandler::GeometricSpace::SKYBOX);
	sphere->render();
	GLHandler::clearDepthBuffer();

	movingCube->render();

	GLHandler::setUpRender(shaderProgram.toGLShaderProgram());
	mesh->render();
	GLHandler::setUpRender(pointsShader.toGLShaderProgram());
	GLHandler::setPointSize(8);
	pointsMesh->render();
	GLHandler::setPointSize(1);

	GLHandler::setUpRender(playareaShader.toGLShaderProgram(), QMatrix4x4(),
	                       GLHandler::GeometricSpace::STANDINGTRACKED);
	playarea->render(PrimitiveType::LINES);

	if(vrHandler->isEnabled())
	{
		model->render(camera.standingTrackedSpaceToWorldTransform().inverted()
		                  * camera.getWorldSpacePosition(),
		              modelModel, *light,
		              GLHandler::GeometricSpace::STANDINGTRACKED);
	}
	else
	{
		model->render(camera.getWorldSpacePosition(), modelModel, *light);
	}

	widget3d->render();
	bill->render(camera);
	text->render();
}

void MainWin::applyPostProcShaderParams(
    QString const& id, GLShaderProgram const& shader,
    GLFramebufferObject const& currentTarget) const
{
	AbstractMainWin::applyPostProcShaderParams(id, shader, currentTarget);
	if(id == "distort")
	{
		shader.setUniform("BarrelPower", barrelPower);
	}
}

MainWin::~MainWin()
{
	delete sbTexture;
	delete skybox;

	delete mesh;

	delete pointsMesh;

	delete playarea;

	delete movingCube;

	delete model;
	delete light;

	delete bill;
	delete text;
	delete widget3d;
}
