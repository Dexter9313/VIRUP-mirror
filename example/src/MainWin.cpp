#include "MainWin.hpp"

std::vector<float> cubeVertices(uint64_t dt)
{
	float dtf = dt / 2000.f;

	std::vector<float> result = {
	    -1.0f, -1.0f, -1.0f, // 0
	    -1.0f, -1.0f, 1.0f,  // 1
	    -1.0f, 1.0f,  -1.0f, // 2
	    -1.0f, 1.0f,  1.0f,  // 3
	    1.0f,  -1.0f, -1.0f, // 4
	    1.0f,  -1.0f, 1.0f,  // 5
	    1.0f,  1.0f,  -1.0f, // 6
	    1.0f,  1.0f,  1.0f,  // 7
	};

	for(unsigned int i(0); i < 24; i += 3)
	{
		result[i] *= cosf(dtf);
	}
	for(unsigned int i(1); i < 24; i += 3)
	{
		result[i] *= sinf(dtf);
	}

	return result;
}

GLHandler::Mesh createCube(GLHandler::ShaderProgram const& shader)
{
	GLHandler::Mesh mesh(GLHandler::newMesh());
	std::vector<float> vertices = cubeVertices(0);

	std::vector<unsigned int> elements = {
	    0, 1, 0, 2, 0, 4,

	    7, 6, 7, 5, 7, 3,

	    1, 3, 1, 5,

	    2, 6, 2, 3,

	    4, 6, 4, 5,
	};
	GLHandler::setVertices(mesh, vertices, shader, {{"position", 3}}, elements);
	return mesh;
}

void MainWin::keyPressEvent(QKeyEvent* e)
{
	AbstractMainWin::keyPressEvent(e);
	if(e->key() == Qt::Key_PageUp)
	{
		barrelPower = 1.f + (barrelPower - 1.f) * 1.2f;
	}
	else
	{
		barrelPower = 1.f + (barrelPower - 1.f) / 1.2f;
	}
}

void MainWin::initScene()
{
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
	cubeShader = GLHandler::newShader("default");
	GLHandler::setShaderParam(cubeShader, "alpha", 0.5f);
	GLHandler::setShaderParam(cubeShader, "color",
	                          QColor::fromRgbF(1.0f, 1.0f, 1.0f));
	cube = createCube(cubeShader);

	// create points
	pointsMesh   = GLHandler::newMesh();
	pointsShader = GLHandler::newShader("default");
	GLHandler::setShaderParam(pointsShader, "alpha", 1.0f);
	GLHandler::setShaderParam(pointsShader, "color",
	                          QColor::fromRgbF(1.0f, 1.0f, 1.0f));
	std::vector<float> points = {0, 0, 0};
	GLHandler::setVertices(pointsMesh, points, pointsShader, {{"position", 3}});
	cubeTimer.start();

	sphereShader = GLHandler::newShader("default");
	GLHandler::setShaderParam(sphereShader, "alpha", 1.0f);
	GLHandler::setShaderParam(sphereShader, "color",
	                          QColor::fromRgbF(0.5f, 0.5f, 1.0f));
	sphere = Primitives::newUnitSphere(sphereShader, 100, 100);

	bill           = new Billboard("data/example/images/cc.png");
	bill->position = QVector3D(0.f, 0.f, 0.8f);

	text = new Text3D(200, 40);
	text->setText("Hello World !\nLet's draw some text !");
	text->setColor(QColor(0, 0, 0, 255));
	text->setBackgroundColor(QColor(255, 0, 0, 127));
	text->setRectangle(QRect(50, 0, 150, 40));
	text->setSuperSampling(2.f);
	text->setFlags(Qt::AlignRight);

	text->getModel().rotate(135.f, 0.f, 0.f, 1.f);
	text->getModel().rotate(45.f, 1.f, 0.f);
	text->getModel().translate(0.f, 0.f, 0.5f);

	getCamera().setEyeDistanceFactor(5.0f);

	appendPostProcessingShader("distort", "distort");
}

void MainWin::updateScene(BasicCamera& camera)
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
			QVector4D pos(camera.trackedSpaceToWorldTransform()
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

	GLHandler::updateVertices(cube, cubeVertices(cubeTimer.elapsed()));
}

void MainWin::renderScene(BasicCamera const& camera)
{
	Q_UNUSED(camera);
	QMatrix4x4 model;
	model.translate(-1.5, 0, 0);
	GLHandler::setUpRender(sphereShader, model,
	                       GLHandler::GeometricSpace::SKYBOX);
	GLHandler::render(sphere);
	GLHandler::clearDepthBuffer();

	GLHandler::setUpRender(cubeShader);
	GLHandler::render(cube, GLHandler::PrimitiveType::LINES);
	GLHandler::setUpRender(shaderProgram);
	GLHandler::render(mesh);
	GLHandler::setUpRender(pointsShader);
	GLHandler::setPointSize(8);
	GLHandler::render(pointsMesh);
	GLHandler::setPointSize(1);

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
	GLHandler::deleteMesh(mesh);

	GLHandler::deleteMesh(pointsMesh);
	GLHandler::deleteShader(pointsShader);

	GLHandler::deleteMesh(cube);
	GLHandler::deleteShader(cubeShader);

	delete bill;
	delete text;
}
