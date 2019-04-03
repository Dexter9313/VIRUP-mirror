#include "methods/OctreeLOD.hpp"

int64_t& OctreeLOD::usedMem()
{
	static int64_t usedMem(0);
	return usedMem;
}

const int64_t& OctreeLOD::memLimit()
{
	static const int64_t memLimit(
	    static_cast<int64_t>(1000000)
	    * QSettings().value("misc/maxvramusagemb").toInt());
	return memLimit;
}

QElapsedTimer& OctreeLOD::starTimer()
{
	static QElapsedTimer starTimer;
	return starTimer;
}

// TODO just draw nothing if vertices.size() == 0 (prevents nullptr tests when
// drawing)

OctreeLOD::OctreeLOD(GLHandler::ShaderProgram const& shaderProgram, Flags flags,
                     unsigned int lvl)
    : Octree(flags)
    , lvl(lvl)
    , file(nullptr)
    , isLoaded(false)
    , dataSize(0)
    , shaderProgram(&shaderProgram)
{
}

void OctreeLOD::init(std::vector<float>& data)
{
	Octree::init(data);
	computeBBox();
	ramToVideo();
}

void OctreeLOD::init(std::istream& in)
{
	Octree::init(in);
}

void OctreeLOD::init(int64_t file_addr, std::istream& in)
{
	Octree::init(file_addr, in);
}

void OctreeLOD::readOwnData(std::istream& in)
{
	Octree::readOwnData(in);

	if((getFlags() & Flags::NORMALIZED_NODES) == Flags::NONE)
	{
		for(size_t i(0); i < data.size(); i += dimPerVertex)
		{
			for(unsigned int j(0); j < 3; ++j)
			{
				data[i + j] -= localTranslation.at(j);
				data[i + j] /= localScale;
			}
		}
	}
	ramToVideo();
}

void OctreeLOD::readBBox(std::istream& in)
{
	Octree::readBBox(in);
	computeBBox();
}

std::vector<float> OctreeLOD::getOwnData() const
{
	std::vector<float> result(data);
	for(size_t i(0); i < result.size(); i += dimPerVertex)
	{
		for(unsigned int j(0); j < 3; ++j)
		{
			result[i + j] *= localScale;
			result[i + j] += localTranslation.at(j);
		}
	}

	return result;
}

void OctreeLOD::unload()
{
	if(isLoaded)
	{
		usedMem() -= dataSize * sizeof(float);
		dataSize = 0;
		GLHandler::deleteMesh(mesh);
		for(Octree* oct : children)
		{
			if(oct != nullptr)
			{
				dynamic_cast<OctreeLOD*>(oct)->unload();
			}
		}
		isLoaded = false;
	}
}

void OctreeLOD::setFile(std::istream* file)
{
	this->file = file;
	for(Octree* oct : children)
	{
		if(oct != nullptr)
		{
			dynamic_cast<OctreeLOD*>(oct)->setFile(file);
		}
	}
}

bool OctreeLOD::preloadLevel(unsigned int lvlToLoad)
{
	if(usedMem() >= memLimit())
	{
		return false;
	}
	if(lvlToLoad == 0)
	{
		readOwnData(*file);
	}
	else
	{
		for(Octree* oct : children)
		{
			if(oct != nullptr)
			{
				if(!dynamic_cast<OctreeLOD*>(oct)->preloadLevel(lvlToLoad - 1))
				{
					return false;
				}
			}
		}
	}
	return true;
}

