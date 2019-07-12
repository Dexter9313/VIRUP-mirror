#include "MainWin.hpp"

QColor MainWin::getCubeColor() const
{
	return QSettings().value("misc/cubecolor").value<QColor>();
}

void MainWin::setCubeColor(QColor const& color)
{
	QSettings().setValue("misc/cubecolor", color);
	GLHandler::setShaderParam(cubeShader, "color", color);
}

void MainWin::keyPressEvent(QKeyEvent* e)
{
	if(e->key() == Qt::Key_PageUp)
	{
		method->setAlpha(method->getAlpha() * 10 / 8);
	}
	else if(e->key() == Qt::Key_PageDown)
	{
		method->setAlpha(method->getAlpha() * 8 / 10);
	}
	else if(e->key() == Qt::Key_Home)
	{
		// integralDt    = 0;
		if(vrHandler)
		{
			vrHandler.resetPos();
		}
	}
	else if(e->key() == Qt::Key_M)
	{
		method->toggleDarkMatter();
	}
	else if(e->key() == Qt::Key_C)
	{
		showCube = !showCube;
	}
	else if(e->key() == Qt::Key_H)
	{
		setHDR(!getHDR());
	}
	else if(e->key() == Qt::Key_P)
	{
		printPositionInDataSpace();
	}
	movementControls->keyPressEvent(e);
	AbstractMainWin::keyPressEvent(e);
}

void MainWin::keyReleaseEvent(QKeyEvent* e)
{
	movementControls->keyReleaseEvent(e);
	AbstractMainWin::keyReleaseEvent(e);
}

void MainWin::mouseMoveEvent(QMouseEvent* e)
{
	if(!isActive() || vrHandler)
	{
		return;
	}
	auto cam(dynamic_cast<Camera*>(&getCamera("default")));
	float dx = (static_cast<float>(width()) / 2 - e->globalX()) / width();
	float dy = (static_cast<float>(height()) / 2 - e->globalY()) / height();
	cam->yaw += dx * 3.14f / 3.f;
	cam->pitch += dy * 3.14f / 3.f;
	QCursor::setPos(width() / 2, height() / 2);
}

void MainWin::wheelEvent(QWheelEvent* e)
{
	movementControls->wheelEvent(e);
	AbstractMainWin::wheelEvent(e);
}

