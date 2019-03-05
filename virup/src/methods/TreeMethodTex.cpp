#include "methods/TreeMethodTex.hpp"

TreeMethodTex::TreeMethodTex()
    : TreeMethodLOD("default", "texture")
{
	// load texture
	tex = GLHandler::newTexture("data/virup/images/particle.png");

	GLHandler::setPointSize(8);

	// init chrono
	// gettimeofday(&t0, NULL);
}

void TreeMethodTex::render(Camera const& camera, QMatrix4x4 const& model)
{
	GLHandler::useTextures({tex});
	TreeMethodLOD::render(camera, model);
}

TreeMethodTex::~TreeMethodTex()
{
	cleanUp();
	GLHandler::deleteTexture(tex);
}
