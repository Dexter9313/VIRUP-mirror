#include "MainWin.hpp"

MainWin::MainWin()
    : AbstractMainWin()
{
}

void MainWin::keyPressEvent(QKeyEvent* e)
{
	Camera& cam(static_cast<Camera&>(getCamera()));
	if(e->key() == Qt::Key_Up)
		cam.distance /= 1.2;
	else if(e->key() == Qt::Key_Down)
		cam.distance *= 1.2;
	else if(e->key() == Qt::Key_PageUp)
		method->setAlpha(method->getAlpha() * 10 / 8);
	else if(e->key() == Qt::Key_PageDown)
		method->setAlpha(method->getAlpha() * 8 / 10);
	else if(e->key() == Qt::Key_Home)
	{
		// integralDt    = 0;
		cam.angleAroundZ = 0.f;
		cam.angleAboveXY = 0.f;
		cam.distance     = 0.01f;
		if(vrHandler)
			vrHandler.resetPos();
	}
	else if(e->key() == Qt::Key_D)
		method->showdm = !method->showdm;
	else if(e->key() == Qt::Key_C)
		showCube = !showCube;
	else if(e->key() == Qt::Key_H)
		setHDR(!getHDR());

	AbstractMainWin::keyPressEvent(e);
}

void MainWin::mousePressEvent(QMouseEvent* e)
{
	if(e->button() != Qt::MouseButton::RightButton)
		return;

	lastCursorPos = QCursor::pos();
	QCursor::setPos(width() / 2, height() / 2);
	QCursor c(cursor());
	c.setShape(Qt::CursorShape::BlankCursor);
	setCursor(c);
	trackballEnabled = true;

}

void MainWin::mouseReleaseEvent(QMouseEvent* e)
{
	if(e->button() != Qt::MouseButton::RightButton)
		return;

	trackballEnabled = false;
	QCursor c(cursor());
	c.setShape(Qt::CursorShape::ArrowCursor);
	setCursor(c);
	QCursor::setPos(lastCursorPos);
}

void MainWin::mouseMoveEvent(QMouseEvent* e)
{
	if(!trackballEnabled)
		return;
	Camera& cam(static_cast<Camera&>(getCamera()));
	float dx = static_cast<float>((width() / 2) - e->globalX()) / width();
	float dy = static_cast<float>((height() / 2) - e->globalY()) / height();
	cam.angleAroundZ += dx * 3.14f / 3.f;
	cam.angleAboveXY += dy * 3.14f / 3.f;
	QCursor::setPos(width() / 2, height() / 2);
}

