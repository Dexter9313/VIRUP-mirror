#include "methods/TreeMethodLOD.hpp"

TreeMethodLOD::TreeMethodLOD()
    : TreeMethodLOD("default", "default")
{
}

TreeMethodLOD::TreeMethodLOD(std::string const& vertexShaderPath,
                             std::string const& fragmentShaderPath)
    : Method(vertexShaderPath, fragmentShaderPath)
    , currentTanAngle(1.0f)
{
	// init chrono
	//gettimeofday(&t0, NULL);

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
	if(gazVertices.size() != 0 && !gazTree)
	{
		gazTree = new OctreeLOD(shaderProgram);
		gazTree->init(gazVertices);
	}
	if(starsVertices.size() != 0 && !starsTree)
	{
		starsTree = new OctreeLOD(shaderProgram);
		starsTree->init(starsVertices);
	}
	if(darkMatterVertices.size() != 0 && !darkMatterTree)
	{
		darkMatterTree = new OctreeLOD(shaderProgram);
		darkMatterTree->init(darkMatterVertices);
	}
}

void TreeMethodLOD::init(std::string const& gazPath,
                         std::string const& starsPath,
                         std::string const& darkMatterPath)
{
	if(gazPath != "" && !gazTree)
	{
		std::cout << "Loading gaz..." << std::endl;
		std::ifstream* file = new std::ifstream();
		file->open(gazPath, std::fstream::in | std::fstream::binary);
		gazTree = new OctreeLOD(shaderProgram);
		gazTree->init(*file);
		gazTree->setFile(file);
		/*// update bbox
		gazTree->readBBoxes(*file);*/
		gazTree->readData(*file);
		std::cout << "Gaz loaded..." << std::endl;
	}
	if(starsPath != "" && !starsTree)
	{
		std::cout << "Loading stars..." << std::endl;
		std::ifstream* file = new std::ifstream();
		file->open(starsPath, std::fstream::in | std::fstream::binary);
		starsTree = new OctreeLOD(shaderProgram);
		starsTree->init(*file);
		starsTree->setFile(file);
		/*// update bbox
		starsTree->readBBoxes(*file);*/
		starsTree->readData(*file);
		std::cout << "Stars loaded..." << std::endl;
	}
	if(darkMatterPath != "" && !darkMatterTree)
	{
		std::cout << "Loading dark matter..." << std::endl;
		std::ifstream* file = new std::ifstream;
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
		if(gazTree)
			if(!gazTree->preloadLevel(lvlToLoad))
				break;
		if(starsTree)
			if(!starsTree->preloadLevel(lvlToLoad))
				break;
		if(darkMatterTree)
			if(!darkMatterTree->preloadLevel(lvlToLoad))
				break;
		++lvlToLoad;
	}
	std::cout << "OK" << std::endl;
}

std::pair<float, std::string> humanReadable(long int bytes)
{
	if(bytes < 1024)
		return std::pair<float, std::string>(bytes, " bytes");
	if(bytes < 1024 * 1024)
		return std::pair<float, std::string>(bytes / (float) 1024, " Kib");
	if(bytes < 1024 * 1024 * 1024)
		return std::pair<float, std::string>(bytes / (float) (1024 * 1024),
		                                     " Mib");
	return std::pair<float, std::string>(bytes / (float) (1024 * 1024 * 1024),
	                                     " Gib");
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
	GLHandler::setShaderParam(shaderProgram, "color", QSettings().value("data/gazcolor").value<QVector3D>());
	unsigned int rendered = 0;
	if(gazTree)
		rendered += gazTree->renderAboveTanAngle(currentTanAngle, camera, model,
		                                         1000000000);
	GLHandler::setShaderParam(shaderProgram, "color", QSettings().value("data/starscolor").value<QVector3D>());
	if(starsTree)
		rendered += starsTree->renderAboveTanAngle(currentTanAngle, camera, model,
		                                           1000000000);
	GLHandler::setShaderParam(shaderProgram, "color", QSettings().value("data/darkmattercolor").value<QVector3D>());
	if(darkMatterTree && showdm)
		rendered += darkMatterTree->renderAboveTanAngle(currentTanAngle, camera, model,
		                                                1000000000);
	GLHandler::endTransparent();

	std::pair<float, std::string> h(humanReadable(OctreeLOD::getUsedMem()));
	std::cout.precision(4);
	std::cout << "VRAM : " << h.first << h.second << " used... " << rendered
	          << " points rendered..."
	          << "\r" << std::fflush(stdout);
}

TreeMethodLOD::~TreeMethodLOD()
{
	if(gazTree != nullptr)
	{
		if(gazTree->getFile())
			delete gazTree->getFile();
		delete gazTree;
	}
	gazTree = nullptr;
	if(starsTree != nullptr)
	{
		if(starsTree->getFile())
			delete starsTree->getFile();
		delete starsTree;
	}
	starsTree = nullptr;
	if(darkMatterTree != nullptr)
	{
		if(darkMatterTree->getFile())
			delete darkMatterTree->getFile();
		delete darkMatterTree;
	}
	darkMatterTree = nullptr;
}
