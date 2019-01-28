#ifndef MAINWIN_H
#define MAINWIN_H

#include "AbstractMainWin.hpp"

class MainWin : public AbstractMainWin
{
	Q_OBJECT
  public:
	MainWin();
	~MainWin();

  protected:
	// declare drawn resources
	virtual void initScene() override;

	// update physics/controls/meshes, etc...
	// prepare for rendering
	virtual void updateScene(BasicCamera& camera) override;

	// render user scene on camera
	// (no controllers or hands)
	virtual void renderScene(BasicCamera const& camera) override;

  private:
	GLHandler::Mesh mesh;
	GLHandler::ShaderProgram shaderProgram;

	GLHandler::Mesh pointsMesh;
	GLHandler::ShaderProgram pointsShader;

	GLHandler::Mesh cube;
	GLHandler::ShaderProgram cubeShader;
	QElapsedTimer cubeTimer;
};

#endif // MAINWIN_H
