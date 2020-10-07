#ifndef OCTREELOD_H
#define OCTREELOD_H

#include <QElapsedTimer>
#include <liboctree/Octree.hpp>
#include <random>

#include "graphics/renderers/OrbitalSystemRenderer.hpp"

#include "Camera.hpp"
#include "GLHandler.hpp"
#include "Primitives.hpp"
#include "math/Vector3.hpp"
#include "physics/blackbody.hpp"
#include "utils.hpp"

#define MAX_LEAVES_PER_NODE 16000

class OctreeLOD : public Octree
{
  public:
	OctreeLOD(GLHandler::ShaderProgram const& shaderProgram,
	          Flags flags = Flags::NONE, unsigned int lvl = 0);
	virtual void init(std::vector<float>& data) override;
	virtual void init(std::istream& in) override;
	virtual void init(int64_t file_addr, std::istream& in) override;
	BBox getBoundingBox() const { return bbox; };
	virtual void readOwnData(std::istream& in) override;
	virtual void readBBox(std::istream& in) override;
	virtual std::vector<float> getOwnData() const override;
	void unload();
	void setFile(std::istream* file);
	std::istream* getFile() { return file; };
	bool preloadLevel(unsigned int lvlToLoad);
	unsigned int renderAboveTanAngle(float tanAngle, Camera const& camera,
	                                 unsigned int maxPoints, bool isStarField,
	                                 float alpha);
	~OctreeLOD();

	static int64_t getUsedMem() { return usedMem(); };
	static int64_t getMemLimit() { return memLimit(); };

	static bool renderPlanetarySystem;
	static Vector3& planetarySysInitData();
	static Vector3& solarSystemDataPos();

  protected:
	virtual Octree* newOctree(Flags flags) const override;

  private:
	unsigned int lvl;
	BBox bbox;

	std::istream* file    = nullptr;
	bool isLoaded         = false;
	unsigned int dataSize = 0;
	// total used memory across all instances
	static int64_t& usedMem();
	static const int64_t& memLimit();

	GLHandler::Mesh mesh = {};
	GLHandler::ShaderProgram const* shaderProgram;

	void computeBBox();
	float currentTanAngle(Camera const& camera, QMatrix4x4 const& model) const;
	void ramToVideo();

	/* PRECISION ENHANCEMENT */
	std::vector<float> absoluteData; // backup data from file
	double neighborDist      = 0.0;
	Vector3 localTranslation = Vector3(0.f, 0.f, 0.f);

	/* PERFORMANCE */
	Vector3 closestBackup = Vector3(DBL_MAX, DBL_MAX, DBL_MAX);
};

#endif // OCTREELOD_H
