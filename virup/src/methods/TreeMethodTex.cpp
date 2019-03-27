#include "methods/TreeMethodTex.hpp"

TreeMethodTex::TreeMethodTex()
    : TreeMethodLOD("default", "texture")
    , tex()
{
	// load texture
	tex = GLHandler::newTexture("data/virup/images/particle.png");

	GLHandler::setPointSize(8);

	// init chrono
	// gettimeofday(&t0, NULL);
}

void TreeMethodTex::render(Camera const& camera, double scale,
                           std::array<double, 3> const& translation)
{
	GLHandler::useTextures({tex});
	TreeMethodLOD::render(camera, scale, translation);
}

TreeMethodTex::~TreeMethodTex()
{
	GLHandler::deleteTexture(tex);
}
