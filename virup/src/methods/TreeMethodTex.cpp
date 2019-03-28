#include "methods/TreeMethodTex.hpp"

TreeMethodTex::TreeMethodTex()
    : TreeMethodLOD("default", "texture")
    , tex()
{
	GLHandler::glf().glEnable(GL_POINT_SPRITE);
	// load texture
	tex = GLHandler::newTexture("data/virup/images/particle.png");
}

void TreeMethodTex::render(Camera const& camera, double scale,
                           std::array<double, 3> const& translation)
{
	GLHandler::setPointSize(8);
	GLHandler::useTextures({tex});
	TreeMethodLOD::render(camera, scale, translation);
	GLHandler::setPointSize(1);
}

TreeMethodTex::~TreeMethodTex()
{
	GLHandler::deleteTexture(tex);
}
