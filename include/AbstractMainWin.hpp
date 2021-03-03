#ifndef ABSTRACTMAINWIN_H
#define ABSTRACTMAINWIN_H

#include <QDebug>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QKeyEvent>
#include <QProcess>
#include <QRunnable>
#include <QScreen>
#include <QStandardPaths>
#include <QThreadPool>
#include <QWindow>
#include <vector>

#include "AbstractLibrary.hpp"
#include "AsyncMesh.hpp"
#include "AsyncTexture.hpp"
#include "BasicCamera.hpp"
#include "DebugCamera.hpp"
#include "InputManager.hpp"
#include "NetworkManager.hpp"
#include "PythonQtHandler.hpp"
#include "Renderer.hpp"
#include "ShaderProgram.hpp"
#include "ToneMappingModel.hpp"
#include "gl/GLHandler.hpp"
#include "gl/GLShaderProgram.hpp"
#include "vr/OpenVRHandler.hpp"
#include "vr/StereoBeamerHandler.hpp"

/** @ingroup pycall
 *
 * @brief Main window to be displayed.
 *
 * Callable in Python as the "HydrogenVR" object unless replaced by
 * application's MainWin object.
 *
 * Inherit from it to implement your application, calling your class "MainWin".
 *
 * Most of the program resources will be handled by this class. Especially,
 * all Handlers will be initialized and cleaned up by this class.
 *
 * @par Python API
 *
 * By default, a Python object called "HydrogenVR" will be created with
 * properties and public slots of this class as its members. If you inherit from
 * this class with a custom class, the properties and slots of the custom class
 * won't be taken into account. You will have to set your own Python object by
 * overriding the @ref setupPythonAPI method.
 *
 * Some engine class are also instanciated in Python. For the complete list, see
 * : @ref pycall .
 *
 * The following methods have a Python equivalent function and both will get
 * called by the engine. Implementation of any Python functions is optional.
 * * @ref keyPressEvent : <code>keyPressEvent(QKeyEvent e)</code>
 * * @ref keyReleaseEvent : <code>keyReleaseEvent(QKeyEvent e)</code>
 * * @ref vrEvent : <code>vrEvent(EventType type, Side side, Button
 * button)</code>. See @ref VRHandler::Event for details about each parameter.
 * * @ref applyPostProcShaderParams : <code>applyPostProcShaderParams(str id,
 * ShaderProgram shader)</code>
 * * @ref initScene : <code>%initScene()</code>
 * * @ref updateScene : <code>%updateScene()</code>. If you need to access the
 * current rendering camera, the Python <code>camera</code> object can be
 * accessed.
 * * @ref renderScene : <code>%renderScene()</code>. If you need to access the
 * current rendering camera, the Python <code>camera</code> object can be
 * accessed.
 * * #~AbstractMainWin : <code>cleanUpScene()</code>
 *
 * TODO (florian) rewrite
 * @par Post-processing
 *
 * A post processing fragment shader must have :
 * * a vec2 input named "texCoord" which are the texture coordinates to sample
 * the render to texture last result.
 * * a vec4 output named "outColor" which holds the result color of the current
 * fragment.
 * * a sampler2D uniform to sample the render to texture last result. The first
 * declared sampler2D will be used.
 *
 * @par Events
 *
 * Currently, the engine uses these keyboard keys :
 * * F1 : Toggles debug camera
 * * F8 : Toggles Python interpreter console
 * * F11 : Toggles VR mode
 * * Alt+Return : Toggles fullscreen
 * * Escape : Quit
 */
