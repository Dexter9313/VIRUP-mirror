#include "methods/Method.hpp"

Method::Method(std::string const& shadersCommonName)
    : Method(shadersCommonName, shadersCommonName)
{
}

Method::Method(std::string const& vertexShaderPath,
               std::string const& fragmentShaderPath)
{
	shaderProgram = GLHandler::newShader(vertexShaderPath.c_str(),
	                                     fragmentShaderPath.c_str());
	resetAlpha();
}

void Method::init(std::string const& gazPath)
{
	init(gazPath, "", "");
}

void Method::init(std::string const& gazPath, std::string const& starsPath)
{
	init(gazPath, starsPath, "");
}

void Method::resetAlpha()
{
	setAlpha(0.011);
}

void Method::setAlpha(float alpha)
{
	this->alpha = alpha;
	GLHandler::setShaderParam(shaderProgram, "alpha", alpha);
}

Method::~Method()
{
	GLHandler::deleteShader(shaderProgram);
}
