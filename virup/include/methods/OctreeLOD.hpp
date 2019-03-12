#ifndef OCTREELOD_H
#define OCTREELOD_H

#include <liboctree/Octree.hpp>

#include "Camera.hpp"
#include "GLHandler.hpp"
#include "utils.hpp"

#define MAX_LEAVES_PER_NODE 16000

class OctreeLOD : public Octree
{
  public:
	OctreeLOD(GLHandler::ShaderProgram const& shaderProgram,
	          unsigned int lvl = 0);
	virtual void init(std::vector<float>& data) override;
	virtual void init(std::istream& in) override;
	virtual void init(int64_t file_addr, std::istream& in) override;
	virtual void readOwnData(std::istream& in) override;
	void unload();
	void setFile(std::istream* file);
	std::istream* getFile() { return file; };
	bool preloadLevel(unsigned int lvlToLoad);
	unsigned int renderAboveTanAngle(float tanAngle, Camera const& camera,
	                                 QMatrix4x4 const& model,
	                                 unsigned int maxPoints);
	~OctreeLOD();

	static int64_t getUsedMem() { return usedMem(); };

  protected:
	virtual Octree* newOctree() const override;

  private:
	unsigned int lvl;
	BBox bbox;

	std::istream* file;
	bool isLoaded;
	unsigned int dataSize;
	// total used memory across all instances
	static int64_t& usedMem();
	static const int64_t& memLimit();

	GLHandler::Mesh mesh = {};
	GLHandler::ShaderProgram const* shaderProgram;

	void computeBBox();
	float currentTanAngle(Camera const& camera, QMatrix4x4 const& model) const;
};

#endif // OCTREELOD_H
