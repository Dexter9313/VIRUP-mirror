#ifndef MAINWIN_H
#define MAINWIN_H

#include <QCalendarWidget>

#include "AbstractMainWin.hpp"
#include "Billboard.hpp"
#include "Model.hpp"
#include "Primitives.hpp"
#include "ShaderProgram.hpp"
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
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;
	virtual void mouseMoveEvent(QMouseEvent* e) override;

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
	    QString const& id, GLHandler::ShaderProgram shader,
	    GLHandler::RenderTarget const& currentTarget) const override;

  private:
	ShaderProgram sbShader;
	GLHandler::Mesh skybox;
	GLHandler::Texture sbTexture;

	GLHandler::Mesh mesh;
	ShaderProgram shaderProgram;

	GLHandler::Mesh pointsMesh;
	ShaderProgram pointsShader;

	MovingCube* movingCube;

	GLHandler::Mesh sphere;
	ShaderProgram sphereShader;

	GLHandler::Mesh playarea;
	ShaderProgram playareaShader;

	Model* model;
	Light* light;
	QMatrix4x4 modelModel;

	Billboard* bill;
	Text3D* text;
	Widget3D* widget3d;

	float barrelPower = 1.01f;

	bool moveView = false;
	QPoint cursorPosBackup;
	float yaw;
	float pitch;

	QElapsedTimer timer;
};

#endif // MAINWIN_H
