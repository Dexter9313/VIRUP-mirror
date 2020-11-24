#ifndef MAINWIN_H
#define MAINWIN_H

#include <QCalendarWidget>

#include "AbstractMainWin.hpp"
#include "Billboard.hpp"
#include "CalibrationCompass.hpp"
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
	class State : public AbstractState
	{
	  public:
		State()                   = default;
		State(State const& other) = default;
		State(State&& other)      = default;
		virtual void readFromDataStream(QDataStream& stream) override
		{
			stream >> exposure;
			stream >> dynamicrange;
			stream >> yaw;
			stream >> pitch;
			stream >> servHFOV;
			stream >> servRTWidth;
		};
		virtual void writeInDataStream(QDataStream& stream) override
		{
			stream << exposure;
			stream << dynamicrange;
			stream << yaw;
			stream << pitch;
			stream << servHFOV;
			stream << servRTWidth;
		};

		float exposure           = 0.f;
		float dynamicrange       = 0.f;
		float yaw                = 0.f;
		float pitch              = 0.f;
		float servHFOV           = 70.f;
		unsigned int servRTWidth = 1920;
	};

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
	    QString const& id, GLShaderProgram const& shader,
	    GLHandler::RenderTarget const& currentTarget) const override;

	virtual AbstractState* constructNewState() const override
	{
		return new MainWin::State;
	};
	virtual void readState(AbstractState const& s) override
	{
		auto const& state              = dynamic_cast<State const&>(s);
		toneMappingModel->exposure     = state.exposure;
		toneMappingModel->dynamicrange = state.dynamicrange;
		yaw                            = state.yaw;
		pitch                          = state.pitch;
		CalibrationCompass::serverHorizontalFOV()     = state.servHFOV;
		CalibrationCompass::serverRenderTargetWidth() = state.servRTWidth;
	};
	virtual void writeState(AbstractState& s) const override
	{
		auto& state        = dynamic_cast<State&>(s);
		state.exposure     = toneMappingModel->exposure;
		state.dynamicrange = toneMappingModel->dynamicrange;
		state.yaw          = yaw;
		state.pitch        = pitch;
		state.servHFOV     = renderer.getHorizontalFOV();
		state.servRTWidth  = renderer.getSize().width();
	};

  private:
	ShaderProgram sbShader;
	GLMesh* skybox;
	GLHandler::Texture sbTexture;

	GLMesh* mesh;
	ShaderProgram shaderProgram;

	GLMesh* pointsMesh;
	ShaderProgram pointsShader;

	MovingCube* movingCube;

	GLMesh* sphere;
	ShaderProgram sphereShader;

	GLMesh* playarea;
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