class AbstractMainWin : public QWindow
{
	Q_OBJECT
	Q_PROPERTY(float horizontalFOV READ getHorizontalFOV()
	               WRITE setHorizontalFOV)
	Q_PROPERTY(float verticalFOV READ getVerticalFOV() WRITE setVerticalFOV)
	/**
	 * @brief Horizontal angle shift in degrees compared to server if current
	 * instance is a network client.
	 */
	Q_PROPERTY(double horizontalAngleShift READ getHorizontalAngleShift WRITE
	               setHorizontalAngleShift)
	/**
	 * @brief Vertical angle shift in degrees compared to server if current
	 * instance is a network client.
	 */
	Q_PROPERTY(double verticalAngleShift READ getVerticalAngleShift WRITE
	               setVerticalAngleShift)
	/**
	 * @brief For Stereo Beamer VR mode. Shifts camera's frustum tip. In case
	 * the user's head is not aligned with the screen's center, this can fix
	 * perspective.
	 */
	Q_PROPERTY(QVector3D virtualCamShift READ getVirtualCamShift WRITE
	               setVirtualCamShift)
	/*
	 * @brief Compass tilt around the 0->180deg axis.
	 */
	Q_PROPERTY(float compassTilt READ getCompassTilt WRITE setCompassTilt)
	/**
	 * @brief Wether the window is displayed in full screen or not.
	 *
	 * @accessors isFullscreen(), setFullscreen()
	 */
	Q_PROPERTY(bool fullscreen READ isFullscreen WRITE setFullscreen)
	/**
	 * @brief Wether the engine renders all meshes as wireframes or not.
	 *
	 * @accessors getWireframe(), setWireframe()
	 */
	Q_PROPERTY(bool wireframe READ getWireframe WRITE setWireframe)
	/**
	 * @brief Wether VR mode is enabled or not.
	 *
	 * @accessors vrIsEnabled(), setVR()
	 */
	Q_PROPERTY(bool vr READ vrIsEnabled WRITE setVR)
	/**
	 * @brief Gamma value to use for gamma correction.
	 */
	Q_PROPERTY(float gamma MEMBER gamma)

  public:
	/**
	 * @brief Constructs an @ref AbstractMainWin.
	 *
	 * Sets the QSurfaceFormat of the window, initializes the @ref
	 * PythonQtHandler then adds some engine related objects to the Python API.
	 */
	AbstractMainWin();

	/**
	 * @getter{horizontalFOV}
	 */
	double getHorizontalFOV() const;
	/**
	 * @getter{verticalFOV}
	 */
	double getVerticalFOV() const;
	/**
	 * @setter{horizontalFOV}
	 */
	void setHorizontalFOV(double fov);
	/**
	 * @setter{verticalFOV}
	 */
	void setVerticalFOV(double fov);
	/**
	 * @getter{horizontalAngleShift}
	 */
	double getHorizontalAngleShift() const;
	/**
	 * @getter{verticalAngleShift}
	 */
	double getVerticalAngleShift() const;
	/**
	 * @setter{horizontalAngleShift}
	 */
	void setHorizontalAngleShift(double angleShift);
	/**
	 * @setter{verticalAngleShift}
	 */
	void setVerticalAngleShift(double angleShift);
	/**
	 * @getter{virtualCamShift}
	 */
	QVector3D getVirtualCamShift() const;
	/**
	 * @setter{virtualcamShift}
	 */
	void setVirtualCamShift(QVector3D const& virtualCamShift);
	/**
	 * @getter{compasstilt}
	 */
	float getCompassTilt() { return CalibrationCompass::tilt(); };
	/**
	 * @setter{compasstilt}
	 */
	void setCompassTilt(float tilt) { CalibrationCompass::tilt() = tilt; };
	/**
	 * @getter{fullscreen}
	 */
	bool isFullscreen() const;
	/**
	 * @setter{fullscreen, fullscreen}
	 */
	void setFullscreen(bool fullscreen);
	/**
	 * @getter{wireframe}
	 */
	bool getWireframe() const { return renderer.wireframe; };
	/**
	 * @setter{wireframe}
	 */
	void setWireframe(bool wireframe) { renderer.wireframe = wireframe; };
	/**
	 * @getter{vr}
	 */
	bool vrIsEnabled() const;
	/**
	 * @setter{vr, vr}
	 */
	void setVR(bool vr);
	/**
	 * @brief Destroys the @ref AbstractMainWin instance, freeing its resources.
	 *
	 * Also calls the Python function @e cleanUpScene before freeing anything.
	 */
	virtual ~AbstractMainWin();

