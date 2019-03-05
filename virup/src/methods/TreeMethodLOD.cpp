#include "methods/TreeMethodLOD.hpp"

TreeMethodLOD::TreeMethodLOD()
    : TreeMethodLOD("invsq")
{
}

TreeMethodLOD::TreeMethodLOD(std::string const& shadersCommonName)
    : TreeMethodLOD(shadersCommonName, shadersCommonName)
{
}

TreeMethodLOD::TreeMethodLOD(std::string const& vertexShaderPath,
                             std::string const& fragmentShaderPath)
    : Method(vertexShaderPath, fragmentShaderPath)
    , currentTanAngle(1.0f)
{
	// init chrono
	// gettimeofday(&t0, NULL);

	// setting PID controller
	ctrl.Kp = -0.000001f;
	ctrl.Ki = -0.0000001f;
	ctrl.Kd = 0.00001f;

	ctrl.controlVariable = &currentTanAngle;

	ctrl.tol = 500;

	showdm = false;
}

void TreeMethodLOD::init(std::vector<float> const& gazVertices,
                         std::vector<float> const& starsVertices,
                         std::vector<float> const& darkMatterVertices)
{
	if(!gazVertices.empty() && gazTree == nullptr)
	{
		gazTree = new OctreeLOD(shaderProgram);
		gazTree->init(gazVertices);
	}
	if(!starsVertices.empty() && starsTree == nullptr)
	{
		starsTree = new OctreeLOD(shaderProgram);
		starsTree->init(starsVertices);
	}
	if(!darkMatterVertices.empty() && darkMatterTree == nullptr)
	{
		darkMatterTree = new OctreeLOD(shaderProgram);
		darkMatterTree->init(darkMatterVertices);
	}
}

void TreeMethodLOD::init(std::string const& gazPath,
                         std::string const& starsPath,
                         std::string const& darkMatterPath)
{
	if(!gazPath.empty() && gazTree == nullptr)
	{
		std::cout << "Loading gaz..." << std::endl;
		auto file = new std::ifstream();
		file->open(gazPath, std::fstream::in | std::fstream::binary);
		gazTree = new OctreeLOD(shaderProgram);
		gazTree->init(*file);
		gazTree->setFile(file);
		/*// update bbox
		gazTree->readBBoxes(*file);*/
		gazTree->readData(*file);
		std::cout << "Gaz loaded..." << std::endl;
	}
	if(!starsPath.empty() && starsTree == nullptr)
	{
		std::cout << "Loading stars..." << std::endl;
		auto file = new std::ifstream();
		file->open(starsPath, std::fstream::in | std::fstream::binary);
		starsTree = new OctreeLOD(shaderProgram);
		starsTree->init(*file);
		starsTree->setFile(file);
		/*// update bbox
		starsTree->readBBoxes(*file);*/
		starsTree->readData(*file);
		std::cout << "Stars loaded..." << std::endl;
	}
	if(!darkMatterPath.empty() && darkMatterTree == nullptr)
	{
		std::cout << "Loading dark matter..." << std::endl;
		auto file = new std::ifstream();
		file->open(darkMatterPath, std::fstream::in | std::fstream::binary);
		darkMatterTree = new OctreeLOD(shaderProgram);
		darkMatterTree->init(*file);
		darkMatterTree->setFile(file);
		/*// update bbox
		darkMatterTree->readBBoxes(*file);*/
		darkMatterTree->readData(*file);
		std::cout << "Dark matter loaded..." << std::endl;
	}

	// preload data to fill VRAM giving priority to top levels
	unsigned int lvlToLoad(0);
	while(lvlToLoad == 0)
	{
		if(gazTree != nullptr)
		{
			if(!gazTree->preloadLevel(lvlToLoad))
			{
				break;
			}
		}
		if(starsTree != nullptr)
		{
			if(!starsTree->preloadLevel(lvlToLoad))
			{
				break;
			}
		}
		if(darkMatterTree != nullptr)
		{
			if(!darkMatterTree->preloadLevel(lvlToLoad))
			{
				break;
			}
		}
		++lvlToLoad;
	}
}