void MainWin::wheelEvent(QWheelEvent* e)
{
	cubeScale += cubeScale * e->angleDelta().y() / 1000.f;
	AbstractMainWin::wheelEvent(e);
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
				{
					Controller const* left(vrHandler.getController(Side::LEFT));
					Controller const* right(
					    vrHandler.getController(Side::RIGHT));
					if(e.side == Side::LEFT && left)
					{
						leftGripPressed = true;
						initControllerPosInCube
						    = getCamera().trackedSpaceToWorldTransform()
						          * left->getPosition()
						      - cubeTranslation;
					}
					else if(e.side == Side::RIGHT && right)
					{
						rightGripPressed = true;
						initControllerPosInCube
						    = getCamera().trackedSpaceToWorldTransform()
						          * right->getPosition()
						      - cubeTranslation;
					}
					else
						break;
					if(leftGripPressed && rightGripPressed && left && right)
					{
						initControllersDistance
						    = left->getPosition().distanceToPoint(
						        right->getPosition());
						initScale = cubeScale;
						QVector3D controllersMidPoint
						    = left->getPosition() + right->getPosition();
						controllersMidPoint /= 2.f;
						controllersMidPoint
						    = getCamera().trackedSpaceToWorldTransform()
						      * controllersMidPoint;
						scaleCenter = controllersMidPoint;

						QVector3D controllersMidPointInCube(computeCubeModel().inverted()
															* controllersMidPoint);

						if(controllersMidPointInCube.x() < -1
						   || controllersMidPointInCube.x() > 1
						   || controllersMidPointInCube.y() < -1
						   || controllersMidPointInCube.y() > 1
						   || controllersMidPointInCube.z() < -1
						   || controllersMidPointInCube.z() > 1)
							scaleCenter = cubeTranslation;
					}
					break;
				}
				case VRHandler::Button::TOUCHPAD:
				{
					Controller const* ctrl(vrHandler.getController(e.side));
					if(ctrl)
					{
						QVector2D padCoords(ctrl->getPadCoords());
						if(padCoords.length() < 0.5) // CENTER
						{
							method->resetAlpha();
							cam.setEyeDistanceFactor(1.0f);
						}
						else if(fabs(padCoords[0])
						        > fabs(padCoords[1])) // LEFT OR
						                              // RIGHT
						{
							if(padCoords[0] < 0.0f) // LEFT
								method->setAlpha(method->getAlpha() * 8
								                 / 10);
							else // RIGHT
								method->setAlpha(method->getAlpha() * 10
								                 / 8);
						}
					}
					break;
				}
				case VRHandler::Button::TRIGGER:
					method->showdm = !method->showdm;
					break;
				case VRHandler::Button::MENU:
					setHDR(!getHDR());
					break;
				default:
					std::cout << "Button (?) Pressed on controller "
					          << (unsigned int) e.side << " !" << std::endl;
			}
			break;
		case VRHandler::EventType::BUTTON_UNPRESSED:
			switch(e.button)
			{
				case VRHandler::Button::GRIP:
				{
					Controller const* left(vrHandler.getController(Side::LEFT));
					Controller const* right(
					    vrHandler.getController(Side::RIGHT));
					if(e.side == Side::LEFT)
					{
						leftGripPressed = false;
						if(right && rightGripPressed)
						{
							initControllerPosInCube
							    = getCamera().trackedSpaceToWorldTransform()
							          * right->getPosition()
							      - cubeTranslation;
						}
					}
					else if(e.side == Side::RIGHT)
					{
						rightGripPressed = false;
						if(left && leftGripPressed)
						{
							initControllerPosInCube
							    = getCamera().trackedSpaceToWorldTransform()
							          * left->getPosition()
							      - cubeTranslation;
						}
					}
					break;
				}
				default:
					break;
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
	    QSettings().value("misc/cubecolor").value<QColor>());
	cube = createCube(cubeShader);

	Camera* cam = new Camera(&vrHandler);
	cam->setPerspectiveProj(70.0f, (float) width() / (float) height());
	cam->distance = 0.5;

	setCamera(cam);
}

void MainWin::updateScene(BasicCamera& camera)
{
	Camera& cam(static_cast<Camera&>(camera));
	cam.currentFrameTiming = frameTiming;

	Controller const* left(vrHandler.getController(Side::LEFT));
	Controller const* right(vrHandler.getController(Side::RIGHT));

	// single grip = translation
	if(leftGripPressed != rightGripPressed)
	{
		QVector3D controllerPosInCube;
		if(leftGripPressed && left)
		{
			controllerPosInCube = getCamera().trackedSpaceToWorldTransform()
			                          * left->getPosition()
			                      - cubeTranslation;
		}
		else if(rightGripPressed && right)
		{
			controllerPosInCube = getCamera().trackedSpaceToWorldTransform()
			                          * right->getPosition()
			                      - cubeTranslation;
		}
		cubeTranslation += controllerPosInCube - initControllerPosInCube;
	}
	// double grip = scale
	if(leftGripPressed && rightGripPressed && left && right)
	{

		rescaleCube(
		    initScale
		        * left->getPosition().distanceToPoint(right->getPosition())
		        / initControllersDistance,
		    scaleCenter);
	}

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

	QMatrix4x4 model(computeCubeModel());
	if(showCube)
	{
		GLHandler::setUpRender(cubeShader, model);
		GLHandler::render(cube, GLHandler::PrimitiveType::LINES);
	}
	method->render(cam, model);
}

QMatrix4x4 MainWin::computeCubeModel() const
{
	QMatrix4x4 result;
	result.translate(cubeTranslation);
	result.scale(cubeScale);
	return result;
}

void MainWin::rescaleCube(float newScale, QVector3D const& scaleCenter)
{
	QVector3D scaleCenterPosInCube = scaleCenter - cubeTranslation;
	scaleCenterPosInCube *= newScale / cubeScale;
	cubeTranslation = scaleCenter - scaleCenterPosInCube;
	cubeScale       = newScale;
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
