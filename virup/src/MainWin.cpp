#include "MainWin.hpp"

MainWin::MainWin()
{
	srand(time(nullptr));
	OctreeLOD::solarSystemDataPos() = solarSystemDataPos;
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

	if(((OctreeLOD::planetarySysInitData() - solarSystemDataPos).length() < 1e-6
	    && planetarySystemName == "")
	   || planetarySystemName == "Solar System")
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
			QStringList files;
			QDirIterator it(planetsystemdir, QStringList() << "*.json",
			                QDir::Files, QDirIterator::Subdirectories);
			while(it.hasNext())
			{
				files << it.next();
			}

			if(tries == 2 && planetarySystemName != "")
			{
				jsonFile.setFileName(planetsystemdir + planetarySystemName
				                     + "/definition.json");
			}
			else
			{
				jsonFile.setFileName(files[rand() % files.size()]);
			}

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
			else if(tries == 0)
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
	forceUpdateFromCosmo                  = true;

	planetarySystemName = orbitalSystem->getName().c_str();
}

QDateTime MainWin::getSimulationTime() const
{
	return SimulationTime::utToDateTime(clock.getCurrentUt());
}

void MainWin::setSimulationTime(QDateTime const& simulationTime)
{
	clock.setCurrentUt(SimulationTime::dateTimeToUT(simulationTime, false));
}

double MainWin::getScale() const
{
	return getCamera<Camera>("cosmo").scale * mtokpc;
}

void MainWin::setScale(double scale)
{
	getCamera<Camera>("cosmo").scale      = scale / mtokpc;
	CelestialBodyRenderer::overridenScale = scale;
}

Vector3 MainWin::getCosmoPosition() const
{
	return getCamera<Camera>("cosmo").position;
}

void MainWin::setCosmoPosition(Vector3 cosmoPosition)
{
	auto& cosmoCam(getCamera<Camera>("cosmo"));
	auto& planetCam(getCamera<OrbitalSystemCamera>("planet"));

	Vector3 diff(cosmoPosition - cosmoCam.position);
	cosmoCam.position = cosmoPosition;
	planetCam.relativePosition += diff / mtokpc;
}

QString MainWin::getPlanetTarget() const
{
	return getCamera<OrbitalSystemCamera>("planet").target->getName().c_str();
}

void MainWin::setPlanetTarget(QString const& name)
{
	auto ptrs = orbitalSystem->getAllCelestialBodiesPointers();
	for(auto ptr : ptrs)
	{
		if(QString(ptr->getName().c_str()) == name)
		{
			getCamera<OrbitalSystemCamera>("planet").target = ptr;
		}
	}
}

Vector3 MainWin::getPlanetPosition() const
{
	return getCamera<OrbitalSystemCamera>("planet").relativePosition;
}

void MainWin::setPlanetPosition(Vector3 planetPosition)
{
	auto& cosmoCam(getCamera<Camera>("cosmo"));
	auto& planetCam(getCamera<OrbitalSystemCamera>("planet"));

	Vector3 diff(planetPosition - planetCam.relativePosition);
	planetCam.relativePosition = planetPosition;
	cosmoCam.position += diff * mtokpc;
}

void MainWin::setCamPitch(float pitch)
{
	getCamera<Camera>("cosmo").pitch               = pitch;
	getCamera<OrbitalSystemCamera>("planet").pitch = pitch;
}

void MainWin::setCamYaw(float yaw)
{
	getCamera<Camera>("cosmo").yaw               = yaw;
	getCamera<OrbitalSystemCamera>("planet").yaw = yaw;
}

void MainWin::actionEvent(BaseInputManager::Action a, bool pressed)
{
	if(loaded)
	{
		if(pressed)
		{
			if(a.id == "alphaup")
			{
				method->setAlpha(method->getAlpha() * 10 / 8);
			}
			else if(a.id == "alphadown")
			{
				method->setAlpha(method->getAlpha() * 8 / 10);
			}
			else if(a.id == "resetvrpos")
			{
				// integralDt    = 0;
				if(vrHandler)
				{
					vrHandler.resetPos();
				}
			}
			else if(a.id == "toggledm")
			{
				method->toggleDarkMatter();
			}
			else if(a.id == "togglelabels")
			{
				CelestialBodyRenderer::renderLabels
				    = !CelestialBodyRenderer::renderLabels;
			}
			else if(a.id == "toggleorbits")
			{
				CelestialBodyRenderer::renderOrbits
				    = !CelestialBodyRenderer::renderOrbits;
			}
			else if(a.id == "toggleegrid")
			{
				showGrid = !showGrid;
			}
			/*else if(e->key() == Qt::Key_H)
			{
			    setHDR(!getHDR());
			}*/
			else if(a.id == "showposition")
			{
				printPositionInDataSpace();
			}
			else if(a.id == "timecoeffdown")
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
			else if(a.id == "timecoeffup")
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
		}
		movementControls->actionEvent(a, pressed);
	}
	AbstractMainWin::actionEvent(a, pressed);
}

