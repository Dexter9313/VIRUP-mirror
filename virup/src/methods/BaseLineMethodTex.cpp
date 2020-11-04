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

void BaseLineMethodTex::render(Camera const& camera)
{
	shaderProgram.setUniform("scale", static_cast<float>(camera.scale));
	GLHandler::useTextures({tex});
	BaseLineMethod::render(camera);
}

BaseLineMethodTex::~BaseLineMethodTex()
{
	GLHandler::deleteTexture(tex);
}
