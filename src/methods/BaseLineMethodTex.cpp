#include "methods/BaseLineMethodTex.hpp"

BaseLineMethodTex::BaseLineMethodTex()
    : BaseLineMethod("default", "texture")
{
	pointSize = 4;
	// load texture
	tex = GLHandler::newTexture("data/images/particle.png");
}

void BaseLineMethodTex::render(Camera const& camera)
{
	GLHandler::useTextures({tex});
	BaseLineMethod::render(camera);
}

BaseLineMethodTex::~BaseLineMethodTex()
{
	GLHandler::deleteTexture(tex);
}
