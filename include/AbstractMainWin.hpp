#ifndef ABSTRACTMAINWIN_H
#define ABSTRACTMAINWIN_H

#include <QDebug>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QOpenGLWindow>
#include <QProcess>
#include <vector>

#include "BasicCamera.hpp"
#include "DebugCamera.hpp"
#include "GLHandler.hpp"
#include "PythonQtHandler.hpp"
#include "vr/VRHandler.hpp"

/**
 * @brief Main window to be displayed.
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
 * The following Python object are also declared by the engine :
 * * <code>%VRHandler</code> : Is equivalent to accessing @ref vrHandler. See
 * @ref VRHandler description for a Python API specification. This object is
 * only available (not None) if and only if VR is enabled.
 * * <code>camera</code> : Is equivalent to calling @ref getCamera(). See @ref
 * BasicCamera description for a Python API specification.
 * * <code>dbgcamera</code> : Is equivalent to calling @ref getDebugCamera().
 * See @ref DebugCamera description for a Python API specification.
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
class AbstractMainWin : public QOpenGLWindow
{
	Q_OBJECT
	/**
	 * @brief Wether the window is displayed in full screen or not.
	 *
	 * @accessors isFullscreen(), setFullscreen()
	 */
	Q_PROPERTY(bool fullscreen READ isFullscreen WRITE setFullscreen)
	/**
	 * @brief Wether the engine uses High Dynamic Range rendering or not.
	 *
	 * See : <a
	 * href="https://en.wikipedia.org/wiki/High-dynamic-range_rendering">High-dynamic-range
	 * rendering</a>
	 * @accessors getHDR(), setHDR()
	 */
	Q_PROPERTY(bool hdr READ getHDR WRITE setHDR)
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
	struct RenderPath
	{
		explicit RenderPath(BasicCamera* camera)
		    : camera(camera){};
		RenderPath(GLbitfield clearMask, BasicCamera* camera)
		    : clearMask(clearMask)
		    , camera(camera){};

		GLbitfield clearMask = 0x0; // don't clear anything by default
		BasicCamera* camera;
	};

	/**
	 * @brief Constructs an @ref AbstractMainWin.
	 *
	 * Sets the QSurfaceFormat of the window, initializes the @ref
	 * PythonQtHandler then adds some engine related objects to the Python API.
	 */
	AbstractMainWin();
	/**
	 * @getter{fullscreen}
	 */
	bool isFullscreen() const;
	/**
	 * @setter{fullscreen, fullscreen}
	 */
	void setFullscreen(bool fullscreen);
	/**
	 * @getter{hdr}
	 */
	bool getHDR() const { return hdr; };
	/**
	 * @setter{hdr, hdr}
	 *
	 * This will reload the post-processing render targets.
	 */
	void setHDR(bool hdr);
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
	/**
	 * @toggle{fullscreen}
	 */
	void toggleFullscreen();
	/**
	 * @toggle{hdr}
	 */
	void toggleHDR();
	/**
	 * @toggle{vr}
	 */
	void toggleVR();
	// /!\ ownership of path.camera
	void appendSceneRenderPath(QString const& id, RenderPath path);
	// /!\ ownership of path.camera
	void insertSceneRenderPath(QString const& id, RenderPath path,
	                           unsigned int pos);
	void removeSceneRenderPath(QString const& id);
	/**
	 * @brief Appends a post-processing shader to the post-processing pipeline.
	 *
	 * The engine will take care of the vertex shader for you as long as you
	 * follow the post-processing instructions given in the class description.
	 *
	 * @param id Identifier to refer to the fragment shader later.
	 * @param fragment Path to the fragment shader to use. See README for
	 * informations about data paths.
	 */
	void appendPostProcessingShader(QString const& id, QString const& fragment);
	/**
	 * @brief Inserts a post-processing shader into the post-processing
	 * pipeline.
	 *
	 * The engine will take care of the vertex shader for you as long as you
	 * follow the post-processing instructions given in the class description.
	 *
	 * @param id Identifier to refer to the shader later.
	 * @param fragment Path to the fragment shader to use. See README for
	 * informations about data paths.
	 * @param pos Position at which to insert the shader.
	 */
	void insertPostProcessingShader(QString const& id, QString const& fragment,
	                                unsigned int pos);
	/**
	 * @brief Removes a post-processing shader from the post-processing
	 * pipeline.
	 *
	 * @param id Identifier given when appending or inserting the shader.
	 */
	void removePostProcessingShader(QString const& id);

  protected:
	/**
	 * @brief Captures a Qt event.
	 *
	 * See <a
	 * href="https://doc.qt.io/qt-5/qobject.html#event">QObject::event</a>. Make
	 * sure you call @ref AbstractMainWin#event if you override it.
	 */
	virtual bool event(QEvent* e) override;
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

	void renderVRControls() const;
	/**
	 * @brief Returns a constant reference to the @ref BasicCamera used for
	 * rendering during specific scene rendering path.
	 *
	 * Can be of polymorphic type if you have your own cameras registered.
	 */
	BasicCamera const& getCamera(QString const& pathId) const;
	/**
	 * @brief Returns a constant reference to the @ref BasicCamera used for
	 * rendering during specific scene rendering path.
	 *
	 * Can be of polymorphic type if you have your own cameras registered.
	 */
	BasicCamera& getCamera(QString const& pathId);
	/**
	 * @brief Returns a reference to the @ref DebugCamera of the engine.
	 */
	DebugCamera& getDebugCamera() { return *dbgCamera; };

	/**
	 * @brief Gets called before applying a specific post-processing shader.
	 *
	 * Override this method if you want to pass some uniforms (or do anything)
	 * to a post-processing shader you added or inserted in the post-processing
	 * pipeline before it is actually used.
	 *
	 * @param id Identifier of the shader that is going to be used for post
	 * processing.
	 * @param shader The actual shader program.
	 */
	virtual void
	    applyPostProcShaderParams(QString const& id,
	                              GLHandler::ShaderProgram shader) const;
	/**
	 * @brief Reloads the post-processing targets (2D and VR).
	 *
	 * Mainly used after toggling HDR rendering. The new render targets will use
	 * @ref GLHandler#defaultRenderTargetFormat. Will also call @ref
	 * VRHandler#reloadPostProcessingTargets if necessary.
	 */
	void reloadPostProcessingTargets();
	/**
	 * @brief The engine's only @ref VRHandler.
	 */
	VRHandler vrHandler;
	/**
	 * @brief Last frame time to render in seconds.
	 *
	 * This member is read-only.
	 */
	float const& frameTiming = frameTiming_;

	/**
	 * @brief Ordered list of render passes to apply as scene rendering.
	 *
	 * This member is read-only.
	 *
	 * For each QPair p :
	 *
	 * * p.first is the path identifier set when adding or inserting it in the
	 * pipeline using the corresponding methods.
	 * * p.second is the render path itself.
	 */
	QList<QPair<QString, RenderPath>> const& sceneRenderPipeline
	    = sceneRenderPipeline_;
	/**
	 * @brief Ordered list of post-processing shaders to apply at the end of
	 * scene rendering.
	 *
	 * This member is read-only.
	 *
	 * For each QPair p :
	 *
	 * * p.first is the shader identifier set when adding or inserting it in the
	 * pipeline using the corresponding methods.
	 * * p.second is the shader itself.
	 */
	QList<QPair<QString, GLHandler::ShaderProgram>> const&
	    postProcessingPipeline
	    = postProcessingPipeline_;
	/**
	 * @brief Gamma value to use for gamma correction
	 */
	float gamma = 2.2f;

	bool renderControllersBeforeScene  = true;
	QString pathIdRenderingControllers = "default";

  private:
	void initializeGL() override;
	void vrRenderSinglePath(RenderPath& renderPath, QString const& pathId,
	                        bool debug, bool debugInHeadset);
	void vrRender(Side side, bool debug, bool debugInHeadset);
	void paintGL() override;
	void resizeGL(int w, int h) override;

	float frameTiming_ = 0.f;
	QElapsedTimer frameTimer;

	QList<QPair<QString, RenderPath>> sceneRenderPipeline_;
	DebugCamera* dbgCamera = nullptr;

	bool hdr = QSettings().value("window/hdr").toBool();

	QList<QPair<QString, GLHandler::ShaderProgram>> postProcessingPipeline_;
	std::array<GLHandler::RenderTarget, 2> postProcessingTargets = {{{}, {}}};
};

#endif // ABSTRACTMAINWIN_H