unsigned int OctreeLOD::renderAboveTanAngle(
    float tanAngle, Camera const& camera, double scale,
    std::array<double, 3> const& translation, unsigned int maxPoints)
{
	QMatrix4x4 model;
	model.translate(QVector3D(translation[0], translation[1], translation[2]));
	model.scale(scale);
	if(camera.shouldBeCulled(bbox, model, true) && lvl > 0)
	{
		if(usedMem() > (memLimit() * 80) / 100)
		{
			unload();
		}
		return 0;
	}

	std::array<double, 3> totalTranslation = {};
	for(unsigned int i(0); i < 3; ++i)
	{
		totalTranslation.at(i)
		    = translation.at(i) + scale * localTranslation.at(i);
	}
	double totalScale(scale * localScale);

	QMatrix4x4 totalModel;
	totalModel.translate(QVector3D(totalTranslation[0], totalTranslation[1],
	                               totalTranslation[2]));
	totalModel.scale(totalScale);

	if(!isLoaded)
	{
		/*if(usedMem() < memLimit())
		{*/
		readOwnData(*file);
		/*}
		else
		{
		    return 0;
		}*/
	}

	if(currentTanAngle(camera, model) > tanAngle && !isLeaf())
	{
		unsigned int remaining = maxPoints;
		// RENDER SUBTREES
		for(Octree* oct : children)
		{
			if(oct != nullptr)
			{
				remaining -= dynamic_cast<OctreeLOD*>(oct)->renderAboveTanAngle(
				    tanAngle, camera, scale, translation, remaining);
			}
		}
		return maxPoints - remaining;
	}

	if(!isLeaf() && usedMem() > (memLimit() * 80) / 100)
	{
		// unload children
		for(Octree* oct : children)
		{
			if(oct != nullptr)
			{
				dynamic_cast<OctreeLOD*>(oct)->unload();
			}
		}
	}

	if(isLeaf() && totalScale > 1000)
	{
		QVector3D campos = model.inverted()
		                   * camera.hmdScaledSpaceToWorldTransform()
		                   * QVector3D(0.f, 0.f, 0.f);
		if(campos.x() > bbox.minx && campos.x() < bbox.maxx
		   && campos.y() > bbox.miny && campos.y() < bbox.maxy
		   && campos.z() > bbox.minz && campos.z() < bbox.maxz)
		{
			// Maybe don't do all this every frame...
			Octree::readOwnData(*file);
			QVector3D closest(FLT_MAX, FLT_MAX, FLT_MAX);
			float dist(FLT_MAX);
			for(unsigned int i(0); i < data.size(); i += 3)
			{
				QVector3D x(data[i], data[i + 1], data[i + 2]);
				float distx(campos.distanceToPoint(x));
				if(distx < dist)
				{
					closest = x;
					dist    = distx;
				}
			}
			localTranslation[0] = closest.x();
			localTranslation[1] = closest.y();
			localTranslation[2] = closest.z();
			for(unsigned int i(0); i < data.size(); i += 3)
			{
				data[i] -= closest.x();
				data[i] /= localScale;
				data[i + 1] -= closest.y();
				data[i + 1] /= localScale;
				data[i + 2] -= closest.z();
				data[i + 2] /= localScale;
			}
			GLHandler::updateVertices(mesh, data);
			data.resize(0);
			data.shrink_to_fit();

			if(starLoaded)
			{
				renderStar(totalModel);
			}
			else
			{
				initStar(1.f / 500000.f);
				renderStar(totalModel);
			}
			GLHandler::useShader(*shaderProgram);
		}
		else if(starLoaded)
		{
			deleteStar();
		}
	}
	else if(starLoaded)
	{
		deleteStar();
	}

	if(dataSize / dimPerVertex <= maxPoints)
	{
		GLHandler::setShaderParam(
		    *shaderProgram, "view",
		    camera.hmdScaledSpaceToWorldTransform().inverted() * totalModel);
		GLHandler::setUpRender(*shaderProgram, totalModel);
		GLHandler::render(mesh);
		return dataSize / dimPerVertex;
	}
	return 0;
}

