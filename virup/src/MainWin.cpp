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
	// CONTROLS
	else if(e->key() == Qt::Key_W || e->key() == Qt::Key_Up)
	{
		cubePositiveVelocity.setZ(1);
	}
	else if(e->key() == Qt::Key_A || e->key() == Qt::Key_Left)
	{
		cubePositiveVelocity.setX(1);
	}
	else if(e->key() == Qt::Key_S || e->key() == Qt::Key_Down)
	{
		cubeNegativeVelocity.setZ(-1);
	}
	else if(e->key() == Qt::Key_D || e->key() == Qt::Key_Right)
	{
		cubeNegativeVelocity.setX(-1);
	}

	AbstractMainWin::keyPressEvent(e);
}

void MainWin::keyReleaseEvent(QKeyEvent* e)
{
	// CONTROLS
	if(e->key() == Qt::Key_W || e->key() == Qt::Key_Up)
	{
		cubePositiveVelocity.setZ(0);
	}
	else if(e->key() == Qt::Key_A || e->key() == Qt::Key_Left)
	{
		cubePositiveVelocity.setX(0);
	}
	else if(e->key() == Qt::Key_S || e->key() == Qt::Key_Down)
	{
		cubeNegativeVelocity.setZ(0);
	}
	else if(e->key() == Qt::Key_D || e->key() == Qt::Key_Right)
	{
		cubeNegativeVelocity.setX(0);
	}

	AbstractMainWin::keyPressEvent(e);
}
/*
void MainWin::mousePressEvent(QMouseEvent* e)
{
    if(e->button() != Qt::MouseButton::RightButton)
    {
        return;
    }

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
    {
        return;
    }

    trackballEnabled = false;
    QCursor c(cursor());
    c.setShape(Qt::CursorShape::ArrowCursor);
    setCursor(c);
    QCursor::setPos(lastCursorPos);
}
*/
void MainWin::mouseMoveEvent(QMouseEvent* e)
{
	/*if(!trackballEnabled)
	{
	    return;
	}*/
	auto cam(dynamic_cast<Camera*>(&getCamera()));
	float dx = (static_cast<float>(width()) / 2 - e->globalX()) / width();
	float dy = (static_cast<float>(height()) / 2 - e->globalY()) / height();
	cam->yaw += dx * 3.14f / 3.f;
	cam->pitch += dy * 3.14f / 3.f;
	QCursor::setPos(width() / 2, height() / 2);
}

void MainWin::wheelEvent(QWheelEvent* e)
{
	rescaleCube(cubeScale * (1.f + e->angleDelta().y() / 1000.f));
	AbstractMainWin::wheelEvent(e);
}

