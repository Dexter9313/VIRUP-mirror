#include "methods/TreeMethodTex.hpp"

TreeMethodTex::TreeMethodTex()
    : TreeMethodLOD("default", "texture")
{
	// load texture
	tex = GLHandler::newTexture("data/images/particle.png");

	GLHandler::setPointSize(8);

	// init chrono
	//gettimeofday(&t0, NULL);
}

void TreeMethodTex::render(Camera const& camera)
{
	GLHandler::useTextures({tex});
	TreeMethodLOD::render(camera);
}

TreeMethodTex::~TreeMethodTex()
{
	GLHandler::deleteTexture(tex);
}
