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

OctreeLOD::OctreeLOD(GLHandler::ShaderProgram const& shaderProgram,
                     unsigned int lvl)
    : lvl(lvl)
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

	GLHandler::setVertices(mesh, data, *shaderProgram, {{"position", 3}});
	dataSize = data.size();
	this->data.clear();
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

	for(unsigned int i(0); i < data.size(); i += 3)
	{
		data[i] -= bbox.minx;
		data[i] /= localScale;
		data[i + 1] -= bbox.miny;
		data[i + 1] /= localScale;
		data[i + 2] -= bbox.minz;
		data[i + 2] /= localScale;
	}

	mesh = GLHandler::newMesh();
	GLHandler::setVertices(mesh, data, *shaderProgram, {{"position", 3}});
	dataSize = data.size();
	usedMem() += dataSize * sizeof(float);
	data.resize(0);
	data.shrink_to_fit();
	isLoaded = true;
}

void OctreeLOD::readBBox(std::istream& in)
{
	Octree::readBBox(in);
	computeBBox();
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

	if(dataSize / 3 <= maxPoints)
	{
		GLHandler::setPointSize(1);

		GLHandler::setShaderParam(
		    *shaderProgram, "view",
		    camera.hmdScaledSpaceToWorldTransform().inverted() * totalModel);
		GLHandler::setUpRender(*shaderProgram, totalModel);
		GLHandler::render(mesh);
		return dataSize / 3;
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

Octree* OctreeLOD::newOctree() const
{
	return new OctreeLOD(*shaderProgram, lvl + 1);
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
