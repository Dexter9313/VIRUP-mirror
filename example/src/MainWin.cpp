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
		getCamera("default").seatedVROrigin
		    = !getCamera("default").seatedVROrigin;
	}
	AbstractMainWin::actionEvent(a, pressed);
}

void MainWin::initScene()
{
	// SKYBOX
	sbShader = GLHandler::newShader("skybox");
	skybox   = Primitives::newUnitCube(sbShader);

	std::array<const char*, 6> paths = {};
	paths.at(static_cast<unsigned int>(GLHandler::CubeFace::BACK))
	    = "data/example/images/ame_ash/ashcanyon_bk.tga";
	paths.at(static_cast<unsigned int>(GLHandler::CubeFace::BOTTOM))
	    = "data/example/images/ame_ash/ashcanyon_dn.tga";
	paths.at(static_cast<unsigned int>(GLHandler::CubeFace::FRONT))
	    = "data/example/images/ame_ash/ashcanyon_ft.tga";
	paths.at(static_cast<unsigned int>(GLHandler::CubeFace::LEFT))
	    = "data/example/images/ame_ash/ashcanyon_lf.tga";
	paths.at(static_cast<unsigned int>(GLHandler::CubeFace::RIGHT))
	    = "data/example/images/ame_ash/ashcanyon_rt.tga";
	paths.at(static_cast<unsigned int>(GLHandler::CubeFace::TOP))
	    = "data/example/images/ame_ash/ashcanyon_up.tga";
	sbTexture = GLHandler::newTexture(paths);

	shaderProgram = GLHandler::newShader("colorpervert");
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
	mesh = GLHandler::newMesh();
	GLHandler::setVertices(mesh, vertices, shaderProgram, {{"position", 3}},
	                       indices);
	GLHandler::setShaderUnusedAttributesValues(shaderProgram,
	                                           {{"color", {1.0, 1.0, 0.0}}});

	// create cube
	movingCube = new MovingCube;

	// create points
	pointsMesh   = GLHandler::newMesh();
	pointsShader = GLHandler::newShader("default");
	GLHandler::setShaderParam(pointsShader, "alpha", 1.0f);
	GLHandler::setShaderParam(pointsShader, "color",
	                          QColor::fromRgbF(1.0f, 1.0f, 1.0f));
	std::vector<float> points = {0, 0, 0};
	GLHandler::setVertices(pointsMesh, points, pointsShader, {{"position", 3}});

	sphereShader = GLHandler::newShader("default");
	GLHandler::setShaderParam(sphereShader, "alpha", 1.0f);
	GLHandler::setShaderParam(sphereShader, "color",
	                          QColor::fromRgbF(0.5f, 0.5f, 1.0f));
	sphere = Primitives::newUnitSphere(sphereShader, 100, 100);

	playareaShader = GLHandler::newShader("default");
	GLHandler::setShaderParam(playareaShader, "color", QColor(255, 0, 0));
	GLHandler::setShaderParam(playareaShader, "alpha", 1.f);
	playarea = GLHandler::newMesh();
	if(vrHandler)
	{
		auto playareaquad(vrHandler.getPlayAreaQuad());
		vertices = {
		    playareaquad[0].x(), playareaquad[0].y(), playareaquad[0].z(),
		    playareaquad[1].x(), playareaquad[1].y(), playareaquad[1].z(),
		    playareaquad[2].x(), playareaquad[2].y(), playareaquad[2].z(),
		    playareaquad[3].x(), playareaquad[3].y(), playareaquad[3].z(),
		};
		indices = {0, 1, 1, 2, 2, 3, 3, 0};
		GLHandler::setVertices(playarea, vertices, playareaShader,
		                       {{"position", 3}}, indices);
	}

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

	getCamera("default").setEyeDistanceFactor(5.0f);

	appendPostProcessingShader("distort", "distort");
}

void MainWin::updateScene(BasicCamera& camera, QString const& /*pathId*/)
{
	Controller const* cont(vrHandler.getController(Side::LEFT));
	if(cont == nullptr)
	{
		cont = vrHandler.getController(Side::RIGHT);
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
			GLHandler::updateVertices(pointsMesh, points);
		}
	}

	Hand const* leftHand(vrHandler.getHand(Side::LEFT));
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
			GLHandler::updateVertices(pointsMesh, points);
		}
	}

	movingCube->update();
}

void MainWin::renderScene(BasicCamera const& camera, QString const& /*pathId*/)
{
	GLHandler::useTextures({sbTexture});
	GLHandler::setBackfaceCulling(false);
	GLHandler::setUpRender(sbShader, QMatrix4x4(),
	                       GLHandler::GeometricSpace::SKYBOX);
	GLHandler::render(skybox, GLHandler::PrimitiveType::TRIANGLE_STRIP);
	GLHandler::setBackfaceCulling(true);
	GLHandler::clearDepthBuffer();
	if(vrHandler)
	{
		vrHandler.applyHiddenAreaDepth(vrHandler.getCurrentRenderingEye());
	}

	QMatrix4x4 model;
	model.translate(-1.5, 0, 0);
	GLHandler::setUpRender(sphereShader, model,
	                       GLHandler::GeometricSpace::SKYBOX);
	GLHandler::render(sphere);
	GLHandler::clearDepthBuffer();
	if(vrHandler)
	{
		vrHandler.applyHiddenAreaDepth(vrHandler.getCurrentRenderingEye());
	}

	movingCube->render();

	GLHandler::setUpRender(shaderProgram);
	GLHandler::render(mesh);
	GLHandler::setUpRender(pointsShader);
	GLHandler::setPointSize(8);
	GLHandler::render(pointsMesh);
	GLHandler::setPointSize(1);

	GLHandler::setUpRender(playareaShader, QMatrix4x4(),
	                       GLHandler::GeometricSpace::STANDINGTRACKED);
	GLHandler::render(playarea, GLHandler::PrimitiveType::LINES);

	widget3d->render();
	bill->render(camera);
	text->render();
}

void MainWin::applyPostProcShaderParams(QString const& id,
                                        GLHandler::ShaderProgram shader) const
{
	AbstractMainWin::applyPostProcShaderParams(id, shader);
	if(id == "distort")
	{
		GLHandler::setShaderParam(shader, "BarrelPower", barrelPower);
	}
}

MainWin::~MainWin()
{
	GLHandler::deleteTexture(sbTexture);
	GLHandler::deleteMesh(skybox);
	GLHandler::deleteShader(sbShader);

	GLHandler::deleteMesh(mesh);

	GLHandler::deleteMesh(pointsMesh);
	GLHandler::deleteShader(pointsShader);

	GLHandler::deleteMesh(playarea);
	GLHandler::deleteShader(playareaShader);

	delete movingCube;

	delete bill;
	delete text;
	delete widget3d;
}
