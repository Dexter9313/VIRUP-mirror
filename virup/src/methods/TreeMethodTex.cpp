#include "methods/TreeMethodTex.hpp"

TreeMethodTex::TreeMethodTex()
    : TreeMethodLOD("gaz")
    , tex()
{
	GLHandler::glf().glEnable(GL_POINT_SPRITE);
	GLHandler::glf().glEnable(GL_PROGRAM_POINT_SIZE);
	// load texture
	tex = GLHandler::newTexture("data/virup/images/particle.png");

	setPointSize = false;
}

void TreeMethodTex::render(Camera const& camera)
{
	GLHandler::setShaderParam(shaderProgram, "scale",
	                          static_cast<float>(camera.scale));
	GLHandler::useTextures({tex});
	TreeMethodLOD::render(camera);
}

TreeMethodTex::~TreeMethodTex()
{
	GLHandler::deleteTexture(tex);
}
