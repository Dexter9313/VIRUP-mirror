#ifndef MAINWIN_H
#define MAINWIN_H

#include <QCoreApplication>
#include <QMessageBox>

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
	virtual void keyReleaseEvent(QKeyEvent* e) override;
	// virtual void mousePressEvent(QMouseEvent* e) override;
	// virtual void mouseReleaseEvent(QMouseEvent* e) override;
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
	void rescaleCube(double newScale, std::array<double, 3> const& scaleCenter
	                                  = {{0.0, 0.0, 0.0}});
	QVector3D dataToWorldPosition(QVector3D const& data) const;
	QVector3D worldToDataPosition(QVector3D const& world) const;
	void printPositionInDataSpace(Side controller = Side::NONE) const;
	static std::vector<float> generateVertices(unsigned int number,
	                                           unsigned int seed);
	static GLHandler::Mesh createCube(GLHandler::ShaderProgram const& shader);
	static void deleteCube(GLHandler::Mesh mesh,
	                       GLHandler::ShaderProgram shader);

	bool loaded   = false;
	BBox dataBBox = {};

	GLHandler::Mesh cube                = {};
	GLHandler::ShaderProgram cubeShader = {};
	Method* method                      = nullptr;
	bool showCube = QSettings().value("misc/showcube").toBool();

	double cubeScale                      = 1.f;
	std::array<double, 3> cubeTranslation = {{0.f, 0.f, 0.f}};

	QPoint lastCursorPos;
	bool trackballEnabled = false;

	// scaling/translation controls variables
	bool leftGripPressed  = false;
	bool rightGripPressed = false;
	float initControllersDistance;
	std::array<double, 3> scaleCenter;
	std::array<double, 3> initControllerPosInCube;
	double initScale;

	// keyboard controls variables
	QVector3D cubePositiveVelocity;
	QVector3D cubeNegativeVelocity;
};

#endif // MAINWIN_H
