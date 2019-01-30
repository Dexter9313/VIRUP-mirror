#include "MainWin.hpp"

MainWin::MainWin()
    : AbstractMainWin()
{
}

void MainWin::keyPressEvent(QKeyEvent* e)
{
	Camera& cam(static_cast<Camera&>(getCamera()));
	if(e->key() == Qt::Key_Left)
		cam.angle -= 3.14f / 30.0f;
	else if(e->key() == Qt::Key_Right)
		cam.angle += 3.14f / 30.0f;
	else if(e->key() == Qt::Key_Up)
		cam.distance /= 1.2;
	else if(e->key() == Qt::Key_Down)
		cam.distance *= 1.2;
	else if(e->key() == Qt::Key_PageUp)
		method->setAlpha(method->getAlpha() * 21 / 20);
	else if(e->key() == Qt::Key_PageDown)
		method->setAlpha(method->getAlpha() * 20 / 21);
	else if(e->key() == Qt::Key_Home)
	{
		// integralDt    = 0;
		cam.angle    = 0;
		cam.distance = 0.01;
		if(vrHandler)
			vrHandler.resetPos();
	}
	else if(e->key() == Qt::Key_D)
		method->showdm = !method->showdm;
	else if(e->key() == Qt::Key_C)
		showCube = !showCube;

	AbstractMainWin::keyPressEvent(e);
}

void MainWin::vrEvent(VRHandler::Event const& e)
{
	Camera& cam(static_cast<Camera&>(getCamera()));
	switch(e.type)
	{
		case VRHandler::EventType::BUTTON_PRESSED:
			switch(e.button)
			{
				case VRHandler::Button::GRIP:
					method->showdm = !method->showdm;
					break;
				case VRHandler::Button::TOUCHPAD:
				{
					Controller const* ctrl(vrHandler.getController(e.side));
					if(ctrl)
					{
						QVector2D padCoords(ctrl->getPadCoords());
						if(padCoords.length() < 0.5) // CENTER
						{
							if(e.side == Side::RIGHT)
							{
								cam.distance = 1.0f;
								cam.angle    = 0.0f;
								vrHandler.resetPos();
							}
							else if(e.side == Side::LEFT)
							{
								method->resetAlpha();
								cam.setEyeDistanceFactor(1.0f);
							}
						}
						else if(fabs(padCoords[0])
						        > fabs(padCoords[1])) // LEFT OR
						                              // RIGHT
						{
							if(padCoords[0] < 0.0f) // LEFT
							{
								if(e.side == Side::RIGHT)
									cam.angle -= 3.14f / 30.0f;
								else if(e.side == Side::LEFT)
									method->setAlpha(method->getAlpha() * 20
									                 / 21);
							}
							else // RIGHT
							{
								if(e.side == Side::RIGHT)
									cam.angle += 3.14f / 30.0f;
								else if(e.side == Side::LEFT)
									method->setAlpha(method->getAlpha() * 21
									                 / 20);
							}
						}
						else // DOWN OR UP
						{
							if(padCoords[1] < 0.0f) // DOWN
							{
								if(e.side == Side::RIGHT)
									cam.distance *= 1.2;
								else if(e.side == Side::LEFT)
									cam.setEyeDistanceFactor(
									    cam.getEyeDistanceFactor() * 1.2);
							}
							else // UP
							{
								if(e.side == Side::RIGHT)
									cam.distance /= 1.2;
								else if(e.side == Side::LEFT)
									cam.setEyeDistanceFactor(
									    cam.getEyeDistanceFactor() / 1.2f);
							}
						}
					}
					break;
				}
				case VRHandler::Button::TRIGGER:
					std::cout << "Trigger Pressed on controller "
					          << (unsigned int) e.side << " !" << std::endl;
					break;
				case VRHandler::Button::MENU:
					std::cout << "Menu Pressed on controller "
					          << (unsigned int) e.side << " !" << std::endl;
					break;
				default:
					std::cout << "Button (?) Pressed on controller "
					          << (unsigned int) e.side << " !" << std::endl;
			}
			break;
		default:
			break;
	}

	AbstractMainWin::vrEvent(e);
}

