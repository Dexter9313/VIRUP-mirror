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

bool OctreeLOD::renderPlanetarySystem   = false;
double OctreeLOD::planetarySysInitScale = 1.0;
Vector3& OctreeLOD::planetarySysInitData()
{
	static Vector3 planetarySysInitData = Vector3();
	return planetarySysInitData;
}

// TODO just draw nothing if vertices.size() == 0 (prevents nullptr tests when
// drawing)

OctreeLOD::OctreeLOD(GLHandler::ShaderProgram const& shaderProgram, Flags flags,
                     unsigned int lvl)
    : Octree(flags)
    , lvl(lvl)
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
				data[i + j] -= localTranslation[j];
			}
		}
	}
	else
	{
		double localScale;
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
		for(size_t i(0); i < data.size(); i += dimPerVertex)
		{
			for(unsigned int j(0); j < 3; ++j)
			{
				data[i + j] *= localScale;
			}
		}
	}
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
			result[i + j] += localTranslation[j];
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
		ramToVideo();
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
                                            unsigned int maxPoints,
                                            bool isStarField)
{
	QMatrix4x4 model(camera.dataToWorldTransform());
	if(camera.shouldBeCulled(bbox, model, true) && lvl > 0)
	{
		if(usedMem() > (memLimit() * 80) / 100)
		{
			unload();
		}
		return 0;
	}

	if(!isLoaded)
	{
		/*if(usedMem() < memLimit())
		{*/
		readOwnData(*file);
		ramToVideo();
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
				    tanAngle, camera, remaining, isStarField);
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

	if(isLeaf())
	{
		QVector3D camposQt = camera.hmdScaledSpaceToWorldTransform()
		                     * QVector3D(0.f, 0.f, 0.f);
		Vector3 campos(camera.worldToDataPosition(Utils::fromQt(camposQt)));

		if(isStarField && camera.scale > 100 && campos[0] > bbox.minx
		   && campos[0] < bbox.maxx && campos[1] > bbox.miny
		   && campos[1] < bbox.maxy && campos[2] > bbox.minz
		   && campos[2] < bbox.maxz)
		{
			Vector3 closest(DBL_MAX, DBL_MAX, DBL_MAX);
			if((campos - closestBackup).length() > neighborDist / 2.0)
			{
				if(absoluteData.empty())
				{
					readOwnData(*file);
					absoluteData = getOwnData();
					data.resize(0);
					data.shrink_to_fit();
				}

				double dist(FLT_MAX);
				for(unsigned int i(0); i < absoluteData.size();
				    i += dimPerVertex)
				{
					Vector3 x(absoluteData[i], absoluteData[i + 1],
					          absoluteData[i + 2]);
					double distx((campos - x).length());
					if(distx < dist)
					{
						closest = x;
						dist    = distx;
					}
				}
			}
			else
			{
				closest = closestBackup;
			}
			localTranslation = closest;
			bool switchedPoint(false);
			if(closest != closestBackup)
			{
				switchedPoint = true;
				closestBackup = closest;

				std::vector<float> vertexData(absoluteData);
				for(unsigned int i(0); i < vertexData.size(); i += dimPerVertex)
				{
					vertexData[i] -= closest[0];
					vertexData[i + 1] -= closest[1];
					vertexData[i + 2] -= closest[2];
				}
				GLHandler::updateVertices(mesh, vertexData);

				Vector3 closestNeighbor(DBL_MAX, DBL_MAX, DBL_MAX);
				neighborDist = DBL_MAX;
				for(unsigned int i(0); i < vertexData.size(); i += dimPerVertex)
				{
					Vector3 x(vertexData[i], vertexData[i + 1],
					          vertexData[i + 2]);
					// it's closest itself !
					if(x.length() == 0.0)
					{
						continue;
					}

					if(x.length() < neighborDist)
					{
						closestNeighbor = x;
						neighborDist    = x.length();
					}
				}
			}

			if(camera.scale * neighborDist > 2
			   && (!starLoaded || switchedPoint))
			{
				// 3.086e+19 m = 1kpc
				double mtokpc(3.24078e-20);
				planetarySysInitScale  = mtokpc;
				planetarySysInitData() = closest;
				initStar();
			}
			else if(camera.scale * neighborDist <= 2 && starLoaded)
			{
				deleteStar();
			}
		}
		else
		{
			absoluteData.resize(0);
			absoluteData.shrink_to_fit();
			closestBackup = Vector3(DBL_MAX, DBL_MAX, DBL_MAX);
			neighborDist  = 0.0;
			deleteStar();
		}
	}

	if(dataSize / dimPerVertex <= maxPoints)
	{
		Vector3 totalTranslation(camera.dataToWorldPosition(localTranslation));
		QMatrix4x4 totalModel;
		totalModel.translate(Utils::toQt(totalTranslation));
		totalModel.scale(camera.scale);

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

	localTranslation = Vector3(bbox.minx, bbox.miny, bbox.minz);
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

void OctreeLOD::initStar()
{
	if(starLoaded)
	{
		return;
	}
	starLoaded            = true;
	renderPlanetarySystem = true;
}

void OctreeLOD::deleteStar()
{
	if(!starLoaded)
	{
		return;
	}
	starLoaded            = false;
	renderPlanetarySystem = false;
}
