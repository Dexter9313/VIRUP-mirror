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

	computeBBox();

	mesh = GLHandler::newMesh();
	GLHandler::setVertices(mesh, data, *shaderProgram, {{"position", 3}});
	dataSize = data.size();
	usedMem() += dataSize * sizeof(float);
	this->data.clear();
	isLoaded = true;
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

unsigned int OctreeLOD::renderAboveTanAngle(float tanAngle,
                                            Camera const& camera,
                                            QMatrix4x4 const& model,
                                            unsigned int maxPoints)
{
	if(camera.shouldBeCulled(bbox, model) && lvl > 0)
	{
		// if(usedMem() > (memLimit() - (int64_t) (10 * 16000 * 3 *
		// sizeof(float))))
		// unload();
		return 0;
	}

	if(!isLoaded)
	{
		// if(usedMem() < memLimit())
		readOwnData(*file);
		// else
		//	return 0;
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
				    tanAngle, camera, model, remaining);
			}
		}
		return maxPoints - remaining;
	}

	if(dataSize / 3 <= maxPoints)
	{
		/*if(isLeaf)
		    GLHandler::setShaderParam(*shaderProgram, "color",
		                              glm::vec3(0.0f, 1.0f, 1.0f));
		else
		{
		    GLHandler::setShaderParam(*shaderProgram, "color",
		                              glm::vec3(1.0f, 1.0f, 1.0f));*/
		if(!isLeaf()
		   && sqrt(static_cast<double>(getTotalDataSize()) / dataSize) <= 4.0)
		{
			GLHandler::setPointSize(static_cast<unsigned int>(
			    sqrt(static_cast<double>(getTotalDataSize()) / dataSize)));
		}
		else if(!isLeaf())
		{
			GLHandler::setPointSize(4);
		}
		else
		{
			GLHandler::setPointSize(1);
		}
		GLHandler::render(mesh);
		return dataSize / 3;
		//}
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
}

float OctreeLOD::currentTanAngle(Camera const& camera,
                                 QMatrix4x4 const& model) const
{
	if(camera.shouldBeCulled(bbox, model))
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
