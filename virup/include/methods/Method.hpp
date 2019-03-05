#ifndef METHOD_H
#define METHOD_H

#include <string>
#include <vector>

#include "Camera.hpp"
#include "GLHandler.hpp"
#include "utils.hpp"

class Method : public QObject
{
	Q_OBJECT
  public:
	Method(std::string const& shadersCommonName);
	Method(std::string const& vertexShaderPath,
	       std::string const& fragmentShaderPath);
	virtual void render(Camera const& camera, QMatrix4x4 const& model) = 0;
	virtual ~Method();

	GLHandler::ShaderProgram shaderProgram;

  public slots:
	virtual std::string getName() const = 0;
	virtual void init(std::vector<float> const& gazVertices,
	                  std::vector<float> const& starsVertices,
	                  std::vector<float> const& darkMatterVertices)
	    = 0;
	void init(std::string const& gazPath);
	void init(std::string const& gazPath, std::string const& starsPath);
	virtual void init(std::string const& gazPath, std::string const& starsPath,
	                  std::string const& darkMatterPath)
	    = 0;
	void resetAlpha();
	void setAlpha(float alpha);
	float getAlpha() const { return alpha; };
	void setDarkMatterEnabled(bool enabled) { showdm = enabled; };
	bool isDarkMatterEnabled() const { return showdm; };
	void toggleDarkMatter() { showdm = !showdm; };
	virtual void cleanUp(){};

  protected:
	float alpha = 0.011;
	bool showdm = false;
};

#endif // DEFINE_H