void MainWin::vrEvent(VRHandler::Event const& e)
{
	auto cam(dynamic_cast<Camera*>(&getCamera("default")));
	switch(e.type)
	{
		case VRHandler::EventType::BUTTON_PRESSED:
			switch(e.button)
			{
				case VRHandler::Button::TOUCHPAD:
				{
					Controller const* ctrl(vrHandler.getController(e.side));
					if(ctrl != nullptr)
					{
						QVector2D padCoords(ctrl->getPadCoords());
						if(padCoords.length() < 0.5) // CENTER
						{
							method->resetAlpha();
							cam->setEyeDistanceFactor(1.0f);
						}
						else if(fabsf(padCoords[0])
						        > fabsf(padCoords[1])) // LEFT OR
						                               // RIGHT
						{
							if(padCoords[0] < 0.0f) // LEFT
							{
								method->setAlpha(method->getAlpha() * 8 / 10);
							}
							else // RIGHT
							{
								method->setAlpha(method->getAlpha() * 10 / 8);
							}
						}
					}
					break;
				}
				case VRHandler::Button::TRIGGER:
					method->toggleDarkMatter();
					break;
				case VRHandler::Button::MENU:
					printPositionInDataSpace(e.side);
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}

	movementControls->vrEvent(
	    e, getCamera("default").trackedSpaceToWorldTransform());
	AbstractMainWin::vrEvent(e);
}

void MainWin::setupPythonAPI()
{
	PythonQtHandler::addObject("VIRUP", this);
}

void MainWin::initScene()
{
	QCursor c(cursor());
	c.setShape(Qt::CursorShape::BlankCursor);
	setCursor(c);

	cubeShader = GLHandler::newShader("default");
	GLHandler::setShaderParam(cubeShader, "alpha", 0.5f);
	GLHandler::setShaderParam(
	    cubeShader, "color",
	    QSettings().value("misc/cubecolor").value<QColor>());
	cube = createCube(cubeShader);

	auto cam = new Camera(&vrHandler);
	cam->setPerspectiveProj(70.0f, static_cast<float>(width())
	                                   / static_cast<float>(height()));
	removeSceneRenderPath("default");
	appendSceneRenderPath("default", RenderPath(cam));

	// METHOD LOADING
	QStringList argv = QCoreApplication::arguments();
	int argc         = argv.size();
	unsigned int numberOfVertices(500000), seed(time(nullptr));
	QString methodStr("");

	if(argc > 1)
	{
		methodStr = argv[1];
	}

	if(methodStr == "--base")
	{
		method = new BaseLineMethod();
	}
	else if(methodStr == "--basetex")
	{
		method = new BaseLineMethodTex();
	}
	else if(methodStr == "--treetex")
	{
		method = new TreeMethodTex();
	}
	else // if(methodStr == "--treelod")
	{
		method = new TreeMethodLOD();
	}

	std::cout << "Method : " << method->getName() << std::endl;
	std::cout << "Seed : " << seed << std::endl;

	if(argc > 3)
	{
		seed = argv[3].toInt();
	}
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
	{
		method->init(
		    QSettings().value("data/gazfile").toString().toStdString(),
		    QSettings().value("data/starsfile").toString().toStdString(),
		    QSettings().value("data/loaddarkmatter").toBool()
		        ? QSettings()
		              .value("data/darkmatterfile")
		              .toString()
		              .toStdString()
		        : "");
	}

	movementControls
	    = new MovementControls(vrHandler, method->getDataBoundingBox());

	double cubeScale(movementControls->getCubeScale());

	method->setAlpha(method->getAlpha() / (cubeScale * cubeScale));

	loaded = true;
}

void MainWin::updateScene(BasicCamera& camera, QString const& /*pathId*/)
{
	if(!loaded)
	{
		return;
	}
	auto& cam(dynamic_cast<Camera&>(camera));
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

	movementControls->update(cam, frameTiming);
}

void MainWin::renderScene(BasicCamera const& camera, QString const& /*pathId*/)
{
	if(!loaded)
	{
		return;
	}
	auto cam(dynamic_cast<Camera const*>(&camera));

	GLHandler::glf().glDepthFunc(GL_LEQUAL);
	GLHandler::glf().glEnable(GL_DEPTH_CLAMP);
	QMatrix4x4 model;

	std::array<double, 3> cubeTranslation(
	    movementControls->getCubeTranslation());
	double cubeScale(movementControls->getCubeScale());
	model.translate(
	    QVector3D(cubeTranslation[0], cubeTranslation[1], cubeTranslation[2]));
	model.scale(cubeScale);
	if(showCube)
	{
		GLHandler::setUpRender(cubeShader, model);
		GLHandler::render(cube, GLHandler::PrimitiveType::LINES);
	}
	method->render(*cam, cubeScale, cubeTranslation);
	GLHandler::glf().glDisable(GL_DEPTH_CLAMP);
}

QVector3D MainWin::dataToWorldPosition(QVector3D const& data) const
{
	std::array<double, 3> cubeTranslation(
	    movementControls->getCubeTranslation());
	double cubeScale(movementControls->getCubeScale());
	QVector3D result(data);
	result *= cubeScale;
	for(unsigned int i(0); i < 3; ++i)
	{
		result[i] += cubeTranslation.at(i);
	}
	return result;
}

QVector3D MainWin::worldToDataPosition(QVector3D const& world) const
{
	std::array<double, 3> cubeTranslation(
	    movementControls->getCubeTranslation());
	double cubeScale(movementControls->getCubeScale());
	QVector3D result(world);
	for(unsigned int i(0); i < 3; ++i)
	{
		result[i] -= cubeTranslation.at(i);
	}
	result /= cubeScale;
	return result;
}

void MainWin::printPositionInDataSpace(Side controller) const
{
	QVector3D position(0.f, 0.f, 0.f);
	Controller const* cont(vrHandler.getController(controller));
	// world space first
	if(cont != nullptr)
	{
		position = getCamera("default").trackedSpaceToWorldTransform()
		           * cont->getPosition();
	}
	else
	{
		position
		    = getCamera("default").hmdScaledSpaceToWorldTransform() * position;
	}

	// then data space
	position = worldToDataPosition(position);
	QString posstr;
	&posstr << position;

	auto msgBox = new QMessageBox;
	msgBox->setAttribute(Qt::WA_DeleteOnClose);
	msgBox->setStandardButtons(QMessageBox::Ok);
	msgBox->setWindowTitle(tr("Position selected"));
	msgBox->setText(posstr);
	msgBox->setModal(false);
	msgBox->show();
}

std::vector<float> MainWin::generateVertices(unsigned int number,
                                             unsigned int seed)
{
	std::vector<float> vertices;
	vertices.reserve(3 * number);

	srand(seed);

	for(unsigned int i(0); i < 3 * number; ++i)
	{
		vertices.push_back(
		    // NOLINT(cert-msc30-c)
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
	delete movementControls;
	deleteCube(cube, cubeShader);
	delete method;
}
