#ifndef METHOD_H
#define METHOD_H

#include <string>
#include <vector>

#include "../Camera.hpp"
#include "../GLHandler.hpp"
#include "../utils.hpp"

class Method
{
  public:
	Method(std::string const& vertexShaderPath,
	       std::string const& fragmentShaderPath);
	virtual std::string getName() const = 0;
	virtual void init(std::vector<float> const& gazVertices,
	                  std::vector<float> const& starsVertices,
	                  std::vector<float> const& darkMatterVertices)
	    = 0;
	virtual void init(std::string const& gazPath,
	                  std::string const& starsPath      = "",
	                  std::string const& darkMatterPath = "")
	    = 0;
	virtual void render(Camera const& camera, QMatrix4x4 const& model) = 0;
	void resetAlpha();
	void setAlpha(float alpha);
	float getAlpha() const { return alpha; };
	virtual ~Method();

	GLHandler::ShaderProgram shaderProgram;
	bool showdm;

  protected:
	float alpha;
};

#endif // DEFINE_H
