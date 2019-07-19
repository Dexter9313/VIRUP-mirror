#ifndef MAINWIN_H
#define MAINWIN_H

#include <QCoreApplication>
#include <QDirIterator>
#include <QJsonDocument>
#include <QMessageBox>

#include "AbstractMainWin.hpp"
#include "Text3D.hpp"

#include "MovementControls.hpp"
#include "methods/BaseLineMethod.hpp"
#include "methods/BaseLineMethodTex.hpp"
#include "methods/TreeMethodLOD.hpp"
#include "methods/TreeMethodTex.hpp"

#include "graphics/OrbitalSystemCamera.hpp"
#include "graphics/renderers/OrbitalSystemRenderer.hpp"
#include "physics/OrbitalSystem.hpp"
#include "physics/SimulationTime.hpp"

class MainWin : public AbstractMainWin
{
	Q_OBJECT
  public:
	MainWin();
	void loadSolarSystem();
	void loadNewSystem();
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
	virtual void updateScene(BasicCamera& camera,
	                         QString const& pathId) override;

	// render user scene on camera
	// (no controllers or hands)
	virtual void renderScene(BasicCamera const& camera,
	                         QString const& pathId) override;

  private:
	Vector3 dataToWorldPosition(Vector3 const& data) const;
	Vector3 worldToDataPosition(Vector3 const& world) const;
	void printPositionInDataSpace(Side controller = Side::NONE) const;
	static std::vector<float> generateVertices(unsigned int number,
	                                           unsigned int seed);
	static GLHandler::Mesh createCube(GLHandler::ShaderProgram const& shader);
	static void deleteCube(GLHandler::Mesh mesh,
	                       GLHandler::ShaderProgram shader);

	bool loaded    = false;
	Method* method = nullptr;

	GLHandler::Mesh cube                = {};
	GLHandler::ShaderProgram cubeShader = {};
	bool showCube = QSettings().value("misc/showcube").toBool();

	MovementControls* movementControls = nullptr;

	/* PLANET SYSTEMS */
	OrbitalSystem* solarSystem                 = nullptr;
	OrbitalSystemRenderer* solarSystemRenderer = nullptr;

	OrbitalSystemCamera* camPlanet        = nullptr;
	OrbitalSystem* orbitalSystem          = nullptr;
	OrbitalSystemRenderer* systemRenderer = nullptr;
	SimulationTime clock                  = SimulationTime(
        QSettings().value("simulation/starttime").value<QDateTime>());

	/* TEXT */
	Text3D* debugText         = nullptr;
	float timeSinceTextUpdate = FLT_MAX;

	// TEMP
	const int textWidth  = 225;
	const int textHeight = 145;

	Vector3 lastData   = Vector3(DBL_MAX, DBL_MAX, DBL_MAX);
	Vector3 sysInWorld = Vector3(DBL_MAX, DBL_MAX, DBL_MAX);

	std::string lastTargetName = std::string("");
};

#endif // MAINWIN_H