  public slots:
	void reloadPythonEngine();
	/**
	 * @toggle{fullscreen}
	 */
	void toggleFullscreen();
	/**
	 * @toggle{wireframe}
	 */
	void toggleWireframe();
	void toggleCalibrationCompass() { renderer.toggleCalibrationCompass(); };
	void toggleCalibrationCompassMode()
	{
		CalibrationCompass::forceProtractorMode()
		    = !CalibrationCompass::forceProtractorMode();
	};
	double getCalibrationCompassTickResolution()
	{
		return CalibrationCompass::getCurrentTickResolution();
	};
	double getDoubleFarRightPixelSubtendedAngle()
	{
		return renderer.getDoubleFarRightPixelSubtendedAngle();
	};
	void setCalibrationCompassTickResolution(double tickRes)
	{
		CalibrationCompass::forcedTickResolution() = tickRes;
	}
	/**
	 * @brief Reloads all shaders managed by ShadersLoader.
	 */
	void reloadAllShaders() { ShaderProgram::reloadAllShaderPrograms(); };
	/**
	 * @toggle{vr}
	 */
	void toggleVR();
	/**
	 * @brief Saves a screenshot the file specified as @p path.
	 *
	 * If @p path is empty, a valid path will be asked graphically to the user.
	 *
	 */
	void takeScreenshot(QString path = "") const;

  protected:
	/**
	 * @brief Captures a Qt event.
	 *
	 * See <a
	 * href="https://doc.qt.io/qt-5/qobject.html#event">QObject::event</a>. Make
	 * sure you call @ref AbstractMainWin#event if you override it.
	 */
	virtual bool event(QEvent* e) override;
	virtual void resizeEvent(QResizeEvent* ev) override;
	/**
	 * @brief Captures a Qt keyboard press event.
	 *
	 * See <a
	 * href="https://doc.qt.io/qt-5/qwidget.html#keyPressEvent">QWidget::keyPressEvent</a>.
	 * Make sure you call @ref AbstractMainWin#keyPressEvent if you override
	 * it.
	 *
	 * Also calls the Python function @e keyPressEvent.
	 */
	virtual void keyPressEvent(QKeyEvent* e) override;
	/**
	 * @brief Captures a Qt keyboard release event.
	 *
	 * See <a
	 * href="https://doc.qt.io/qt-5/qwidget.html#keyReleaseEvent">QWidget::keyReleaseEvent</a>.
	 * Make sure you call @ref AbstractMainWin#keyReleaseEvent if you override
	 * it.
	 *
	 * Also calls the Python function @e keyReleaseEvent.
	 */
	virtual void keyReleaseEvent(QKeyEvent* e) override;
	/**
	 * @brief Captures a @e BaseInputManager Action triggered by a QKeySequence.
	 *
	 * For a key press, @p pressed is true, for a key release, it is false.
	 */
	virtual void actionEvent(BaseInputManager::Action a, bool pressed);
	/**
	 * @brief Captures an event polled from @ref VRHandler.
	 *
	 * Make sure you call @ref AbstractMainWin#vrEvent if you override
	 * it.
	 *
	 * Also calls the Python function @e vrEvent.
	 */
	virtual void vrEvent(VRHandler::Event const& e);
	/**
	 * @brief Adds an @ref AbstractMainWin object to the Python API.
	 *
	 * Override this to create your own MainWindow python object.
	 * @see PythonQtHandler#addObject.
	 */
	virtual void setupPythonAPI();
	/**
	 * @brief Override this to initialize HydrogenVR libraries you use by
	 * calling @e initLibrary().
	 */
	virtual void initLibraries(){};
	/**
	 * @brief Initialize a library by class. The class must inherit from @e
	 * AbstractLibrary.
	 */
	template <class T>
	void initLibrary();
	/**
	 * @brief Gets called after the OpenGL context is ready and before the main
	 * loop.
	 *
	 * Use it to declare OpenGL resources for example, or change the Camera with
	 * your own.
	 * The Python function @e initScene() will be called alongside it.
	 */
	virtual void initScene() = 0;
	/**
	 * @brief Gets called in the main loop before each rendering.
	 *
	 * Use it to update physics, read controllers position or update your meshes
	 * for example.
	 * The Python function @e updateScene() will be called alongside it.
	 *
	 * @param camera The camera used for rendering.
	 */
	virtual void updateScene(BasicCamera& camera, QString const& pathId) = 0;

