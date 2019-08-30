#ifndef MAINWIN_H
#define MAINWIN_H

#include <QCalendarWidget>

#include "AbstractMainWin.hpp"
#include "Billboard.hpp"
#include "Primitives.hpp"
#include "Text3D.hpp"
#include "Widget3D.hpp"
#include "movingcube/MovingCube.hpp"

class MainWin : public AbstractMainWin
{
	Q_OBJECT
  public:
	MainWin() = default;
	~MainWin();

  protected:
	virtual void actionEvent(BaseInputManager::Action a, bool pressed) override;

	// declare drawn resources
	virtual void initScene() override;

	// update physics/controls/meshes, etc...
	// prepare for rendering
	virtual void updateScene(BasicCamera& camera,
	                         QString const& pathId) override;

	// render user scene on camera
	// (no controllers or hands)
	virtual void renderScene(BasicCamera const& camera,
	                         QString const& pathId) override;

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

	MovingCube* movingCube;

	GLHandler::Mesh sphere;
	GLHandler::ShaderProgram sphereShader;

	GLHandler::Mesh playarea;
	GLHandler::ShaderProgram playareaShader;

	Billboard* bill;
	Text3D* text;
	Widget3D* widget3d;

	float barrelPower = 1.01f;
};

#endif // MAINWIN_H
