#include "MainWin.hpp"

MainWin::MainWin()
{
	srand(time(nullptr));
}

void MainWin::loadSolarSystem()
{
	QString solarsystemdir(
	    QSettings().value("simulation/solarsystemdir").toString());

	QFile jsonFile(solarsystemdir + "/definition.json");

	if(jsonFile.exists())
	{
		PlanetRenderer::currentSystemDir = solarsystemdir;
		CSVOrbit::currentSystemDir       = solarsystemdir;

		jsonFile.open(QIODevice::ReadOnly);
		QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
		QString name(QFileInfo(jsonFile).dir().dirName());
		solarSystem = new OrbitalSystem(name.toStdString(), jsonDoc.object());
		if(!solarSystem->isValid())
		{
			std::cerr << solarSystem->getName() << " is invalid... ";
			delete solarSystem;
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		QMessageBox::critical(nullptr, tr("Invalid data directory"),
		                      tr("The solar system root directory doesn't "
		                         "contain any definition.json file."));
		exit(EXIT_FAILURE);
	}

	auto barycenters = solarSystem->getAllBinariesNames();
	auto stars       = solarSystem->getAllStarsNames();
	auto fcPlanets   = solarSystem->getAllFirstClassPlanetsNames();
	auto satellites  = solarSystem->getAllSatellitePlanetsNames();

	std::cout << "-=-=- SYSTEM " << solarSystem->getName() << " -=-=-"
	          << std::endl;
	std::cout << "Barycenters : " << barycenters.size() << std::endl;
	for(auto const& name : barycenters)
	{
		std::cout << name << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Stars : " << stars.size() << std::endl;
	for(auto const& name : stars)
	{
		std::cout << name << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Main Planets : " << fcPlanets.size() << std::endl;
	for(auto const& name : fcPlanets)
	{
		std::cout << name << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Satellites : " << satellites.size() << std::endl;
	for(auto const& name : satellites)
	{
		std::cout << name << "(" << (*solarSystem)[name]->getParent()->getName()
		          << ")" << std::endl;
	}
	std::cout << std::endl;

	if(camPlanet == nullptr)
	{
		camPlanet = new OrbitalSystemCamera(&vrHandler);
		camPlanet->setPerspectiveProj(
		    70.0f, static_cast<float>(width()) / static_cast<float>(height()));
	}
	camPlanet->target           = solarSystem->getMainCelestialBody();
	camPlanet->relativePosition = Vector3(
	    camPlanet->target->getCelestialBodyParameters().radius * 2.0, 0.0, 0.0);
	solarSystemRenderer = new OrbitalSystemRenderer(solarSystem);

	CelestialBodyRenderer::overridenScale = 1.0;
}

void MainWin::loadNewSystem()
{
	if(orbitalSystem != nullptr && orbitalSystem != solarSystem)
	{
		delete systemRenderer;
		delete orbitalSystem;
	}

	if(static_cast<double>(rand()) / RAND_MAX
	   < QSettings().value("simulation/solarsystemprob").toUInt() / 100.0)
	{
		orbitalSystem  = solarSystem;
		systemRenderer = solarSystemRenderer;
		QString solarsystemdir(
		    QSettings().value("simulation/solarsystemdir").toString());
		PlanetRenderer::currentSystemDir = solarsystemdir;
		CSVOrbit::currentSystemDir       = solarsystemdir;
	}
	else
	{
		QString planetsystemdir(
		    QSettings().value("simulation/planetsystemdir").toString());

		unsigned int tries(3);

		while(tries > 0)
		{
			tries--;
			QFile jsonFile;
			QStringList nameFilter;
			nameFilter << "*.json";

			QStringList files;
			QDirIterator it(planetsystemdir, QStringList() << "*.json",
			                QDir::Files, QDirIterator::Subdirectories);
			while(it.hasNext())
			{
				files << it.next();
			}

			jsonFile.setFileName(files[rand() % files.size()]);

			if(jsonFile.exists())
			{
				PlanetRenderer::currentSystemDir = planetsystemdir;
				CSVOrbit::currentSystemDir       = planetsystemdir;

				jsonFile.open(QIODevice::ReadOnly);
				QJsonDocument jsonDoc
				    = QJsonDocument::fromJson(jsonFile.readAll());
				QString name(QFileInfo(jsonFile).dir().dirName());
				orbitalSystem
				    = new OrbitalSystem(name.toStdString(), jsonDoc.object());
				if(!orbitalSystem->isValid())
				{
					std::cerr << orbitalSystem->getName() << " is invalid... ";
					delete orbitalSystem;
					if(tries > 0)
					{
						std::cerr << "Trying another one..." << std::endl;
					}
					else
					{
						std::cerr << "All tries done. Shuting down..."
						          << std::endl;
						exit(EXIT_FAILURE);
					}
				}
				else
				{
					tries = 0;
				}
			}
			else
			{
				QMessageBox::critical(
				    nullptr, tr("Invalid data directory"),
				    tr("The planetary system root directory doesn't "
				       "contain any definition.json file."));
				exit(EXIT_FAILURE);
			}
		}
		systemRenderer = new OrbitalSystemRenderer(orbitalSystem);
	}

	debugText->setText(QString(orbitalSystem->getName().c_str()));
	lastTargetName      = orbitalSystem->getMainCelestialBody()->getName();
	timeSinceTextUpdate = 0.f;

	auto barycenters = orbitalSystem->getAllBinariesNames();
	auto stars       = orbitalSystem->getAllStarsNames();
	auto fcPlanets   = orbitalSystem->getAllFirstClassPlanetsNames();
	auto satellites  = orbitalSystem->getAllSatellitePlanetsNames();

	std::cout << "-=-=- SYSTEM " << orbitalSystem->getName() << " -=-=-"
	          << std::endl;
	std::cout << "Barycenters : " << barycenters.size() << std::endl;
	for(auto const& name : barycenters)
	{
		std::cout << name << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Stars : " << stars.size() << std::endl;
	for(auto const& name : stars)
	{
		std::cout << name << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Main Planets : " << fcPlanets.size() << std::endl;
	for(auto const& name : fcPlanets)
	{
		std::cout << name << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Satellites : " << satellites.size() << std::endl;
	for(auto const& name : satellites)
	{
		std::cout << name << "("
		          << (*orbitalSystem)[name]->getParent()->getName() << ")"
		          << std::endl;
	}
	std::cout << std::endl;

	if(camPlanet == nullptr)
	{
		camPlanet = new OrbitalSystemCamera(&vrHandler);
		camPlanet->setPerspectiveProj(
		    70.0f, static_cast<float>(width()) / static_cast<float>(height()));
	}
	camPlanet->target           = orbitalSystem->getMainCelestialBody();
	camPlanet->relativePosition = Vector3(
	    camPlanet->target->getCelestialBodyParameters().radius * 2.0, 0.0, 0.0);

	CelestialBodyRenderer::overridenScale = 1.0;
}

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
	if(loaded)
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
		else if(e->key() == Qt::Key_R)
		{
			float tc(clock.getTimeCoeff());
			if(tc > 1.f && !clock.getLockedRealTime())
			{
				clock.setTimeCoeff(tc / 10.f);
				debugText->setText(
				    ("Time coeff. : "
				     + std::to_string(static_cast<int>(tc / 10.f)) + "x")
				        .c_str());
				timeSinceTextUpdate = 0.f;
			}
		}
		else if(e->key() == Qt::Key_T)
		{
			float tc(clock.getTimeCoeff());
			if(tc < 1000000.f && !clock.getLockedRealTime())
			{
				clock.setTimeCoeff(tc * 10.f);
				debugText->setText(
				    ("Time coeff. : "
				     + std::to_string(static_cast<int>(tc * 10.f)) + "x")
				        .c_str());
				timeSinceTextUpdate = 0.f;
			}
		}
		movementControls->keyPressEvent(e);
	}
	AbstractMainWin::keyPressEvent(e);
}

void MainWin::keyReleaseEvent(QKeyEvent* e)
{
	if(loaded)
	{
		movementControls->keyReleaseEvent(e);
	}
	AbstractMainWin::keyReleaseEvent(e);
}

void MainWin::mouseMoveEvent(QMouseEvent* e)
{
	if(!isActive() || vrHandler || !loaded)
	{
		return;
	}
	float dx = (static_cast<float>(width()) / 2 - e->globalX()) / width();
	float dy = (static_cast<float>(height()) / 2 - e->globalY()) / height();
	auto cam(dynamic_cast<Camera*>(&getCamera("cosmo")));
	cam->yaw += dx * 3.14f / 3.f;
	cam->pitch += dy * 3.14f / 3.f;
	auto cam2 = (dynamic_cast<OrbitalSystemCamera*>(&getCamera("planet")));
	cam2->yaw += dx * 3.14f / 3.f;
	cam2->pitch += dy * 3.14f / 3.f;
	QCursor::setPos(width() / 2, height() / 2);
}

void MainWin::wheelEvent(QWheelEvent* e)
{
	if(loaded)
	{
		movementControls->wheelEvent(e);
	}
	AbstractMainWin::wheelEvent(e);
}

void MainWin::vrEvent(VRHandler::Event const& e)
{
	if(loaded)
	{
		auto cam(dynamic_cast<Camera*>(&getCamera("cosmo")));
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
							        > fabsf(padCoords[1])) // LEFT OR RIGHT
							{
								if(padCoords[0] < 0.0f) // LEFT
								{
									method->setAlpha(method->getAlpha() * 8
									                 / 10);
								}
								else // RIGHT
								{
									method->setAlpha(method->getAlpha() * 10
									                 / 8);
								}
							}
							else // UP OR DOWN
							{
								float tc(clock.getTimeCoeff());
								if(padCoords[1] < 0.0f) // DOWN
								{
									if(tc > 1.f && !clock.getLockedRealTime())
									{
										clock.setTimeCoeff(tc / 10.f);
										debugText->setText(
										    ("Time coeff. : "
										     + std::to_string(
										           static_cast<int>(tc / 10.f))
										     + "x")
										        .c_str());
										timeSinceTextUpdate = 0.f;
									}
								}
								else // UP
								{
									if(tc < 1000000.f
									   && !clock.getLockedRealTime())
									{
										clock.setTimeCoeff(tc * 10.f);
										debugText->setText(
										    ("Time coeff. : "
										     + std::to_string(
										           static_cast<int>(tc * 10.f))
										     + "x")
										        .c_str());
										timeSinceTextUpdate = 0.f;
									}
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
		    e, getCamera("cosmo").trackedSpaceToWorldTransform());
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

	// PLANETS LOADING
	debugText = new Text3D(textWidth, textHeight);
	debugText->setFlags(Qt::AlignCenter);
	debugText->setColor(QColor(255, 0, 0));

	loadSolarSystem();
	loadNewSystem();

	debugText->setText("");

	movementControls = new MovementControls(
	    vrHandler, method->getDataBoundingBox(), camPlanet);

	double cubeScale(movementControls->getCubeScale());

	method->setAlpha(method->getAlpha() / (cubeScale * cubeScale));

	removeSceneRenderPath("default");

	appendSceneRenderPath("cosmo", RenderPath(cam));
	appendSceneRenderPath("planet", RenderPath(camPlanet));

	// we will draw them ourselves
	pathIdRenderingControllers = "";

	loaded = true;
}

void MainWin::updateScene(BasicCamera& camera, QString const& pathId)
{
	if(!loaded)
	{
		return;
	}

	if(pathId == "cosmo")
	{
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
	if(pathId == "planet")
	{
		auto& cam = dynamic_cast<OrbitalSystemCamera&>(camera);
		if(vrHandler)
		{
			debugText->getModel() = cam.hmdSpaceToWorldTransform();
			debugText->getModel().translate(QVector3D(0.0f, -0.075f, -0.20f));
			debugText->getModel().scale(
			    1.5 * static_cast<float>(textWidth) / width(),
			    1.5 * static_cast<float>(textHeight) / height());
		}
		else
		{
			debugText->getModel() = cam.screenToWorldTransform();
			// debugText->getModel().translate(QVector3D(-0.88f, 0.88f, 0.f));
			debugText->getModel().scale(
			    2 * static_cast<float>(textWidth) / width(),
			    2 * static_cast<float>(textWidth) / height());
		}

		timeSinceTextUpdate += frameTiming;
		if(!OctreeLOD::renderPlanetarySystem)
		{
			return;
		}
		if(lastData != OctreeLOD::planetarySysInitData())
		{
			loadNewSystem();
		}

		lastData   = OctreeLOD::planetarySysInitData();
		sysInWorld = dataToWorldPosition(lastData);

		if(cam.target == orbitalSystem->getMainCelestialBody())
		{
			cam.relativePosition = -1 * sysInWorld
			                       / (OctreeLOD::planetarySysInitScale
			                          * movementControls->getCubeScale());
		}

		CelestialBodyRenderer::overridenScale
		    = OctreeLOD::planetarySysInitScale
		      * movementControls->getCubeScale();

		sysInWorld = dataToWorldPosition(lastData);

		clock.update();
		cam.updateUT(clock.getCurrentUt());

		systemRenderer->updateMesh(clock.getCurrentUt(), cam);

		std::string targetName(cam.target->getName());
		if(targetName != lastTargetName)
		{
			debugText->setText(targetName.c_str());
			timeSinceTextUpdate = 0.f;
			lastTargetName      = targetName;
		}
		return;
	}
}

void MainWin::renderScene(BasicCamera const& camera, QString const& pathId)
{
	if(!loaded)
	{
		return;
	}

	if(pathId == "planet")
	{
		if(!OctreeLOD::renderPlanetarySystem)
		{
			if(timeSinceTextUpdate < 5.f)
			{
				debugText->render();
			}
			return;
		}
		systemRenderer->render(camera);
		renderVRControls();
		systemRenderer->renderTransparent(camera);
		if(timeSinceTextUpdate < 5.f)
		{
			debugText->render();
		}
		return;
	}

	if(!OctreeLOD::renderPlanetarySystem)
	{
		renderVRControls();
	}
	auto cam(dynamic_cast<Camera const*>(&camera));

	GLHandler::glf().glDepthFunc(GL_LEQUAL);
	GLHandler::glf().glEnable(GL_DEPTH_CLAMP);
	QMatrix4x4 model;

	Vector3 cubeTranslation(movementControls->getCubeTranslation());
	double cubeScale(movementControls->getCubeScale());
	model.translate(
	    QVector3D(cubeTranslation[0], cubeTranslation[1], cubeTranslation[2]));
	model.scale(cubeScale);
	if(showCube)
	{
		GLHandler::setUpRender(cubeShader, model);
		GLHandler::render(cube, GLHandler::PrimitiveType::LINES);
	}
	method->render(
	    *cam, cubeScale,
	    {{cubeTranslation[0], cubeTranslation[1], cubeTranslation[2]}});
	GLHandler::glf().glDisable(GL_DEPTH_CLAMP);
}

Vector3 MainWin::dataToWorldPosition(Vector3 const& data) const
{
	Vector3 cubeTranslation(movementControls->getCubeTranslation());
	double cubeScale(movementControls->getCubeScale());
	Vector3 result(data);
	result *= cubeScale;
	result += cubeTranslation;
	return result;
}

Vector3 MainWin::worldToDataPosition(Vector3 const& world) const
{
	Vector3 cubeTranslation(movementControls->getCubeTranslation());
	double cubeScale(movementControls->getCubeScale());
	Vector3 result(world);
	result -= cubeTranslation;
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
		position = getCamera("cosmo").trackedSpaceToWorldTransform()
		           * cont->getPosition();
	}
	else
	{
		position
		    = getCamera("cosmo").hmdScaledSpaceToWorldTransform() * position;
	}

	// then data space
	position = Utils::toQt(worldToDataPosition(Utils::fromQt(position)));
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
	delete systemRenderer;
	delete orbitalSystem;
	delete debugText;
	delete movementControls;
	deleteCube(cube, cubeShader);
	delete method;
}
