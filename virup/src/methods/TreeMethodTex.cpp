#include "methods/TreeMethodTex.hpp"

TreeMethodTex::TreeMethodTex()
    : TreeMethodLOD("gaz")
    , tex("data/virup/images/particle.png")
{
	GLHandler::glf().glEnable(GL_POINT_SPRITE);
	GLHandler::glf().glEnable(GL_PROGRAM_POINT_SIZE);

	setPointSize = false;
}

void TreeMethodTex::render(Camera const& camera)
{
	shaderProgram.setUniform("scale", static_cast<float>(camera.scale));
	GLHandler::useTextures({&tex});
	TreeMethodLOD::render(camera);
}
