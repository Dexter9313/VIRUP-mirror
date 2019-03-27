#include "methods/BaseLineMethodTex.hpp"

BaseLineMethodTex::BaseLineMethodTex()
    : BaseLineMethod("default", "texture")
    , tex()
{
	pointSize = 4;
	// load texture
	tex = GLHandler::newTexture("data/virup/images/particle.png");
}

void BaseLineMethodTex::render(Camera const& camera, double scale,
                               std::array<double, 3> const& translation)
{
	GLHandler::useTextures({tex});
	BaseLineMethod::render(camera, scale, translation);
}

BaseLineMethodTex::~BaseLineMethodTex()
{
	GLHandler::deleteTexture(tex);
}
