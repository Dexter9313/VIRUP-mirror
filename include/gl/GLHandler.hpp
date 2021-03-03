#ifndef GLHANDLER_H
#define GLHANDLER_H

#include <QColor>
#include <QDebug>
#include <QFile>
#include <QImage>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_2_Core>
#include <QSettings>
#include <QString>
#include <QVector2D>
#include <QVector3D>
#include <QVector>
#include <QtMath>
#include <QtOpenGLExtensions>
#include <array>
#include <functional>

#include "PythonQtHandler.hpp"
#include "utils.hpp"

#include "gl/GLBuffer.hpp"
#include "gl/GLComputeShader.hpp"
#include "gl/GLFramebufferObject.hpp"
#include "gl/GLMesh.hpp"
#include "gl/GLPixelBufferObject.hpp"
#include "gl/GLShaderProgram.hpp"
#include "gl/GLTexture.hpp"

/** @ingroup pycall
 *
 * @brief A convenient wrapper for the OpenGL API. Contains everything related
 * to rendering.
 *
 * Callable in Python as the "GLHandler" object.
 *
 * GLHandler is designed to be used as a static class and you shouldn't
 * instanciate it.
 *
 * Any result of a new*() method should be deleted with the corresponding
 * delete*() method to free resources as if they were new and delete
 * operators.
 *
 * @todo Write examples + one small paragraph per category.
 */
class GLHandler : public QObject
{
	Q_OBJECT
  public: // useful types
	/**
	 * @brief Representing a geometric space.
	 *
	 * HMD is not what is called hmd scaled space elsewhere in this
	 * documentation but has the same scale as tracked space.
	 *
	 * See the TRANSFORMS file.
	 */
	enum class GeometricSpace
	{
		CLIP,
		WORLD,
		EYE,
		CAMERA,
		SEATEDTRACKED,
		STANDINGTRACKED,
		HMD,
		SKYBOX
	};
	Q_ENUM(GeometricSpace)

  public:
	/**
	 * @brief Initializes the OpenGL API. No OpenGL call can be issued before
	 * this method is called, which you can consider all of the other methods
	 * do, including allocating resources through the new*() methods.
	 */
	static bool init();

	/**
	 * @brief Returns a reference to the OpenGL functions retrieved by Qt.
	 *
	 * You can call OpenGL directly through that reference, but be careful !
	 * Make sure you keep a clean OpenGL state.
	 */
	static QOpenGLFunctions_4_2_Core& glf();

	static QOpenGLExtension_ARB_compute_shader& glf_ARB_compute_shader();

  public slots:
	/**
	 * @brief Sets the point size when rendering of @ref PrimitiveType POINTS.
	 *
	 * See <a
	 * href="https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glPointSize.xhtml">glPointSize</a>.
	 */
	static void setPointSize(unsigned int size);

	static void setClearColor(QColor const& color);
	/**
	 * @brief Prepares a @ref RenderTarget to be rendered on.
	 *
	 * Binds the framebuffer, clears its color and depth buffer and adjust the
	 * viewport to coincide with the @p renderTarget's size. By default, the
	 * screen will be used.
	 */
	static void beginRendering(GLFramebufferObject const& renderTarget,
	                           GLTexture::CubemapFace face
	                           = GLTexture::CubemapFace::FRONT,
	                           GLint layer = 0);
	static void beginRendering(GLbitfield clearMask,
	                           GLFramebufferObject const& renderTarget,
	                           GLTexture::CubemapFace face
	                           = GLTexture::CubemapFace::FRONT,
	                           GLint layer = 0);
	/**
	 * @brief Sets the <code>in mat4 camera;</code> input of a shader program.
	 *
	 * The transformation matrix used will depend on the @p space parameter :
	 * * WORLD : fullTransform : from world space to clip space
	 * * EYE: fullEyeSpaceTransform : from eye space to clip space
	 * * CAMERA : fullCameraSpaceTransform : from camera space to clip space
	 * * SEATEDTRACKED : fullSeatedTrackedSpaceTransform : from seated tracked
	 * space to clip space
	 * * STANDINGTEDTRACKED : fullStandingTrackedSpaceTransform : from standing
	 * tracked space to clip space
	 * * HMD : fullHmdSpaceTransform : from hmd space (not world-scaled) to clip
	 * * SKYBOX : fullSkyboxSpaceTransform : from skybox space to clip
	 * space
	 *
	 * The @p model matrix (identity by default) will be multiplied to the
	 * camera matrix before being sent to the shader as a single MVP matrix.
	 *
	 * It doesn't have to be called before each @ref render call as long as the
	 * same shader program is used for all these following renders and that the
	 * meshes are from the same space with the same model matrix. This method
	 * isn't particularly heavy to execute but this fact can lead to a small
	 * optimization by render grouping.
	 *
	 * The @p shader passed as parameter will be used for all the following
	 * renders before another call with a different shader program replaces it.
	 */
	static void setUpRender(GLShaderProgram const& shader,
	                        QMatrix4x4 const& model = QMatrix4x4(),
	                        GeometricSpace space    = GeometricSpace::WORLD);
	/**
	 * @brief Renders @p from's color attachment onto a quad using a
	 * post-processing @p shader. The final rendering gets stored on the @p to
	 * @ref RenderTarget.
	 *
	 * By default, the final rendering is stored onto the screen.
	 *
	 * The quad is defined as a square whose coordinates range from -1 to 1.
	 * You will have to compute the texture coordinates from those coordinates.
	 *
	 * The @ref GLShaderProgram @p shader must comply with the following :
	 * * Its vertex shader must get a vec2 input named "position" to get the
	 * quad coordinates.
	 * * Its fragment shader must have a uniform sampler2D to get the @p from
	 * color attachment texture into. First sampler2D found in the fragment
	 * shader will be used.
	 * Additional textures can be sent to following sampler2Ds via the @p
	 * uniformTextures parameter.
	 */
	static void postProcess(GLShaderProgram const& shader,
	                        GLFramebufferObject const& from,
	                        GLFramebufferObject const& to,
	                        std::vector<GLTexture const*> const& uniformTextures
	                        = {});
	static void postProcess(
	    GLComputeShader const& shader, GLFramebufferObject const& inplace,
	    std::vector<
	        std::pair<GLTexture const*, GLComputeShader::DataAccessMode>> const&
	        uniformTextures
	    = {});
	static void postProcess(
	    GLComputeShader const& shader, GLFramebufferObject const& from,
	    GLFramebufferObject const& to,
	    std::vector<
	        std::pair<GLTexture const*, GLComputeShader::DataAccessMode>> const&
	        uniformTextures
	    = {});
	static void renderFromScratch(GLShaderProgram const& shader,
	                              GLFramebufferObject const& to);