	virtual AbstractState* constructNewState() const { return nullptr; };
	virtual void readState(AbstractState const& /*s*/){};
	virtual void writeState(AbstractState& /*s*/) const {};

  public:
	/**
	 * @brief Gets called in the main loop during each rendering.
	 *
	 * Can be called several times per frames, as much as there are geometry
	 * render passes (twice for VR for example). Mostly use it to render your
	 * meshes. The Python function @e renderScene() will be called alongside it.
	 *
	 * @param camera The camera used for rendering.
	 */
	virtual void renderScene(BasicCamera const& camera, QString const& pathId)
	    = 0;

	/**
	 * @brief Gets called before applying a specific post-processing shader.
	 *
	 * Override this method if you want to pass some uniforms (or do anything)
	 * to a post-processing shader you added or inserted in the post-processing
	 * pipeline before it is actually used.
	 *
	 * @warning Don't forget to call this class version of the method for core
	 * post-processing (tone mapping for example) and python post-processing to
	 * work !
	 *
	 * @param id Identifier of the shader that is going to be used for post
	 * processing.
	 * @param shader The actual shader program.
	 */
	virtual void applyPostProcShaderParams(
	    QString const& id, GLShaderProgram const& shader,
	    GLFramebufferObject const& currentTarget) const;
	/**
	 * @brief Override to return textures to use in your post-processing
	 * shaders.
	 *
	 * Don't forget that the first sampler2D will be used by GLHandler to store
	 * the previous rendering result.
	 *
	 * @param id Identifier of the shader that is going to be used for post
	 * processing.
	 * @param shader The actual shader program.
	 */
	virtual std::vector<
	    std::pair<GLTexture const*, GLComputeShader::DataAccessMode>>
	    getPostProcessingUniformTextures(
	        QString const& id, GLShaderProgram const& shader,
	        GLFramebufferObject const& currentTarget) const;

  protected:
	/**
	 * @brief The engine's only @ref BaseInputManager.
	 */
	InputManager inputManager;
	/**
	 * @brief The engine's only @ref VRHandler.
	 */
	VRHandler* vrHandler
	    = QSettings().value("vr/mode").toBool()
	          ? static_cast<VRHandler*>(new OpenVRHandler)
	          : static_cast<VRHandler*>(new StereoBeamerHandler);
	/**
	 * @brief The engine's only @ref Renderer.
	 */
	Renderer renderer;
	NetworkManager* networkManager = nullptr;
	/**
	 * @brief Last frame time to render in seconds.
	 *
	 * This member is read-only.
	 */
	float const& frameTiming = frameTiming_;
	/**
	 * @brief Gamma value to use for gamma correction
	 */
	float gamma = 2.2f;

	// OFFSCREEN RENDERING
	bool videomode                 = false;
	unsigned int currentVideoFrame = 0;

	// Postprocessing
	ToneMappingModel* toneMappingModel = nullptr;

  private:
	void initializeGL();
	void initializePythonQt();
	void reloadPythonQt();
	void setupPythonScripts();
	void paintGL();

	float frameTiming_ = 0.f;
	QElapsedTimer frameTimer;

	QOpenGLContext m_context;
	bool initialized = false;
	bool reloadPy    = false;

	// BLOOM
	bool bloom = QSettings().value("graphics/bloom").toBool();
	std::array<GLFramebufferObject*, 2> bloomTargets = {{nullptr, nullptr}};
	void reloadBloomTargets();
};

template <class T>
void AbstractMainWin::initLibrary()
{
	static_assert(
	    std::is_base_of<AbstractLibrary, T>::value,
	    "Initializing a library that doesn't inherit from AbstractLibrary.");
	T lib;
	lib.setupPythonAPI();
}

class ImageWriter : public QRunnable
{
	QString filename;
	QImage img;

  public:
	ImageWriter(QString filename, QImage img)
	    : filename(filename)
	    , img(img)
	{
	}
	void run() override { img.save(filename); }
};

#endif // ABSTRACTMAINWIN_H
