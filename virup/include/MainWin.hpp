#ifndef MAINWIN_H
#define MAINWIN_H

#include <QCoreApplication>

#include "AbstractMainWin.hpp"

#include "methods/BaseLineMethod.hpp"
#include "methods/BaseLineMethodTex.hpp"
#include "methods/TreeMethodLOD.hpp"
#include "methods/TreeMethodTex.hpp"

class MainWin : public AbstractMainWin
{
	Q_OBJECT
  public:
	MainWin() = default;
	~MainWin();

  public slots:
	bool cubeEnabled() const { return showCube; };
	void setCubeEnabled(bool enabled) { showCube = enabled; };
	void toggleCube() { setCubeEnabled(!cubeEnabled()); };
	QColor getCubeColor() const;
	void setCubeColor(QColor const& color);

  protected:
	virtual void keyPressEvent(QKeyEvent* e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;
	virtual void mouseMoveEvent(QMouseEvent* e) override;
	virtual void wheelEvent(QWheelEvent* e) override;
	virtual void vrEvent(VRHandler::Event const& e) override;

	virtual void setupPythonAPI() override;

	// declare drawn resources
	virtual void initScene() override;

	// update physics/controls/meshes, etc...
	// prepare for rendering
	virtual void updateScene(BasicCamera& camera) override;

	// render user scene on camera
	// (no controllers or hands)
	virtual void renderScene(BasicCamera const& camera) override;

  private:
	QMatrix4x4 computeCubeModel() const;
	void rescaleCube(float newScale, QVector3D const& scaleCenter);
	static std::vector<float> generateVertices(unsigned int number,
	                                           unsigned int seed);
	static GLHandler::Mesh createCube(GLHandler::ShaderProgram const& shader);
	static void deleteCube(GLHandler::Mesh mesh,
	                       GLHandler::ShaderProgram shader);

	bool loaded = false;

	GLHandler::Mesh cube                = {};
	GLHandler::ShaderProgram cubeShader = {};
	Method* method                      = nullptr;
	bool showCube = QSettings().value("misc/showcube").toBool();

	float cubeScale           = 1.f;
	QVector3D cubeTranslation = QVector3D(0.f, 0.f, 0.f);

	QPoint lastCursorPos;
	bool trackballEnabled = false;

	// scaling/translation controls variables
	bool leftGripPressed  = false;
	bool rightGripPressed = false;
	float initControllersDistance;
	QVector3D scaleCenter;
	QVector3D initControllerPosInCube;
	float initScale;
};

#endif // MAINWIN_H
