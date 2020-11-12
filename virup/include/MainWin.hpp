#ifndef MAINWIN_H
#define MAINWIN_H

#include <QCoreApplication>
#include <QDirIterator>
#include <QJsonDocument>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "AbstractMainWin.hpp"
#include "Text3D.hpp"

#include "CSVObjects.hpp"
#include "CosmologicalSimulation.hpp"
#include "Grid.hpp"
#include "MovementControls.hpp"

#include "graphics/OrbitalSystemCamera.hpp"
#include "graphics/renderers/OrbitalSystemRenderer.hpp"
#include "physics/OrbitalSystem.hpp"
#include "physics/SimulationTime.hpp"

#include "LibPlanet.hpp"

/** @ingroup pycall
 *
 * Callable in Python as the "VIRUP" object.
 */
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
	 * @brief The current luminosity of the cosmological data.
	 *
	 * @accessors getCosmoLum(), setCosmoLum()
	 */
	Q_PROPERTY(float cosmolum READ getCosmoLum WRITE setCosmoLum)
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
	 * @brief Wether the orbits is enabled or not.
	 *
	 * @accessors orbitsEnabled(), setOrbitsEnabled()
	 */
	Q_PROPERTY(bool orbitsEnabled READ orbitsEnabled WRITE setOrbitsEnabled)
	/**
	 * @brief Wether the labels is enabled or not.
	 *
	 * @accessors labelsEnabled(), setLabelsEnabled()
	 */
	Q_PROPERTY(bool labelsEnabled READ labelsEnabled WRITE setLabelsEnabled)
	/**
	 * @brief Wether the darkmatter is enabled or not.
	 *
	 * @accessors darkmatterEnabled(), setDarkmatterEnabled()
	 */
	Q_PROPERTY(bool darkmatterEnabled READ darkmatterEnabled WRITE
	               setDarkmatterEnabled)
	/**
	 * @brief Wether the grid is enabled or not.
	 *
	 * @accessors gridEnabled(), setGridEnabled()
	 */
	Q_PROPERTY(bool gridEnabled READ gridEnabled WRITE setGridEnabled)
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
	Q_PROPERTY(bool isServer READ isServer)

	bool isServer() const { return networkManager->isServer(); };

  public:
	class State : public AbstractState
	{
	  public:
		State()                   = default;
		State(State const& other) = default;
		State(State&& other)      = default;
		virtual void readFromDataStream(QDataStream& stream) override
		{
			toneMappingState.readFromDataStream(stream);
			cosmoCamState.readFromDataStream(stream);
			planetCamState.readFromDataStream(stream);
			double dut;
			stream >> dut;
			ut = dut;
			stream >> renderLabels;
			stream >> renderOrbits;
			stream >> planetarySystemName;
			stream >> cosmoLum;
			stream >> compass;
			stream >> compassForceTickRes;
			stream >> compassProtractor;
			stream >> servHFOV;
			stream >> servRTWidth;
		};
		virtual void writeInDataStream(QDataStream& stream) override
		{
			toneMappingState.writeInDataStream(stream);
			cosmoCamState.writeInDataStream(stream);
			planetCamState.writeInDataStream(stream);
			double dut(ut);
			stream << dut;
			stream << renderLabels;
			stream << renderOrbits;
			stream << planetarySystemName;
			stream << cosmoLum;
			stream << compass;
			stream << compassForceTickRes;
			stream << compassProtractor;
			stream << servHFOV;
			stream << servRTWidth;
		};

		ToneMappingModel::State toneMappingState;
		Camera::State cosmoCamState;
		OrbitalSystemCamera::State planetCamState;
		UniversalTime ut;
		float renderLabels;
		float renderOrbits;
		QString planetarySystemName;
		float cosmoLum;
		bool compass               = false;
		double compassForceTickRes = 0.0;
		bool compassProtractor     = false;
		float servHFOV             = 70.f;
		unsigned int servRTWidth   = 1920;
	};

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
	/**
	 * @getter{cosmoLum}
	 */
	float getCosmoLum() const;
	/**
	 * @setter{cosmoLum, cosmoLum}
	 */
	void setCosmoLum(float cosmoLum);

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
	 * @getter{orbitsEnabled}
	 */
	bool orbitsEnabled() const { return CelestialBodyRenderer::renderOrbits; };
	/**
	 * @setter{orbitsEnabled, orbitsEnabled}
	 */
	void setOrbitsEnabled(bool enabled)
	{
		CelestialBodyRenderer::renderOrbits = enabled;
	};
	/**
	 * @getter{labelsEnabled}
	 */
	bool labelsEnabled() const { return CelestialBodyRenderer::renderLabels; };
	/**
	 * @setter{labelsEnabled, labelsEnabled}
	 */
	void setLabelsEnabled(bool enabled)
	{
		CelestialBodyRenderer::renderLabels = enabled;
	};
	/**
	 * @getter{darkmatterEnabled}
	 */
	bool darkmatterEnabled() const
	{
		return cosmologicalSim->trees.isDarkMatterEnabled();
	};
	/**
	 * @setter{darkmatterEnabled, darkmatterEnabled}
	 */
	void setDarkmatterEnabled(bool enabled)
	{
		cosmologicalSim->trees.setDarkMatterEnabled(enabled);
	};
	/**
	 * @getter{gridEnabled}
	 */
	bool gridEnabled() const { return showGrid; };
	/**
	 * @setter{gridEnabled, gridEnabled}
	 */
	void setGridEnabled(bool enabled) { showGrid = enabled; };

	// CAMERA ORIENTATION

	/**
	 * @getter{camPitch}
	 */
	float getCamPitch() const
	{
		return renderer.getCamera<Camera>("cosmo").pitch;
	}
	/**
	 * @setter{camPitch, camPitch}
	 */
	void setCamPitch(float pitch);
	/**
	 * @getter{camYaw}
	 */
	float getCamYaw() const { return renderer.getCamera<Camera>("cosmo").yaw; }
	/**
	 * @setter{camYaw, camYaw}
	 */
	void setCamYaw(float yaw);

	~MainWin();

	QString planetarySystemName = "";

  public slots:
	/**
	 * @brief Toggles the @e gridEnabled property.
	 */
	void toggleGrid() { setGridEnabled(!gridEnabled()); };
	/**
	 * @brief Returns closest common ancestor between two planetary bodies.
	 */
	QString
	    getClosestCommonAncestorName(QString const& celestialBodyName0,
	                                 QString const& celestialBodyName1) const;
	/**
	 * @brief Returns celestial body position relative to another at a given
	 * date/time.
	 */
	Vector3 getCelestialBodyPosition(QString const& bodyName,
	                                 QString const& referenceBodyName,
	                                 QDateTime const& dt
	                                 = QDateTime::currentDateTimeUtc()) const;
	/**
	 * @brief Interpolates celestial bodies coordinates relative to their
	 * closest common ancestor.
	 */
	Vector3 interpolateCoordinates(QString const& celestialBodyName0,
	                               QString const& celestialBodyName1,
	                               float t) const;

  protected:
	virtual void actionEvent(BaseInputManager::Action a, bool pressed) override;
	virtual bool event(QEvent* e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;
	virtual void mouseMoveEvent(QMouseEvent* e) override;
	virtual void wheelEvent(QWheelEvent* e) override;
	virtual void vrEvent(VRHandler::Event const& e) override;

	virtual void setupPythonAPI() override;
	virtual void initLibraries() override;

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

	virtual std::vector<GLHandler::Texture> getPostProcessingUniformTextures(
	    QString const& id, GLShaderProgram const& shader,
	    GLHandler::RenderTarget const& currentTarget) const override;

	virtual AbstractState* constructNewState() const override
	{
		return new MainWin::State;
	};
	virtual void readState(AbstractState const& s) override
	{
		auto const& state = dynamic_cast<State const&>(s);
		toneMappingModel->readState(state.toneMappingState);

		auto& cam(renderer.getCamera<Camera&>("cosmo"));
		cam.readState(state.cosmoCamState);
		if(isPlanetarySystemLoaded())
		{
			try
			{
				auto& cam2(renderer.getCamera<OrbitalSystemCamera&>("planet"));
				cam2.readState(state.planetCamState);
			}
			catch(...)
			{
			}
		}
		clock.setCurrentUt(state.ut);
		CelestialBodyRenderer::renderLabels = state.renderLabels;
		CelestialBodyRenderer::renderOrbits = state.renderOrbits;
		planetarySystemName                 = state.planetarySystemName;
		setCosmoLum(state.cosmoLum);
		renderer.setCalibrationCompass(state.compass);
		CalibrationCompass::forcedTickResolution() = state.compassForceTickRes;
		CalibrationCompass::forceProtractorMode()  = state.compassProtractor;
		CalibrationCompass::serverHorizontalFOV()  = state.servHFOV;
		CalibrationCompass::serverRenderTargetWidth() = state.servRTWidth;
	};
	virtual void writeState(AbstractState& s) const override
	{
		auto& state = dynamic_cast<State&>(s);
		toneMappingModel->writeState(state.toneMappingState);

		auto const& cam(renderer.getCamera<Camera const&>("cosmo"));
		cam.writeState(state.cosmoCamState);
		if(isPlanetarySystemLoaded())
		{
			auto const& cam2(
			    renderer.getCamera<OrbitalSystemCamera const&>("planet"));
			cam2.writeState(state.planetCamState);
		}
		state.ut                  = clock.getCurrentUt();
		state.renderLabels        = CelestialBodyRenderer::renderLabels;
		state.renderOrbits        = CelestialBodyRenderer::renderOrbits;
		state.planetarySystemName = planetarySystemName;
		state.cosmoLum            = getCosmoLum();
		state.compass             = renderer.getCalibrationCompass();
		state.compassForceTickRes = CalibrationCompass::forcedTickResolution();
		state.compassProtractor   = CalibrationCompass::forceProtractorMode();
		state.servHFOV            = renderer.getHorizontalFOV();
		state.servRTWidth         = renderer.getSize().width();
	};

  private:
	void loadSolarSystem();
	void loadNewSystem();
	void printPositionInDataSpace(Side controller = Side::NONE) const;
	static std::vector<float> generateVertices(unsigned int number,
	                                           unsigned int seed);

	bool loaded                             = false;
	CosmologicalSimulation* cosmologicalSim = nullptr;
	CSVObjects* hyg                         = nullptr;
	CSVObjects* sdss                        = nullptr;

	Grid* grid    = nullptr;
	bool showGrid = QSettings().value("misc/showgrid").toBool();

	bool moveView = false;
	QPoint cursorPosBackup;
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

	// in kpc
	/*
	Vector3 milkyWayDataPos    = Vector3(0.0, 0.0, 0.0);
	Vector3 m31DataPos = Vector3(382.92994334, -617.94616647, 288.2071201);
	*/
	Vector3 solarSystemDataPos = Vector3();
	std::vector<std::pair<Vector3, LabelRenderer*>> cosmoLabels;

	// TEMP
	const int textWidth  = 225;
	const int textHeight = 145;

	Vector3 lastData   = Vector3(DBL_MAX, DBL_MAX, DBL_MAX);
	Vector3 sysInWorld = Vector3(DBL_MAX, DBL_MAX, DBL_MAX);

	std::string lastTargetName = std::string("");

	bool forceUpdateFromCosmo = true;

	// LENSING
	GLHandler::Texture lenseDistortionMap = {};
	QVector4D lenseScreenCoord;
	float lenseDist = 0.f;

	// SCENES
	QDialog* dialog                   = nullptr;
	std::vector<QPushButton*> buttons = {};
	QPushButton* transitionsButton    = nullptr;
};

#endif // MAINWIN_H
