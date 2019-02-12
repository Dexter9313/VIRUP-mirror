#include "MainWin.hpp"

MainWin::MainWin()
    : AbstractMainWin()
{
}

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
		result[i] *= cos(dtf);
	for(unsigned int i(1); i < 24; i += 3)
		result[i] *= sin(dtf);

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
		barrelPower = 1.f + (barrelPower - 1.f) * 1.2f;
	else
		barrelPower = 1.f + (barrelPower - 1.f) / 1.2f;
}

void MainWin::initScene()
{
	shaderProgram = GLHandler::newShader("default");
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	GLHandler::setShaderParam(shaderProgram, "alpha", 1.0f);
	GLHandler::setShaderParam(shaderProgram, "color", QColor::fromRgbF(1.0f, 0.4f, 0.3f));

	std::vector<float> vertices = {
	    0.5f,  0.5f,  0.0f, // top right
	    0.5f,  -0.5f, 0.0f, // bottom right
	    -0.5f, -0.5f, 0.0f, // bottom left
	    -0.5f, 0.5f,  0.0f  // top left
	};
	std::vector<unsigned int> indices = {
	    // note that we start from 0!
	    0, 1, 3, // first Triangle
	    1, 2, 3  // second Triangle
	};
	mesh = GLHandler::newMesh();
	GLHandler::setVertices(mesh, vertices, shaderProgram, {{"position", 3}},
	                       indices);

	// create cube
	cubeShader = GLHandler::newShader("default");
	GLHandler::setShaderParam(cubeShader, "alpha", 0.5f);
	GLHandler::setShaderParam(cubeShader, "color", QColor::fromRgbF(1.0f, 1.0f, 1.0f));
	cube = createCube(cubeShader);

	// create points
	pointsMesh   = GLHandler::newMesh();
	pointsShader = GLHandler::newShader("default");
	GLHandler::setShaderParam(pointsShader, "alpha", 1.0f);
	GLHandler::setShaderParam(pointsShader, "color", QColor::fromRgbF(1.0f, 1.0f, 1.0f));
	std::vector<float> points = {0, 0, 0};
	GLHandler::setVertices(pointsMesh, points, pointsShader, {{"position", 3}});
	cubeTimer.start();

	getCamera().setEyeDistanceFactor(5.0f);

	appendPostProcessingShader("distort", "distort");
}

void MainWin::updateScene(BasicCamera& camera)
{
	Controller const* cont(vrHandler.getController(Side::LEFT));
	if(!cont)
		cont = vrHandler.getController(Side::RIGHT);
	if(cont)
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
	if(leftHand)
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
	GLHandler::setUpRender(cubeShader);
	GLHandler::render(cube, GLHandler::PrimitiveType::LINES);
	GLHandler::setUpRender(shaderProgram);
	GLHandler::render(mesh);
	GLHandler::setUpRender(pointsShader);
	GLHandler::setPointSize(8);
	GLHandler::render(pointsMesh);
}

void MainWin::applyPostProcShaderParams(QString const& id, GLHandler::ShaderProgram shader) const
{
	AbstractMainWin::applyPostProcShaderParams(id, shader);
	if(id == "distort")
		GLHandler::setShaderParam(shader, "BarrelPower", barrelPower);
}

MainWin::~MainWin()
{
	GLHandler::deleteMesh(mesh);

	GLHandler::deleteMesh(pointsMesh);
	GLHandler::deleteShader(pointsShader);

	GLHandler::deleteMesh(cube);
	GLHandler::deleteShader(cubeShader);
}
