#include "methods/BaseLineMethodTex.hpp"

BaseLineMethodTex::BaseLineMethodTex()
    : BaseLineMethod("gaz")
    , tex()
{
	GLHandler::glf().glEnable(GL_POINT_SPRITE);
	GLHandler::glf().glEnable(GL_PROGRAM_POINT_SIZE);
	// load texture
	tex = GLHandler::newTexture("data/virup/images/particle.png");
}

void BaseLineMethodTex::render(Camera const& camera, double scale,
                               std::array<double, 3> const& translation)
{
	GLHandler::setShaderParam(shaderProgram, "scale", scale);
	GLHandler::useTextures({tex});
	BaseLineMethod::render(camera, scale, translation);
}

BaseLineMethodTex::~BaseLineMethodTex()
{
	GLHandler::deleteTexture(tex);
}