void MainWin::initScene()
{
	QStringList argv = QCoreApplication::arguments();
	int argc = argv.size();
	unsigned int numberOfVertices(500000), seed(time(NULL));
	QString methodStr("");

	if(argc > 1)
		methodStr = argv[1];

	if(methodStr == "--base")
		method = new BaseLineMethod();
	else if(methodStr == "--basetex")
		method = new BaseLineMethodTex();
	else if(methodStr == "--treetex")
		method = new TreeMethodTex();
	else // if(methodStr == "--treelod")
		method = new TreeMethodLOD();

	std::cout << "Method : " << method->getName() << std::endl;
	std::cout << "Seed : " << seed << std::endl;

	if(argc > 3)
		seed = argv[3].toInt();
	if(argc > 2)
	{
		std::vector<float> vertices[3];
		numberOfVertices = argv[2].toInt();
		vertices[0]      = generateVertices(numberOfVertices / 3, seed);
		vertices[1]      = generateVertices(numberOfVertices / 3, seed);
		vertices[2]      = generateVertices(
		    numberOfVertices - 2 * (numberOfVertices / 3), seed);
		method->init(vertices[0], vertices[1], vertices[2]);
	}
	else
		method->init(
		    QSettings().value("data/gazfile").toString().toStdString(),
		    QSettings().value("data/starsfile").toString().toStdString(),
		    QSettings().value("data/loaddarkmatter").toBool()
		        ? QSettings()
		              .value("data/darkmatterfile")
		              .toString()
		              .toStdString()
		        : "");

	cubeShader = GLHandler::newShader("default");
	GLHandler::setShaderParam(cubeShader, "alpha", 0.5f);
	GLHandler::setShaderParam(
	    cubeShader, "color",
	    QSettings().value("misc/cubecolor").value<QVector3D>());
	cube = createCube(cubeShader);

	Camera* cam = new Camera(&vrHandler);
	cam->setPerspectiveProj(70.0f, (float) width() / (float) height());
	cam->angle    = 0;
	cam->distance = 0.01;

	setCamera(cam);
}

void MainWin::updateScene(BasicCamera& camera)
{
	Camera& cam(static_cast<Camera&>(camera));
	cam.currentFrameTiming = frameTiming;
	/*float distPeriod = 60.f, anglePeriod = 10.f;
	integralDt += dt;
	if(integralDt
	   < 500000.0f * distPeriod) // if we are within a semi-period
	{
	    camera->distance
	        = 1.001
	          + 1 * cos(6.28 * integralDt / (1000000.0f * distPeriod));
	    camera->distance
	        = (camera->distance < 0.001) ? 0.001 : camera->distance;
	    anglePeriod = 20.f / camera->distance;
	    camera->angle += 6.28f * (dt / 1000000.0f) / anglePeriod;
	    camera->update();
	    // empirical
	    method->setAlpha((-28.0f / 1999.0f) * camera->distance
	                     + ((3 + ((2000 * 28) / 1999.0f)) / 1000.0f));
	}*/
}

void MainWin::renderScene(BasicCamera const& camera)
{
	Camera const& cam(static_cast<Camera const&>(camera));

	if(showCube)
	{
		GLHandler::setUpRender(cubeShader);
		GLHandler::render(cube, GLHandler::PrimitiveType::LINES);
	}
	method->render(cam);
}

std::vector<float> MainWin::generateVertices(unsigned int number, unsigned int seed)
{
	std::vector<float> vertices;
	vertices.reserve(3 * number);

	srand(seed);

	for(unsigned int i(0); i < 3 * number; ++i)
	{
		vertices.push_back(
		    2 * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX))
		    - 1);
	}

	return vertices;
}

GLHandler::Mesh MainWin::createCube(GLHandler::ShaderProgram const& shader)
{
	GLHandler::Mesh mesh(GLHandler::newMesh());
	std::vector<float> vertices = {
	    -1.0f, -1.0f, -1.0f, // 0
	    -1.0f, -1.0f, 1.0f,  // 1
	    -1.0f, 1.0f,  -1.0f, // 2
	    -1.0f, 1.0f,  1.0f,  // 3
	    1.0f,  -1.0f, -1.0f, // 4
	    1.0f,  -1.0f, 1.0f,  // 5
	    1.0f,  1.0f,  -1.0f, // 6
	    1.0f,  1.0f,  1.0f,  // 7
	};
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

void MainWin::deleteCube(GLHandler::Mesh mesh, GLHandler::ShaderProgram shader)
{
	GLHandler::deleteShader(shader);
	GLHandler::deleteMesh(mesh);
}

MainWin::~MainWin()
{
	deleteCube(cube, cubeShader);
	delete method;
}