void MainWin::mouseMoveEvent(QMouseEvent* e)
{
	if(!isActive() || vrHandler || !loaded)
	{
		return;
	}
	float dx = (static_cast<float>(width()) / 2 - e->globalX()) / width();
	float dy = (static_cast<float>(height()) / 2 - e->globalY()) / height();
	auto& cam(getCamera<Camera>("cosmo"));
	cam.yaw += dx * 3.14f / 3.f;
	cam.pitch += dy * 3.14f / 3.f;
	auto& cam2(getCamera<OrbitalSystemCamera>("planet"));
	cam2.yaw += dx * 3.14f / 3.f;
	cam2.pitch += dy * 3.14f / 3.f;
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
							if(fabsf(padCoords[0])
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
						CelestialBodyRenderer::renderLabels
						    = !CelestialBodyRenderer::renderLabels;
						CelestialBodyRenderer::renderOrbits
						    = !CelestialBodyRenderer::renderOrbits;
						toggleGrid();
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

void MainWin::initLibraries()
{
	initLibrary<LibPlanet>();
}

void MainWin::initScene()
{
	QCursor c(cursor());
	c.setShape(Qt::CursorShape::BlankCursor);
	setCursor(c);

	grid = new Grid;

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
	    vrHandler, method->getDataBoundingBox(), cam, camPlanet);

	method->setAlpha(method->getAlpha() / (cam->scale * cam->scale));

	removeSceneRenderPath("default");

	appendSceneRenderPath("cosmo", RenderPath(cam));
	appendSceneRenderPath("planet", RenderPath(camPlanet));

	// we will draw them ourselves
	pathIdRenderingControllers = "";

	loaded = true;

	// LABELS
	QString labelspath(QSettings().value("data/cosmolabelsfile").toString());
	if(labelspath != "")
	{
		QFile f(labelspath);
		if(!f.open(QFile::ReadOnly | QFile::Text))
		{
			std::cerr << "Invalid cosmological labels file path : "
			          << labelspath.toStdString() << std::endl;
		}
		else
		{
			QTextStream in(&f);
			Text3D labelText(200, 200);
			labelText.setColor(QColor(255, 0, 0));
			labelText.setSuperSampling(2.f);
			labelText.setFlags(Qt::AlignCenter);
			while(!in.atEnd())
			{
				QString line       = in.readLine();
				QStringList fields = line.split(",");
				QString label(fields[0]);
				Vector3 dataPos(fields[1].toDouble(), fields[2].toDouble(),
				                fields[3].toDouble());

				labelText.setText(label);
				cosmoLabels.push_back(
				    {dataPos, new Billboard(labelText.getImage())});
			}
		}
	}
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

		Vector3 camPosData(cam.worldToDataPosition(Utils::fromQt(
		    cam.hmdScaledSpaceToWorldTransform() * QVector3D(0.f, 0.f, 0.f))));

		for(auto cosmoLabel : cosmoLabels)
		{
			double camDist((camPosData - cosmoLabel.first).length());
			Vector3 pos(cam.dataToWorldPosition(cosmoLabel.first));
			double rescale(pos.length() <= 8000.0 ? 1.0
			                                      : 8000.0 / pos.length());
			cosmoLabel.second->position = Utils::toQt(pos * rescale);
			cosmoLabel.second->width    = rescale * camDist * cam.scale / 3.0;
		}
		movementControls->update(frameTiming);
	}
	if(pathId == "planet")
	{
		auto& cam      = dynamic_cast<OrbitalSystemCamera&>(camera);
		auto& cosmoCam = getCamera<Camera>("cosmo");
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
			planetarySystemName = "";
			loadNewSystem();
		}
		else if(planetarySystemName != orbitalSystem->getName().c_str())
		{
			loadNewSystem();
		}

		lastData   = OctreeLOD::planetarySysInitData();
		sysInWorld = cosmoCam.dataToWorldPosition(lastData);

		CelestialBodyRenderer::overridenScale = mtokpc * cosmoCam.scale;

		if((cam.target == orbitalSystem->getMainCelestialBody()
		    && CelestialBodyRenderer::overridenScale < 1e-12)
		   || forceUpdateFromCosmo)
		{
			cam.relativePosition = -1 * sysInWorld / (mtokpc * cosmoCam.scale);
			forceUpdateFromCosmo = false;
		}

		sysInWorld = cosmoCam.dataToWorldPosition(lastData);

		clock.update();
		cam.updateUT(clock.getCurrentUt());

		systemRenderer->updateMesh(clock.getCurrentUt(), cam);

		std::string targetName(cam.target->getName());
		if(targetName != lastTargetName)
		{
			debugText->setText(QString("Locked to ") + targetName.c_str());
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
		}
		else
		{
			systemRenderer->render(camera);
			renderVRControls();
			systemRenderer->renderTransparent(camera);
			if(timeSinceTextUpdate < 5.f)
			{
				debugText->render();
			}
			if(showGrid)
			{
				grid->renderPlanet(
				    dynamic_cast<OrbitalSystemCamera const&>(camera));
			}
		}
		movementControls->renderGuides();
		return;
	}

	if(!OctreeLOD::renderPlanetarySystem)
	{
		renderVRControls();
	}
	auto& cam(dynamic_cast<Camera const&>(camera));

	GLHandler::glf().glDepthFunc(GL_LEQUAL);
	GLHandler::glf().glEnable(GL_DEPTH_CLAMP);
	if(showGrid && !isPlanetarySystemLoaded())
	{
		grid->renderCosmo(cam);
	}
	method->render(cam);

	// TODO(florian) better than this
	if(CelestialBodyRenderer::renderLabels)
	{
		for(auto cosmoLabel : cosmoLabels)
		{
			if(cosmoLabel.first == solarSystemDataPos
			   && OctreeLOD::renderPlanetarySystem
			   && orbitalSystem == solarSystem)
			{
				continue;
			}
			cosmoLabel.second->render(camera);
		}
	}
	GLHandler::glf().glDisable(GL_DEPTH_CLAMP);
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
	position = Utils::toQt(getCamera<Camera>("cosmo").worldToDataPosition(
	    Utils::fromQt(position)));
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

MainWin::~MainWin()
{
	for(auto cosmoLabel : cosmoLabels)
	{
		delete cosmoLabel.second;
	}
	delete systemRenderer;
	delete orbitalSystem;
	delete debugText;
	delete movementControls;
	delete method;
	delete grid;
}