	static void generateEnvironmentMap(
	    GLFramebufferObject const& renderTarget,
	    std::function<void(bool, QMatrix4x4, QMatrix4x4)> const& renderFunction,
	    QVector3D const& shift = QVector3D(0, 0, 0));
	/**
	 * @brief Begins wireframe rendering.
	 */
	static void beginWireframe();
	/**
	 * @brief Ends wireframe rendering.
	 */
	static void endWireframe();
	/**
	 * @brief Begins transparent meshes rendering.
	 *
	 * The meshes will access the depth buffer as read only to not occlude one
	 * another or themselves.
	 *
	 * @attention Therefore, it is advised to render transparent meshes last.
	 *
	 * Parameters are passed to the glBlendFunc function as is.
	 */
	static void beginTransparent(GLenum blendfuncSfactor = GL_SRC_ALPHA,
	                             GLenum blendfuncDfactor
	                             = GL_ONE_MINUS_SRC_ALPHA);
	/**
	 * @brief Ends transparent meshes rendering.
	 *
	 * @attention Even if transparent meshes are the last to be rendered in a
	 * frame, you should call endTransparent() before ending the frame rendering
	 * as the engine won't do it itself and it could make the beginning of the
	 * next frame render in transparent mode.
	 */
	static void endTransparent();
	static void setBackfaceCulling(bool on, GLenum faceToCull = GL_BACK,
	                               GLenum frontFaceWindingOrder = GL_CCW);
	/**
	 * @brief Clears depth buffer
	 *
	 * Useful to render successive levels of depth, as skyboxes for example.
	 */
	static void clearDepthBuffer();
	/**
	 * @brief Used mostly by cameras to upload their matrices to send to vertex
	 * shaders as their <code>in mat4 camera</code> input.
	 *
	 * Each matrix corresponds to a particular @ref GeometricSpace when calling
	 * @ref setUpRender.
	 *
	 * See the TRANSFORMS file for more details about the parameters.
	 */
	static void
	    setUpTransforms(QMatrix4x4 const& fullTransform,
	                    QMatrix4x4 const& fullEyeSpaceTransform,
	                    QMatrix4x4 const& fullCameraSpaceTransform,
	                    QMatrix4x4 const& fullSeatedTrackedSpaceTransform,
	                    QMatrix4x4 const& fullStandingTrackedSpaceTransform,
	                    QMatrix4x4 const& fullHmdSpaceTransform,
	                    QMatrix4x4 const& fullSkyboxSpaceTransform);

	// TEXTURES
	static void useTextures(std::vector<GLTexture const*> const& textures);

	// http://entropymine.com/imageworsener/srgbformula/
	static QColor sRGBToLinear(QColor const& srgb);
	static QColor linearTosRGB(QColor const& linear);

  private:
	// object to screen transforms
	// transform for any world object
	static QMatrix4x4& fullTransform();
	// transform for any Camera space object (follows Camera)
	static QMatrix4x4& fullEyeSpaceTransform();
	// transform for any Camera space object (follows Camera)
	static QMatrix4x4& fullCameraSpaceTransform();
	// transform for any Seated Tracked space object
	static QMatrix4x4& fullSeatedTrackedSpaceTransform();
	// transform for any Standing Tracked space object
	static QMatrix4x4& fullStandingTrackedSpaceTransform();
	// transform for any HMD space object (follows HMD)
	static QMatrix4x4& fullHmdSpaceTransform();
	// transform for any Skybox space object (follows HMD translations + no
	// stereo)
	static QMatrix4x4& fullSkyboxSpaceTransform();
};

#endif // GLHANDLER_H
