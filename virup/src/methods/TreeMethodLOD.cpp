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
	timer.start();
	// init chrono
	// gettimeofday(&t0, NULL);

	// setting PID controller
	ctrl.Kp = -0.000001f;
	ctrl.Ki = -0.0000001f;
	ctrl.Kd = 0.00001f;

	ctrl.controlVariable = &currentTanAngle;

	ctrl.tol = 500;

	showdm = true;

	GLHandler::setPointSize(1);
}

void TreeMethodLOD::init(std::vector<float>& gasVertices,
                         std::vector<float>& starsVertices,
                         std::vector<float>& darkMatterVertices)
{
	if(!gasVertices.empty() && gasTree == nullptr)
	{
		gasTree = new OctreeLOD(shaderProgram);
		gasTree->init(gasVertices);
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

void TreeMethodLOD::init(std::string const& gasPath,
                         std::string const& starsPath,
                         std::string const& darkMatterPath)
{
	if(!gasPath.empty() && gasTree == nullptr)
	{
		loadOctreeFromFile(gasPath, &gasTree, "Gas", shaderProgram);
	}
	if(!starsPath.empty() && starsTree == nullptr)
	{
		loadOctreeFromFile(starsPath, &starsTree, "Stars", shaderProgram);
	}
	if(!darkMatterPath.empty() && darkMatterTree == nullptr)
	{
		loadOctreeFromFile(darkMatterPath, &darkMatterTree, "Dark matter",
		                   shaderProgram);
	}

	// preload data to fill VRAM giving priority to top levels
	uint64_t max(OctreeLOD::getMemLimit());

	uint64_t wholeData(0);
	if(gasTree != nullptr)
	{
		wholeData += gasTree->getTotalDataSize();
	}
	if(starsTree != nullptr)
	{
		wholeData += starsTree->getTotalDataSize();
	}
	if(darkMatterTree != nullptr)
	{
		wholeData += darkMatterTree->getTotalDataSize();
	}

	wholeData *= sizeof(float);

	QProgressDialog progress(tr("Preloading trees data..."), QString(), 0,
	                         max < wholeData ? max : wholeData);
	progress.setMinimumDuration(0);
	progress.setValue(0);

	unsigned int lvlToLoad(0);
	while(lvlToLoad < 10)
	{
		if(gasTree != nullptr)
		{
			if(!gasTree->preloadLevel(lvlToLoad))
			{
				break;
			}
			QCoreApplication::processEvents();
			progress.setValue(OctreeLOD::getUsedMem());
		}
		if(starsTree != nullptr)
		{
			if(!starsTree->preloadLevel(lvlToLoad))
			{
				break;
			}
			QCoreApplication::processEvents();
			progress.setValue(OctreeLOD::getUsedMem());
		}
		if(darkMatterTree != nullptr)
		{
			if(!darkMatterTree->preloadLevel(lvlToLoad))
			{
				break;
			}
			QCoreApplication::processEvents();
			progress.setValue(OctreeLOD::getUsedMem());
		}
		++lvlToLoad;
	}
}

BBox TreeMethodLOD::getDataBoundingBox() const
{
	std::vector<BBox> bboxes;
	if(gasTree != nullptr)
	{
		bboxes.push_back(gasTree->getBoundingBox());
	}
	if(starsTree != nullptr)
	{
		bboxes.push_back(starsTree->getBoundingBox());
	}
	if(darkMatterTree != nullptr)
	{
		bboxes.push_back(darkMatterTree->getBoundingBox());
	}
	return globalBBox(bboxes);
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

void TreeMethodLOD::render(Camera const& camera)
{
	render(camera, camera.dataToWorldTransform(),
	       Utils::toQt(camera.getTruePosition()));
}

void TreeMethodLOD::render(Camera const& camera, QMatrix4x4 const& model,
                           QVector3D const& campos)
{
	if(setPointSize)
	{
		GLHandler::setPointSize(1);
	}
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

	// if something very bad happened regarding last frame rendering
	if(timer.restart() > 200)
	{
		currentTanAngle = 1.2f;
	}

	GLHandler::beginTransparent(GL_ONE, GL_ONE);
	shaderProgram.setUnusedAttributesValues(
	    {{"color", std::vector<float>{1.0f, 1.0f, 1.0f}}});
	GLHandler::setUpRender(shaderProgram, model);
	shaderProgram.setUniform("pixelSolidAngle", camera.pixelSolidAngle());
	unsigned int rendered = 0;
	if(gasTree != nullptr)
	{
		if((gasTree->getFlags() & Octree::Flags::STORE_COLOR)
		   == Octree::Flags::NONE)
		{
			setShaderColor(QSettings().value("data/gazcolor").value<QColor>());
		}
		rendered += gasTree->renderAboveTanAngle(currentTanAngle, camera, model,
		                                         campos, 100000000, false,
		                                         getAlpha());
	}
	if(starsTree != nullptr)
	{
		if((starsTree->getFlags() & Octree::Flags::STORE_COLOR)
		   == Octree::Flags::NONE)
		{
			setShaderColor(
			    QSettings().value("data/starscolor").value<QColor>());
		}
		rendered += starsTree->renderAboveTanAngle(currentTanAngle, camera,
		                                           model, campos, 100000000,
		                                           true, getAlpha());
	}
	if(darkMatterTree != nullptr && showdm)
	{
		if((darkMatterTree->getFlags() & Octree::Flags::STORE_COLOR)
		   == Octree::Flags::NONE)
		{
			setShaderColor(
			    QSettings().value("data/darkmattercolor").value<QColor>());
		}
		rendered += darkMatterTree->renderAboveTanAngle(
		    currentTanAngle, camera, model, campos, 100000000, false,
		    getAlpha());
	}
	GLHandler::endTransparent();

	(void) rendered;

	/*std::pair<float, std::string> h(humanReadable(OctreeLOD::getUsedMem()));
	std::cout.precision(4);
	std::cout << "VRAM : " << h.first << h.second << " used... " << rendered
	          << " points rendered..."
	          << "\r" << std::fflush(stdout);*/
}

void TreeMethodLOD::cleanUp()
{
	if(gasTree != nullptr)
	{
		if(gasTree->getFile() != nullptr)
		{
			delete gasTree->getFile();
		}
		delete gasTree;
	}
	gasTree = nullptr;
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

void TreeMethodLOD::loadOctreeFromFile(std::string const& path,
                                       OctreeLOD** octree,
                                       std::string const& name,
                                       GLShaderProgram const& shaderProgram)
{
	std::cout << "Loading " + name + " octree..." << std::endl;
	auto file = new std::ifstream();
	file->open(path, std::fstream::in | std::fstream::binary);
	*octree = new OctreeLOD(shaderProgram);

	// Init tree with progress bar
	int64_t cursor(file->tellg());
	int64_t size;
	brw::read(*file, size);
	file->seekg(cursor);
	size *= -1;

	QProgressDialog progress(tr("Loading %1 tree structure").arg(name.c_str()),
	                         QString(), 0, size);
	progress.setMinimumDuration(0);
	progress.setValue(0);

	auto future = std::async(std::launch::async, &initOctree, *octree, file);

	while(future.wait_for(std::chrono::duration<int, std::milli>(100))
	      != std::future_status::ready)
	{
		QCoreApplication::processEvents();
		if(file->tellg() < size)
		{
			progress.setValue(file->tellg());
		}
	}

	(*octree)->setFile(file);
	// update bbox
	progress.setLabelText(
	    tr("Loading %1 tree bounding boxes...").arg(name.c_str()));
	QCoreApplication::processEvents();
	(*octree)->readBBoxes(*file);
	// (*octree)->readData(*file);
	std::cout << name << " loaded..." << std::endl;
}

void TreeMethodLOD::initOctree(OctreeLOD* octree, std::istream* in)
{
	octree->init(*in);
}

void TreeMethodLOD::setShaderColor(QColor const& color)
{
	shaderProgram.setUnusedAttributesValues(
	    {{"color",
	      {float(color.redF()), float(color.greenF()), float(color.blueF())}}});
}

TreeMethodLOD::~TreeMethodLOD()
{
	if(gasTree != nullptr)
	{
		if(gasTree->getFile() != nullptr)
		{
			delete gasTree->getFile();
		}
		delete gasTree;
	}
	gasTree = nullptr;
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
