#ifndef BASELINEMETHOD_H
#define BASELINEMETHOD_H

#include <liboctree/Octree.hpp>

#include "GLHandler.hpp"
#include "Method.hpp"

class BaseLineMethod : public Method
{
	Q_OBJECT
  public:
	BaseLineMethod();
	BaseLineMethod(std::string const& shadersCommonName);
	BaseLineMethod(std::string const& vertexShaderPath,
	               std::string const& fragmentShaderPath);
	virtual void render(Camera const& camera, QMatrix4x4 const& model) override;
	~BaseLineMethod();

  public slots:
	virtual std::string getName() const override { return "Base Line"; };
	virtual void init(std::vector<float>& gazVertices,
	                  std::vector<float>& starsVertices,
	                  std::vector<float>& darkMatterVertices) override;
	virtual void init(std::string const& gazPath, std::string const& starsPath,
	                  std::string const& darkMatterPath) override;
	virtual void cleanUp() override;

  private:
	GLHandler::Mesh mesh = {};

  protected:
	unsigned int pointSize;
};

#endif // BASELINEMETHOD_H