void OctreeLOD::computeBBox()
{
	bbox.minx     = minX;
	bbox.maxx     = maxX;
	bbox.miny     = minY;
	bbox.maxy     = maxY;
	bbox.minz     = minZ;
	bbox.maxz     = maxZ;
	bbox.diameter = sqrtf((bbox.maxx - bbox.minx) * (bbox.maxx - bbox.minx)
	                      + (bbox.maxy - bbox.miny) * (bbox.maxy - bbox.miny)
	                      + (bbox.maxz - bbox.minz) * (bbox.maxz - bbox.minz));

	bbox.mid.setX((bbox.maxx + bbox.minx) / 2.0f);
	bbox.mid.setY((bbox.maxy + bbox.miny) / 2.0f);
	bbox.mid.setZ((bbox.maxz + bbox.minz) / 2.0f);

	localTranslation[0] = bbox.minx;
	localTranslation[1] = bbox.miny;
	localTranslation[2] = bbox.minz;
	if((bbox.maxx - bbox.minx >= bbox.maxy - bbox.miny)
	   && (bbox.maxx - bbox.minx >= bbox.maxz - bbox.minz))
	{
		localScale = bbox.maxx - bbox.minx;
	}
	else if(bbox.maxy - bbox.miny >= bbox.maxz - bbox.minz)
	{
		localScale = bbox.maxy - bbox.miny;
	}
	else
	{
		localScale = bbox.maxz - bbox.minz;
	}
}

float OctreeLOD::currentTanAngle(Camera const& camera,
                                 QMatrix4x4 const& model) const
{
	if(camera.shouldBeCulled(bbox, model, true))
	{
		return 0.f;
	}

	return bbox.diameter * model.column(0).x()
	       / (model * bbox.mid)
	             .distanceToPoint(camera.hmdScaledSpaceToWorldTransform()
	                              * QVector3D(0.f, 0.f, 0.f));
}

void OctreeLOD::ramToVideo()
{
	mesh = GLHandler::newMesh();
	std::vector<QPair<const char*, unsigned int>> mapping = {{"position", 3}};
	std::vector<QPair<const char*, std::vector<float>>> unused;
	if((getFlags() & Flags::STORE_RADIUS) != Flags::NONE)
	{
		mapping.emplace_back("radius", 1);
	}
	else
	{
		unused.emplace_back("radius", std::vector<float>{1.f});
	}
	if((getFlags() & Flags::STORE_LUMINOSITY) != Flags::NONE)
	{
		mapping.emplace_back("luminosity", 1);
	}
	else
	{
		unused.emplace_back("luminosity", std::vector<float>{1.f});
	}

	GLHandler::setShaderUnusedAttributesValues(*shaderProgram, unused);
	GLHandler::setVertices(mesh, data, *shaderProgram, mapping);
	dataSize = data.size();
	usedMem() += dataSize * sizeof(float);
	data.resize(0);
	data.shrink_to_fit();
	isLoaded = true;
}

Octree* OctreeLOD::newOctree(Flags flags) const
{
	return new OctreeLOD(*shaderProgram, flags, lvl + 1);
}

OctreeLOD::~OctreeLOD()
{
	unload();
}

void OctreeLOD::initStar(float radius)
{
	if(starLoaded)
	{
		return;
	}
	starLoaded = true;

	std::random_device rd;
	std::mt19937 generator(rd());
	std::exponential_distribution<double> d(0.0005);

	starShader = GLHandler::newShader("star");
	GLHandler::setShaderParam(starShader, "temperature", 1000 + d(generator));
	GLHandler::setShaderParam(
	    starShader, "blackbodyBoundaries",
	    QVector2D(blackbody_min_temp, blackbody_max_temp));

	starMesh = Primitives::newUnitSphere(starShader, 50, 50);

	starTex = GLHandler::newTexture(
	    (blackbody_max_temp - blackbody_min_temp) / blackbody_temp_step + 1,
	    // NOLINTNEXTLINE(hicpp-no-array-decay)
	    blackbody_red, blackbody_green, blackbody_blue);

	starModel = QMatrix4x4();
	starModel.scale(radius);

	starTimer().restart();
}

void OctreeLOD::renderStar(QMatrix4x4 const& model)
{
	GLHandler::endTransparent();
	GLHandler::setShaderParam(
	    starShader, "time", static_cast<float>(starTimer().elapsed()) / 1000.f);
	GLHandler::setUpRender(starShader, model * starModel);
	GLHandler::useTextures({starTex});
	GLHandler::render(starMesh);
	GLHandler::beginTransparent();
}

void OctreeLOD::deleteStar()
{
	if(!starLoaded)
	{
		return;
	}
	GLHandler::deleteTexture(starTex);
	GLHandler::deleteMesh(starMesh);
	GLHandler::deleteShader(starShader);
	starLoaded = false;
}
