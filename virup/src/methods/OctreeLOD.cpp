#include "methods/OctreeLOD.hpp"

int64_t OctreeLOD::usedMem        = 0;
const int64_t OctreeLOD::memLimit = (((int64_t) 1000000) * QSettings().value("misc/maxvramusagemb").toInt());

// TODO just draw nothing if vertices.size() == 0 (prevents nullptr tests when
// drawing)

OctreeLOD::OctreeLOD(GLHandler::ShaderProgram const& shaderProgram,
                     unsigned int lvl)
    : Octree()
    , lvl(lvl)
    , file(nullptr)
    , isLoaded(false)
    , shaderProgram(&shaderProgram)
{
}

void OctreeLOD::init(std::vector<float> data)
{
	Octree::init(data);

	computeBBox();

	GLHandler::setVertices(mesh, data, *shaderProgram, {{"position", 3}});
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
	usedMem += mesh.vboSize * 3 * sizeof(float);
	this->data.clear();
	isLoaded = true;
}

void OctreeLOD::unload()
{
	if(isLoaded)
	{
		usedMem -= mesh.vboSize * 3 * sizeof(float);
		GLHandler::deleteMesh(mesh);
		for(Octree* oct : children)
			if(oct)
				static_cast<OctreeLOD*>(oct)->unload();
		isLoaded = false;
	}
}

void OctreeLOD::setFile(std::istream* file)
{
	this->file = file;
	for(Octree* oct : children)
		if(oct)
			static_cast<OctreeLOD*>(oct)->setFile(file);
}

bool OctreeLOD::preloadLevel(unsigned int lvlToLoad)
{
	if(usedMem >= memLimit)
		return false;
	if(lvlToLoad == 0)
		readOwnData(*file);
	else
	{
		for(Octree* oct : children)
		{
			if(oct)
			{
				if(!static_cast<OctreeLOD*>(oct)->preloadLevel(lvlToLoad - 1))
					return false;
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
		// if(usedMem > (memLimit - (int64_t) (10 * 16000 * 3 *
		// sizeof(float))))
		//unload();
		return 0;
	}

	if(!isLoaded)
	{
		// if(usedMem < memLimit)
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
			if(oct)
				remaining -= static_cast<OctreeLOD*>(oct)->renderAboveTanAngle(
				    tanAngle, camera, model, remaining);
		}
		return maxPoints - remaining;
	}
	else if(mesh.vboSize <= maxPoints)
	{
		/*if(isLeaf)
		    GLHandler::setShaderParam(*shaderProgram, "color",
		                              glm::vec3(0.0f, 1.0f, 1.0f));
		else
		{
		    GLHandler::setShaderParam(*shaderProgram, "color",
		                              glm::vec3(1.0f, 1.0f, 1.0f));*/
		if(!isLeaf() && sqrt((getTotalDataSize()/3) / mesh.vboSize) <= 4)
			GLHandler::setPointSize(sqrt((getTotalDataSize()/3) / mesh.vboSize));
		else if(!isLeaf())
			GLHandler::setPointSize(4);
		else
			GLHandler::setPointSize(1);
		GLHandler::render(mesh);
		return mesh.vboSize;
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
	bbox.diameter = sqrt((bbox.maxx - bbox.minx) * (bbox.maxx - bbox.minx)
	                     + (bbox.maxy - bbox.miny) * (bbox.maxy - bbox.miny)
	                     + (bbox.maxz - bbox.minz) * (bbox.maxz - bbox.minz));

	bbox.mid.setX((bbox.maxx + bbox.minx) / 2.0f);
	bbox.mid.setY((bbox.maxy + bbox.miny) / 2.0f);
	bbox.mid.setZ((bbox.maxz + bbox.minz) / 2.0f);
}

float OctreeLOD::currentTanAngle(Camera const& camera, QMatrix4x4 const& model) const
{
	if(camera.shouldBeCulled(bbox, model))
		return 0.f;

	return bbox.diameter * model.constData()[0]
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
