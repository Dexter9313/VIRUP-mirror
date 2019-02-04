#ifndef BASELINEMETHOD_H
#define BASELINEMETHOD_H

#include <liboctree/Octree.hpp>

#include "../GLHandler.hpp"
#include "Method.hpp"

class BaseLineMethod : public Method
{
  public:
	BaseLineMethod();
	BaseLineMethod(std::string const& vertexShaderPath,
	               std::string const& fragmentShaderPath);
	virtual std::string getName() const override { return "Base Line"; };
	virtual void init(std::vector<float> const& gazVertices,
	                  std::vector<float> const& starsVertices,
	                  std::vector<float> const& darkMatterVertices) override;
	virtual void init(std::string const& gazPath, std::string const& starsPath,
	                  std::string const& darkMatterPath) override;
	virtual void render(Camera const& camera, QMatrix4x4 const& model) override;
	~BaseLineMethod();

  private:
	GLHandler::Mesh mesh;

  protected:
	unsigned int pointSize;
};

#endif // BASELINEMETHOD_H
