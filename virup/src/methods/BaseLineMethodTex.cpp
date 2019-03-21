#include "methods/BaseLineMethodTex.hpp"

BaseLineMethodTex::BaseLineMethodTex()
    : BaseLineMethod("default", "texture")
    , tex()
{
	pointSize = 4;
	// load texture
	tex = GLHandler::newTexture("data/virup/images/particle.png");
}

void BaseLineMethodTex::render(Camera const& camera, QMatrix4x4 const& model)
{
	GLHandler::useTextures({tex});
	BaseLineMethod::render(camera, model);
}

BaseLineMethodTex::~BaseLineMethodTex()
{
	GLHandler::deleteTexture(tex);
}
