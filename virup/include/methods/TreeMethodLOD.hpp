#ifndef TREEMETHODLOD_H
#define TREEMETHODLOD_H

#include "Method.hpp"
#include "OctreeLOD.hpp"
#include "PIDController.hpp"

class TreeMethodLOD : public Method
{
  public:
	TreeMethodLOD();
	TreeMethodLOD(std::string const& shadersCommonName);
	TreeMethodLOD(std::string const& vertexShaderPath,
	              std::string const& fragmentShaderPath);
	virtual std::string getName() const override { return "Tree LOD"; };
	virtual void init(std::vector<float> const& gazVertices,
	                  std::vector<float> const& starsVertices,
	                  std::vector<float> const& darkMatterVertices) override;
	virtual void init(std::string const& gazPath, std::string const& starsPath,
	                  std::string const& darkMatterPath) override;
	virtual void render(Camera const& camera, QMatrix4x4 const& model) override;
	virtual ~TreeMethodLOD();

  protected:
	OctreeLOD* gazTree        = nullptr;
	OctreeLOD* starsTree      = nullptr;
	OctreeLOD* darkMatterTree = nullptr;
	// struct timeval t0;
	float currentTanAngle;
	PIDController ctrl;
};

#endif // TREEMETHOD_H
