#ifndef TREEMETHODLOD_H
#define TREEMETHODLOD_H

#include <QElapsedTimer>
#include <QProgressDialog>
#include <chrono>
#include <future>
#include <thread>

#include "Method.hpp"
#include "OctreeLOD.hpp"
#include "PIDController.hpp"
#include "VolumetricModel.hpp"

class TreeMethodLOD : public Method
{
	Q_OBJECT
  public:
	TreeMethodLOD();
	TreeMethodLOD(std::string const& shadersCommonName);
	TreeMethodLOD(std::string const& vertexShaderPath,
	              std::string const& fragmentShaderPath);
	virtual std::string getName() const override { return "Tree LOD"; };
	virtual void init(std::vector<float>& gasVertices,
	                  std::vector<float>& starsVertices,
	                  std::vector<float>& darkMatterVertices) override;
	virtual void init(std::string const& gasPath, std::string const& starsPath,
	                  std::string const& darkMatterPath) override;
	virtual BBox getDataBoundingBox() const override;
	virtual void render(Camera const& camera) override;
	void render(Camera const& camera, QMatrix4x4 const& model,
	            QVector3D const& campos);
	virtual void cleanUp() override;
	virtual ~TreeMethodLOD();

  protected:
	VolumetricModel* dustModel = nullptr;
	OctreeLOD* gasTree         = nullptr;
	OctreeLOD* starsTree       = nullptr;
	OctreeLOD* darkMatterTree  = nullptr;
	VolumetricModel* hiiModel  = nullptr;

	// struct timeval t0;
	float currentTanAngle;
	PIDController ctrl;

	// ugly fix for pointSize problems
	bool setPointSize = true;

	static void loadOctreeFromFile(std::string const& path, OctreeLOD** octree,
	                               std::string const& name,
	                               GLShaderProgram const& shaderProgram);
	static void initOctree(OctreeLOD* octree, std::istream* in);
	void setShaderColor(QColor const& color);

	// used to detect too long frames
	QElapsedTimer timer;
};

#endif // TREEMETHOD_H