std::pair<float, std::string> humanReadable(int64_t bytes)
{
	float fbytes(bytes);
	if(bytes < 1024)
	{
		return std::pair<float, std::string>(fbytes, " bytes");
	}
	if(bytes < 1024 * 1024)
	{
		return std::pair<float, std::string>(fbytes / 1024, " Kib");
	}
	if(bytes < 1024 * 1024 * 1024)
	{
		return std::pair<float, std::string>(fbytes / (1024 * 1024), " Mib");
	}
	return std::pair<float, std::string>(fbytes / (1024 * 1024 * 1024), " Gib");
}

void TreeMethodLOD::render(Camera const& camera, QMatrix4x4 const& model)
{
	/*struct timeval tf;
	gettimeofday(&tf, NULL);
	uint64_t dt = (tf.tv_sec * 1000000) + tf.tv_usec - t0.tv_usec
	              - (t0.tv_sec * 1000000);
	gettimeofday(&t0, NULL);

	float dtf = dt;
	if(camera.currentFrameTiming != 0)*/
	float dtf = camera.currentFrameTiming * 1000000.f;
	/*ctrl.targetMeasure = &dtf;
	ctrl.setPoint          = 1000000.0f / camera.targetFPS;
	ctrl.update(dt);

	// we don't want points to discard others on depth test, because with
	// transparency they should all be drawn; but we still want to be occluded
	// by solid materials (like controllers for example) so depth test is still
	// enabled*/

	// old way
	float coeff((dtf - 1000000.0f / camera.targetFPS) / 5000000.0f);
	coeff = coeff > 1.f / 90.f ? 1.f / 90.f : coeff;
	currentTanAngle += coeff;
	currentTanAngle = currentTanAngle > 1.2f ? 1.2f : currentTanAngle;
	currentTanAngle = currentTanAngle < 0.05f ? 0.05f : currentTanAngle;

	GLHandler::beginTransparent();
	GLHandler::setShaderParam(shaderProgram, "color",
	                          QVector3D(1.0f, 1.0f, 1.0f));
	GLHandler::setUpRender(shaderProgram, model);
	GLHandler::setShaderParam(shaderProgram, "alpha", model(0, 0) * getAlpha());
	GLHandler::setShaderParam(shaderProgram, "view",
	                          camera.hmdScaledSpaceToWorldTransform().inverted()
	                              * model);
	GLHandler::setShaderParam(
	    shaderProgram, "color",
	    QSettings().value("data/gazcolor").value<QColor>());
	unsigned int rendered = 0;
	if(gazTree != nullptr)
	{
		rendered += gazTree->renderAboveTanAngle(currentTanAngle, camera, model,
		                                         1000000000);
	}
	GLHandler::setShaderParam(
	    shaderProgram, "color",
	    QSettings().value("data/starscolor").value<QColor>());
	if(starsTree != nullptr)
	{
		rendered += starsTree->renderAboveTanAngle(currentTanAngle, camera,
		                                           model, 1000000000);
	}
	GLHandler::setShaderParam(
	    shaderProgram, "color",
	    QSettings().value("data/darkmattercolor").value<QColor>());
	if(darkMatterTree != nullptr && showdm)
	{
		rendered += darkMatterTree->renderAboveTanAngle(currentTanAngle, camera,
		                                                model, 1000000000);
	}
	GLHandler::endTransparent();

	std::pair<float, std::string> h(humanReadable(OctreeLOD::getUsedMem()));
	std::cout.precision(4);
	std::cout << "VRAM : " << h.first << h.second << " used... " << rendered
	          << " points rendered..."
	          << "\r" << std::fflush(stdout);
}

void TreeMethodLOD::cleanUp()
{
	if(gazTree != nullptr)
	{
		if(gazTree->getFile() != nullptr)
		{
			delete gazTree->getFile();
		}
		delete gazTree;
	}
	gazTree = nullptr;
	if(starsTree != nullptr)
	{
		if(starsTree->getFile() != nullptr)
		{
			delete starsTree->getFile();
		}
		delete starsTree;
	}
	starsTree = nullptr;
	if(darkMatterTree != nullptr)
	{
		if(darkMatterTree->getFile() != nullptr)
		{
			delete darkMatterTree->getFile();
		}
		delete darkMatterTree;
	}
	darkMatterTree = nullptr;
}

TreeMethodLOD::~TreeMethodLOD()
{
	cleanUp();
}
