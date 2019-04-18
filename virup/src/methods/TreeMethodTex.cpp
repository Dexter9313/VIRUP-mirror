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

void TreeMethodTex::render(Camera const& camera, double scale,
                           std::array<double, 3> const& translation)
{
	GLHandler::setShaderParam(shaderProgram, "scale",
	                          static_cast<float>(scale));
	GLHandler::useTextures({tex});
	TreeMethodLOD::render(camera, scale, translation);
}

TreeMethodTex::~TreeMethodTex()
{
	GLHandler::deleteTexture(tex);
}