void MainWin::vrEvent(VRHandler::Event const& e)
{
	auto cam(dynamic_cast<Camera*>(&getCamera()));
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
					if(e.side == Side::LEFT && left != nullptr)
					{
						leftGripPressed = true;
						for(unsigned int i(0); i < 3; ++i)
						{
							initControllerPosInCube.at(i)
							    = (getCamera().trackedSpaceToWorldTransform()
							       * left->getPosition())[i]
							      - cubeTranslation.at(i);
						}
					}
					else if(e.side == Side::RIGHT && right != nullptr)
					{
						rightGripPressed = true;
						for(unsigned int i(0); i < 3; ++i)
						{
							initControllerPosInCube.at(i)
							    = (getCamera().trackedSpaceToWorldTransform()
							       * right->getPosition())[i]
							      - cubeTranslation.at(i);
						}
					}
					else
					{
						break;
					}
					if(leftGripPressed && rightGripPressed && left != nullptr
					   && right != nullptr)
					{
						initControllersDistance
						    = left->getPosition().distanceToPoint(
						        right->getPosition());
						initScale                                 = cubeScale;
						std::array<double, 3> controllersMidPoint = {};
						for(unsigned int i(0); i < 3; ++i)
						{
							controllersMidPoint.at(i)
							    = left->getPosition()[i]
							      + right->getPosition()[i];
							controllersMidPoint.at(i) /= 2.f;
						}
						QVector3D controllersMidPointFloat(
						    controllersMidPoint[0], controllersMidPoint[1],
						    controllersMidPoint[2]);
						for(unsigned int i(0); i < 3; ++i)
						{
							controllersMidPoint.at(i)
							    = (getCamera().trackedSpaceToWorldTransform()
							       * controllersMidPointFloat)[i];
							scaleCenter.at(i) = controllersMidPoint.at(i);
						}

						std::array<double, 3> controllersMidPointInCube = {};

						for(unsigned int i(0); i < 3; ++i)
						{
							controllersMidPointInCube.at(i)
							    = (controllersMidPoint.at(i)
							       - cubeTranslation.at(i))
							      / cubeScale;
						}

						if(controllersMidPointInCube[0] < -1
						   || controllersMidPointInCube[0] > 1
						   || controllersMidPointInCube[1] < -1
						   || controllersMidPointInCube[1] > 1
						   || controllersMidPointInCube[2] < -1
						   || controllersMidPointInCube[2] > 1)
						{
							for(unsigned int i(0); i < 3; ++i)
							{
								scaleCenter.at(i) = cubeTranslation.at(i);
							}
						}
					}
					break;
				}
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
					setHDR(!getHDR());
					break;
				default:
					std::cout << "Button (?) Pressed on controller "
					          << static_cast<unsigned int>(e.side) << " !"
					          << std::endl;
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
						if(right != nullptr && rightGripPressed)
						{
							for(unsigned int i(0); i < 3; ++i)
							{
								initControllerPosInCube.at(i)
								    = (getCamera()
								           .trackedSpaceToWorldTransform()
								       * right->getPosition())[i]
								      - cubeTranslation.at(i);
							}
						}
					}
					else if(e.side == Side::RIGHT)
					{
						rightGripPressed = false;
						if(left != nullptr && leftGripPressed)
						{
							for(unsigned int i(0); i < 3; ++i)
							{
								initControllerPosInCube.at(i)
								    = (getCamera()
								           .trackedSpaceToWorldTransform()
								       * left->getPosition())[i]
								      - cubeTranslation.at(i);
							}
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
	setCamera(cam);

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

	BBox dataBBox = method->getDataBoundingBox();
	if((dataBBox.maxx - dataBBox.minx >= dataBBox.maxy - dataBBox.miny)
	   && (dataBBox.maxx - dataBBox.minx >= dataBBox.maxz - dataBBox.minz))
	{
		cubeScale = 1.f / (dataBBox.maxx - dataBBox.minx);
	}
	else if(dataBBox.maxy - dataBBox.miny >= dataBBox.maxz - dataBBox.minz)
	{
		cubeScale = 1.f / (dataBBox.maxy - dataBBox.miny);
	}
	else
	{
		cubeScale = 1.f / (dataBBox.maxz - dataBBox.minz);
	}
	cubeTranslation[0] = -1 * dataBBox.mid.x() * cubeScale;
	cubeTranslation[1] = -1 * dataBBox.mid.y() * cubeScale;
	cubeTranslation[2] = -1 * dataBBox.mid.z() * cubeScale;

	method->setAlpha(method->getAlpha() / (cubeScale * cubeScale));

	loaded = true;
}

void MainWin::updateScene(BasicCamera& camera)
{
	if(!loaded)
	{
		return;
	}
	auto cam(dynamic_cast<Camera*>(&camera));
	cam->currentFrameTiming = frameTiming;

	Controller const* left(vrHandler.getController(Side::LEFT));
	Controller const* right(vrHandler.getController(Side::RIGHT));

	// single grip = translation
	if(leftGripPressed != rightGripPressed)
	{
		std::array<double, 3> controllerPosInCube = {};
		if(leftGripPressed && left != nullptr)
		{
			for(unsigned int i(0); i < 3; ++i)
			{
				controllerPosInCube.at(i)
				    = (getCamera().trackedSpaceToWorldTransform()
				       * left->getPosition())[i]
				      - cubeTranslation.at(i);
			}
		}
		else if(rightGripPressed && right != nullptr)
		{
			for(unsigned int i(0); i < 3; ++i)
			{
				controllerPosInCube.at(i)
				    = (getCamera().trackedSpaceToWorldTransform()
				       * right->getPosition())[i]
				      - cubeTranslation.at(i);
			}
		}
		for(unsigned int i(0); i < 3; ++i)
		{
			cubeTranslation.at(i)
			    += controllerPosInCube.at(i) - initControllerPosInCube.at(i);
		}
	}
	// double grip = scale
	if(leftGripPressed && rightGripPressed && left != nullptr
	   && right != nullptr)
	{
		rescaleCube(
		    initScale
		        * left->getPosition().distanceToPoint(right->getPosition())
		        / initControllersDistance,
		    scaleCenter);
	}

	// apply keyboard controls
	if(!vrHandler)
	{
		for(unsigned int i(0); i < 3; ++i)
		{
			cubeTranslation.at(i)
			    += frameTiming
			       * (cam->getView().inverted()
			          * (cubePositiveVelocity + cubeNegativeVelocity))[i];
		}
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
	if(!loaded)
	{
		return;
	}
	auto cam(dynamic_cast<Camera const*>(&camera));

	GLHandler::glf().glDepthFunc(GL_LEQUAL);
	GLHandler::glf().glEnable(GL_DEPTH_CLAMP);
	QMatrix4x4 model;
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

void MainWin::rescaleCube(double newScale,
                          std::array<double, 3> const& scaleCenter)
{
	std::array<double, 3> scaleCenterPosInCube = {};
	for(unsigned int i(0); i < 3; ++i)
	{
		scaleCenterPosInCube.at(i) = scaleCenter.at(i) - cubeTranslation.at(i);
		scaleCenterPosInCube.at(i) *= newScale / cubeScale;
		cubeTranslation.at(i) = scaleCenter.at(i) - scaleCenterPosInCube.at(i);
	}
	cubeScale = newScale;
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
	deleteCube(cube, cubeShader);
	delete method;
}
