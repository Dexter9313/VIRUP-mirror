#ifndef TREEMETHODLOD_H
#define TREEMETHODLOD_H

#include <QProgressDialog>
#include <chrono>
#include <future>
#include <thread>

#include "Method.hpp"
#include "OctreeLOD.hpp"
#include "PIDController.hpp"

class TreeMethodLOD : public Method
{
	Q_OBJECT
  public:
	TreeMethodLOD();
	TreeMethodLOD(std::string const& shadersCommonName);
	TreeMethodLOD(std::string const& vertexShaderPath,
	              std::string const& fragmentShaderPath);
	virtual std::string getName() const override { return "Tree LOD"; };
	virtual void init(std::vector<float>& gazVertices,
	                  std::vector<float>& starsVertices,
	                  std::vector<float>& darkMatterVertices) override;
	virtual void init(std::string const& gazPath, std::string const& starsPath,
	                  std::string const& darkMatterPath) override;
	virtual BBox getDataBoundingBox() const override;
	virtual void render(Camera const& camera, double scale,
	                    std::array<double, 3> const& translation) override;
	virtual void cleanUp() override;
	virtual ~TreeMethodLOD();

  protected:
	OctreeLOD* gazTree        = nullptr;
	OctreeLOD* starsTree      = nullptr;
	OctreeLOD* darkMatterTree = nullptr;
	// struct timeval t0;
	float currentTanAngle;
	PIDController ctrl;

	// ugly fix for pointSize problems
	bool setPointSize = true;

	static void initOctree(OctreeLOD* octree, std::istream* in);
};

#endif // TREEMETHOD_H
