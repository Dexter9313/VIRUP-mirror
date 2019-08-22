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
	/**
	 * @brief The current time of the simulation.
	 *
	 * @accessors getSimulationTime(), setSimulationTime()
	 */
	Q_PROPERTY(
	    QDateTime simulationTime READ getSimulationTime WRITE setSimulationTime)
	/**
	 * @brief The current time coefficient of the simulation.
	 *
	 * @accessors getTimeCoeff(), setTimeCoeff()
	 */
	Q_PROPERTY(float timeCoeff READ getTimeCoeff WRITE setTimeCoeff)

	/**
	 * @brief The current global scale of the visualization.
	 * Ratio 1 real meter / 1 visualized meter. For example, if scale == 1/1000,
	 * each real meter you see represents one kilometer. There should be no
	 * point to set scale > 1 for astrophysics.
	 *
	 * @accessors getScale(), setScale()
	 */
	Q_PROPERTY(double scale READ getScale WRITE setScale)
	/**
	 * @brief The current camera position in the visualization, relative to the
	 * cosmological space, in kpc.
	 *
	 * @accessors getCosmoPosition(), setCosmoPosition()
	 */
	Q_PROPERTY(
	    Vector3 cosmoPosition READ getCosmoPosition WRITE setCosmoPosition)
	/**
	 * @brief Wether a planetary system is loaded or not.
	 *
	 * @accessors isPlanetarySystemLoaded()
	 */
	Q_PROPERTY(bool planetarySystemLoaded READ isPlanetarySystemLoaded)
	/**
	 * @brief Name of the currently (pre-)loaded planetary system.
	 *
	 * If set, the corresponding planetary system will be loaded.
	 *
	 * The special name "Solar System" will load whichever system that is set as
	 * the "Solar System" in the launcher, whichever its actual name is. If the
	 * name isn't "Solar System", then the system must reside in a directory
	 * named after it, within the exoplanetary systems directory.
	 */
	Q_PROPERTY(QString planetarySystemName MEMBER planetarySystemName)
	/**
	 * @brief Name of the planetary system camera target.
	 *
	 * @accessors getPlanetTarget(), setPlanetTarget()
	 */
	Q_PROPERTY(QString planetTarget READ getPlanetTarget WRITE setPlanetTarget)
	/**
	 * @brief The current camera position in the visualization, relative to the
	 * planetTarget, in meters. Undefined if !planetarySystemLoaded.
	 *
	 * @accessors getPlanetPosition(), setPlanetPosition()
	 */
	Q_PROPERTY(
	    Vector3 planetPosition READ getPlanetPosition WRITE setPlanetPosition)
	/**
	 * @brief Wether the bounding cube is enabled or not.
	 *
	 * @accessors cubeEnabled(), setCubeEnabled()
	 */
	Q_PROPERTY(bool cubeEnabled READ cubeEnabled WRITE setCubeEnabled)
	/**
	 * @brief Bounding cube color.
	 *
	 * @accessors getCubeColor(), setCubeColor()
	 */
	Q_PROPERTY(QColor cubeColor READ getCubeColor WRITE setCubeColor)
	/**
	 * @brief Camera's pitch in radians.
	 *
	 * @accessors getCamPitch(), setCamPitch()
	 */
	Q_PROPERTY(float camPitch READ getCamPitch WRITE setCamPitch)
	/**
	 * @brief Camera's yaw in radians.
	 *
	 * @accessors getCamYaw(), setCamYaw()
	 */
	Q_PROPERTY(float camYaw READ getCamYaw WRITE setCamYaw)

  public:
	MainWin();

	// TIME

	/**
	 * @getter{simulationTime}
	 */
	QDateTime getSimulationTime() const;
	/**
	 * @setter{simulationTime, simulationTime}
	 */
	void setSimulationTime(QDateTime const& simulationTime);
	/**
	 * @getter{timeCoeff}
	 */
	float getTimeCoeff() const { return clock.getTimeCoeff(); };
	/**
	 * @setter{timeCoeff, timeCoeff}
	 */
	void setTimeCoeff(float timeCoeff) { clock.setTimeCoeff(timeCoeff); };

	// SPACE

	/**
	 * @getter{scale}
	 */
	double getScale() const;
	/**
	 * @setter{scale, scale}
	 */
	void setScale(double scale);
	/**
	 * @getter{cosmoPosition}
	 */
	Vector3 getCosmoPosition() const;
	/**
	 * @setter{cosmoPosition, cosmoPosition}
	 */
	void setCosmoPosition(Vector3 cosmoPosition);
	/**
	 * @getter{planetarySystemLoaded}
	 */
	bool isPlanetarySystemLoaded() const
	{
		return OctreeLOD::renderPlanetarySystem;
	};
	/**
	 * @getter{planetTarget}
	 */
	QString getPlanetTarget() const;
	/**
	 * @setter{planetTarget, planetTarget}
	 */
	void setPlanetTarget(QString const& name);
	/**
	 * @getter{planetPosition}
	 */
	Vector3 getPlanetPosition() const;
	/**
	 * @setter{planetPosition, planetPosition}
	 */
	void setPlanetPosition(Vector3 planetPosition);

	// CUBE

	/**
	 * @getter{cubeEnabled}
	 */
	bool cubeEnabled() const { return showCube; };
	/**
	 * @setter{cubeEnabled}
	 */
	void setCubeEnabled(bool enabled) { showCube = enabled; };
	/**
	 * @getter{cubeColor}
	 */
	QColor getCubeColor() const;
	/**
	 * @setter{cubeColor}
	 */
	void setCubeColor(QColor const& color);

	// CAMERA ORIENTATION

	/**
	 * @getter{camPitch}
	 */
	float getCamPitch() const { return getCamera<Camera>("cosmo").pitch; }
	/**
	 * @setter{camPitch}
	 */
	void setCamPitch(float pitch);
	/**
	 * @getter{camYaw}
	 */
	float getCamYaw() const { return getCamera<Camera>("cosmo").yaw; }
	/**
	 * @setter{camYaw}
	 */
	void setCamYaw(float yaw);

	~MainWin();

	QString planetarySystemName = "";

  public slots:
	/**
	 * @brief Toggles the @e cubeEnabled property.
	 */
	void toggleCube() { setCubeEnabled(!cubeEnabled()); };

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
	void loadSolarSystem();
	void loadNewSystem();
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
	// 1 m = 3.24078e-20 kpc
	const double mtokpc = 3.24078e-20;

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

	bool forceUpdateFromCosmo = true;

	// in kpc
	Vector3 milkyWayDataPos    = Vector3(0.0, 0.0, 0.0);
	Vector3 solarSystemDataPos = Vector3(8.29995608, 0.0, -0.027);
	Vector3 m31DataPos = Vector3(382.92994334, -617.94616647, 288.2071201);
	Billboard* milkyWayLabel    = nullptr;
	Billboard* solarSystemLabel = nullptr;
	Billboard* m31Label         = nullptr;
};

#endif // MAINWIN_H
