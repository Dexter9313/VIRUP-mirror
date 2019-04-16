#ifndef MAINWIN_H
#define MAINWIN_H

#include "AbstractMainWin.hpp"
#include "Billboard.hpp"
#include "Primitives.hpp"
#include "Text3D.hpp"

class MainWin : public AbstractMainWin
{
	Q_OBJECT
  public:
	MainWin() = default;
	~MainWin();

  protected:
	virtual void keyPressEvent(QKeyEvent* e) override;

	// declare drawn resources
	virtual void initScene() override;

	// update physics/controls/meshes, etc...
	// prepare for rendering
	virtual void updateScene(BasicCamera& camera) override;

	// render user scene on camera
	// (no controllers or hands)
	virtual void renderScene(BasicCamera const& camera) override;

	virtual void applyPostProcShaderParams(
	    QString const& id, GLHandler::ShaderProgram shader) const override;

  private:
	GLHandler::ShaderProgram sbShader;
	GLHandler::Mesh skybox;
	GLHandler::Texture sbTexture;

	GLHandler::Mesh mesh;
	GLHandler::ShaderProgram shaderProgram;

	GLHandler::Mesh pointsMesh;
	GLHandler::ShaderProgram pointsShader;

	GLHandler::Mesh cube;
	GLHandler::ShaderProgram cubeShader;
	QElapsedTimer cubeTimer;

	GLHandler::Mesh sphere;
	GLHandler::ShaderProgram sphereShader;

	Billboard* bill;
	Text3D* text;

	float barrelPower = 1.01f;
};

#endif // MAINWIN_H
