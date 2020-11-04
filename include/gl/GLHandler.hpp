#ifndef GLHANDLER_H
#define GLHANDLER_H

#include <QColor>
#include <QDebug>
#include <QFile>
#include <QImage>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_0_Core>
#include <QSettings>
#include <QString>
#include <QVector2D>
#include <QVector3D>
#include <QVector>
#include <QtMath>
#include <array>
#include <functional>

#include "PythonQtHandler.hpp"
#include "utils.hpp"

#include "gl/GLMesh.hpp"
#include "gl/GLShaderProgram.hpp"

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
	static unsigned int& renderTargetCount();
	static unsigned int& meshCount();
	static unsigned int& texCount();
	static unsigned int& PBOCount();
	/** @ingroup pywrap
	 * @brief Opaque class that represents a Texture. Use the texture related
	 * methods to handle it.
	 *
	 * A GLHandler#Texture represents a texture as it sits on the GPU and
	 * is manipulated by OpenGL API calls, they are not CPU arrays of data.
	 */
	class Texture
	{
		friend GLHandler;
		GLuint glTexture;
		GLenum glTarget;
	};

	class PixelBufferObject
	{
		friend GLHandler;
		GLuint id;

	  public:
		unsigned int width;
		unsigned int height;
		unsigned char* mappedData;

		PixelBufferObject()                         = default;
		PixelBufferObject(PixelBufferObject const&) = default;
		PixelBufferObject(PixelBufferObject&&)      = default;
		PixelBufferObject& operator=(PixelBufferObject const&) = default;
	};

	/** @ingroup pywrap
	 * @brief Mostly opaque class that represents a render target. Use the
	 * render target related methods to handle it.
	 *
	 * A render target is a collection of buffers in video memory on which a
	 * scene is rendered. The default render target is the screen. Having
	 * several other render targets than the screen can be useful for VR
	 * rendering (one per eye) or post-processing for example (render the scene
	 * on a RenderTarget then pass it to a post-processing shader that is
	 * rendering to the screen buffer). About post-processing, @ref GLHandler as
	 * high level methods to do it, you shouldn't have to do post-processing
	 * manually.
	 *
	 * To be more specific, a RenderTarget contains pointers to a framebuffer, a
	 * color attachment buffer and a render buffer (depth + stencil). Deferred
	 * rendering is therefore not supported yet. It also keeps its attachments
	 * size (width + height).
	 */
	class RenderTarget
	{
		friend GLHandler;
		RenderTarget(unsigned int width, unsigned int height)
		    : width(width)
		    , height(height)
		    , depth(1){};
		RenderTarget(unsigned int width, unsigned int height,
		             unsigned int depth)
		    : width(width)
		    , height(height)
		    , depth(depth){};
		RenderTarget(GLuint _0, Texture _1, GLuint _2, unsigned int _3,
		             unsigned int _4, unsigned int _5 = 1)
		    : frameBuffer(_0)
		    , texColorBuffer(_1)
		    , renderBuffer(_2)
		    , width(_3)
		    , height(_4)
		    , depth(_5){};
		GLuint frameBuffer = 0;
		// if depth map, will be the depth buffer instead
		Texture texColorBuffer = {};
		GLuint renderBuffer    = 0;
		unsigned int width;
		unsigned int height;
		unsigned int depth;
		bool isDepthMap = false;

	  public:
		/**
		 * @brief Default constructor.
		 *
		 * @warning You should use GLHandler#newRenderTarget instead.
		 */
		RenderTarget() = default;
		/**
		 * @brief Copy constructor.
		 */
		RenderTarget(RenderTarget const&) = default;
		/**
		 * @brief Move constructor.
		 */
		RenderTarget(RenderTarget&&) = default;
		/**
		 * @brief Assignment operator.
		 */
		RenderTarget& operator=(RenderTarget const&) = default;
		QSize getSize() const { return QSize(width, height); };
	};

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
		CAMERA,
		SEATEDTRACKED,
		STANDINGTRACKED,
		HMD,
		SKYBOX
	};
	Q_ENUM(GeometricSpace)

	/**
	 * @brief Representing a cube face (used mostly for cubemaps).
	 *
	 * Front : X+
	 *
	 * Back : X-
	 *
	 * Left : Y+
	 *
	 * Right : Y-
	 *
	 * Top : Z+
	 *
	 * Bottom : Z-
	 */
	enum class CubeFace
	{
		FRONT = GL_TEXTURE_CUBE_MAP_POSITIVE_X - GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		BACK  = GL_TEXTURE_CUBE_MAP_NEGATIVE_X - GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		LEFT  = GL_TEXTURE_CUBE_MAP_POSITIVE_Y - GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		RIGHT = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y - GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		TOP   = GL_TEXTURE_CUBE_MAP_POSITIVE_Z - GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		BOTTOM
		= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z - GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	};

  public:
	/**
	 * @brief Default constructor.
	 * @warning Should only be used within @ref PythonQtHandler.
	 *
	 * Adds RenderTarget and #Texture classes to the
	 * Python API.
	 */
	GLHandler();

	/**
	 * @brief Initializes the OpenGL API. No OpenGL call can be issued before
	 * this method is called, which you can consider all of the other methods
	 * do, including allocating resources through the new*() methods.
	 */
	static bool init();

	/**
	 * @brief Returns a reference to the default render target format.
	 *
	 * This default render target format will be used in
	 * GLHandler#newRenderTarget if no format is provided. This is mostly used
	 * to ensure that any part of the engine instantiating a new render target
	 * uses this particular format.
	 *
	 * By default, contains GL_RGBA32F.
	 */
	static GLint& defaultRenderTargetFormat();

	/**
	 * @brief Returns a reference to the OpenGL functions retrieved by Qt.
	 *
	 * You can call OpenGL directly through that reference, but be careful !
	 * Make sure you keep a clean OpenGL state.
	 */
	static QOpenGLFunctions_4_0_Core& glf();

  public slots:
	/**
	 * @brief Sets the point size when rendering of @ref PrimitiveType POINTS.
	 *
	 * See <a
	 * href="https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glPointSize.xhtml">glPointSize</a>.
	 */
	static void setPointSize(unsigned int size);

	// RENDERING
	static RenderTarget newRenderTarget1D(unsigned int width);
	static RenderTarget newRenderTarget1D(unsigned int width, GLint format);
	/**
	 * @brief Calls @ref newRenderTarget(@p width, @p height, @ref
	 * defaultRenderTargetFormat()).
	 */
	static RenderTarget newRenderTarget(unsigned int width, unsigned int height,
	                                    bool cubemap = false);
	/**
	 * @brief Allocates a new @ref RenderTarget.
	 * @param width Width of all buffers of the render target.
	 * @param height Height of all buffers the render target.
	 * @param format Internal format of the color attachment of the render
	 * target. See <a
	 * href="https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml">glTexImage2D's
	 * documentation</a> to have a table of all possible internal formats for
	 * textures.
	 */
	static RenderTarget newRenderTarget(unsigned int width, unsigned int height,
	                                    GLint format, bool cubemap = false);

	static RenderTarget newRenderTargetMultisample(unsigned int width,
	                                               unsigned int height,
	                                               unsigned int samples,
	                                               GLint format);

	static RenderTarget newRenderTarget3D(unsigned int width,
	                                      unsigned int height,
	                                      unsigned int depth);
	static RenderTarget newRenderTarget3D(unsigned int width,
	                                      unsigned int height,
	                                      unsigned int depth, GLint format);

	static RenderTarget newDepthMap(unsigned int width, unsigned int height,
	                                bool cubemap = false);
	/**
	 * @brief Returns the color attachment of a @p renderTarget.
	 *
	 * This is mainly used for post-processing to be passed to a post-processing
	 * fragment shader.
	 */
	static Texture
	    getColorAttachmentTexture(GLHandler::RenderTarget const& renderTarget);
	/**
	 * @brief Blits one color buffer from a render target to another one's.
	 */
	static void blitColorBuffer(RenderTarget const& from,
	                            RenderTarget const& to);
	static void blitColorBuffer(RenderTarget const& from,
	                            RenderTarget const& to, int srcX0, int srcY0,
	                            int srcX1, int srcY1, int dstX0, int dstY0,
	                            int dstX1, int dstY1);
	/**
	 * @brief Blits one depth buffer from a render target to another one's.
	 */
	static void blitDepthBuffer(RenderTarget const& from,
	                            RenderTarget const& to);
	/**
	 * @brief Frees a @ref RenderTarget and any buffer it allocated.
	 */
	static void deleteRenderTarget(GLHandler::RenderTarget const& renderTarget);
	static void setClearColor(QColor const& color);
	/**
	 * @brief Prepares a @ref RenderTarget to be rendered on.
	 *
	 * Binds the framebuffer, clears its color and depth buffer and adjust the
	 * viewport to coincide with the @p renderTarget's size. By default, the
	 * screen will be used.
	 */
	static void beginRendering(GLHandler::RenderTarget const& renderTarget
	                           = {QSettings().value("window/width").toUInt(),
	                              QSettings().value("window/height").toUInt()},
	                           CubeFace face = CubeFace::FRONT,
	                           GLint layer   = 0);
	static void beginRendering(GLbitfield clearMask,
	                           GLHandler::RenderTarget const& renderTarget
	                           = {QSettings().value("window/width").toUInt(),
	                              QSettings().value("window/height").toUInt()},
	                           CubeFace face = CubeFace::FRONT,
	                           GLint layer   = 0);
	/**
	 * @brief Sets the <code>in mat4 camera;</code> input of a shader program.
	 *
	 * The transformation matrix used will depend on the @p space parameter :
	 * * WORLD : fullTransform : from world space to clip space
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
	                        GLHandler::RenderTarget const& from,
	                        RenderTarget const& to
	                        = {QSettings().value("window/width").toUInt(),
	                           QSettings().value("window/height").toUInt()},
	                        std::vector<Texture> const& uniformTextures = {});
	static void
	    renderFromScratch(GLShaderProgram const& shader,
	                      RenderTarget const& to
	                      = {QSettings().value("window/width").toUInt(),
	                         QSettings().value("window/height").toUInt()});

	static RenderTarget getScreenRenderTarget()
	{
		return {QSettings().value("window/width").toUInt(),
		        QSettings().value("window/height").toUInt()};
	};

	static void generateEnvironmentMap(
	    GLHandler::RenderTarget const& renderTarget,
	    std::function<void(bool, QMatrix4x4, QMatrix4x4)> const& renderFunction,
	    QVector3D const& shift = QVector3D(0, 0, 0));
	/**
	 * @brief Shows a @p renderTarget content on screen
	 *
	 * It will be displayed as rectangle which top-left coordinates are (@p
	 * screenx0, @p screeny0) and bottom-right coordinates are (@p screenx1, @p
	 * screeny1).
	 *
	 * Coordinates are from window space (0->width, 0->height).
	 */
	static void showOnScreen(GLHandler::RenderTarget const& renderTarget,
	                         int screenx0, int screeny0, int screenx1,
	                         int screeny1);
	/**
	 * @brief Copies the color content of a RenderTarget in a QImage.
	 */
	static QImage
	    generateScreenshot(RenderTarget const& renderTarget
	                       = {QSettings().value("window/width").toUInt(),
	                          QSettings().value("window/height").toUInt()});
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
	                    QMatrix4x4 const& fullCameraSpaceTransform,
	                    QMatrix4x4 const& fullSeatedTrackedSpaceTransform,
	                    QMatrix4x4 const& fullStandingTrackedSpaceTransform,
	                    QMatrix4x4 const& fullHmdSpaceTransform,
	                    QMatrix4x4 const& fullSkyboxSpaceTransform);

	// TEXTURES
	static Texture newTexture(unsigned int width, const GLvoid* data,
	                          bool sRGB = true);
	static Texture newTexture(unsigned int width, const unsigned char* red,
	                          const unsigned char* green,
	                          const unsigned char* blue,
	                          const unsigned char* alpha = nullptr,
	                          bool sRGB                  = true);
	static Texture newTexture(const char* texturePath, bool sRGB = true);
	static Texture newTexture(QImage const& image, bool sRGB = true);
	static Texture newTexture(unsigned int width, unsigned int height,
	                          const GLvoid* data, bool sRGB = true);
	static Texture newTexture(std::array<const char*, 6> const& texturesPaths,
	                          bool sRGB = true);
	static Texture newTexture(std::array<QImage, 6> const& images,
	                          bool sRGB = true);
	static Texture
	    newTexture1D(unsigned int width, GLvoid const* data = nullptr,
	                 GLint internalFormat = GL_SRGB8_ALPHA8,
	                 GLenum format = GL_RGBA, GLenum target = GL_TEXTURE_1D,
	                 GLint filter = GL_LINEAR, GLint wrap = GL_CLAMP_TO_EDGE,
	                 GLenum type = GL_UNSIGNED_BYTE);
	static Texture newTexture2D(
	    unsigned int width, unsigned int height, GLvoid const* data = nullptr,
	    GLint internalFormat = GL_SRGB8_ALPHA8, GLenum format = GL_RGBA,
	    GLenum target = GL_TEXTURE_2D, GLint filter = GL_LINEAR,
	    GLint wrap = GL_CLAMP_TO_EDGE, GLenum type = GL_UNSIGNED_BYTE);
	static Texture newTextureMultisample(unsigned int width,
	                                     unsigned int height,
	                                     unsigned int samples,
	                                     GLint internalFormat = GL_SRGB8_ALPHA8,
	                                     GLint filter         = GL_LINEAR,
	                                     GLint wrap = GL_CLAMP_TO_EDGE);
	static Texture
	    newTexture3D(unsigned int width, unsigned int height,
	                 unsigned int depth, GLvoid const* data = nullptr,
	                 GLint internalFormat = GL_SRGB8_ALPHA8,
	                 GLenum format = GL_RGBA, GLenum target = GL_TEXTURE_3D,
	                 GLint filter = GL_LINEAR, GLint wrap = GL_CLAMP_TO_EDGE,
	                 GLenum type = GL_UNSIGNED_BYTE);
	static Texture newTextureCubemap(
	    unsigned int side,
	    std::array<GLvoid const*, 6> data
	    = {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
	    GLint internalFormat = GL_SRGB8_ALPHA8, GLenum format = GL_RGBA,
	    GLenum target = GL_TEXTURE_CUBE_MAP, GLint filter = GL_LINEAR,
	    GLint wrap = GL_CLAMP_TO_EDGE);
	static GLuint getGLTexture(Texture const& tex) { return tex.glTexture; };
	// level = level of mipmapping
	static QSize getTextureSize(Texture const& tex, unsigned int level = 0);
	static void generateMipmap(Texture const& tex);
	static unsigned int getHighestMipmapLevel(Texture const& tex);
	static QImage getTextureContentAsImage(Texture const& tex,
	                                       unsigned int level = 0);
	// allocates buff ; don't forget to delete ; returns allocated size (zero if
	// error)
	static unsigned int getTextureContentAsData(GLfloat** buff,
	                                            Texture const& tex,
	                                            unsigned int level = 0);
	static float getTextureAverageLuminance(Texture const& tex);
	static void useTextures(std::vector<Texture> const& textures);
	static void deleteTexture(Texture const& texture);

	// PBOs
	static PixelBufferObject newPixelBufferObject(unsigned int width,
	                                              unsigned int height);
	static Texture copyPBOToTex(PixelBufferObject const& pbo, bool sRGB = true);
	static void deletePixelBufferObject(PixelBufferObject const& pbo);

	// http://entropymine.com/imageworsener/srgbformula/
	static QColor sRGBToLinear(QColor const& srgb);
	static QColor linearTosRGB(QColor const& linear);

  private:
	static QString
	    getFullPreprocessedSource(QString const& path,
	                              QMap<QString, QString> const& defines);
	static GLuint loadShader(QString const& path, GLenum shaderType,
	                         QMap<QString, QString> const& defines);

	// object to screen transforms
	// transform for any world object
	static QMatrix4x4& fullTransform();
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

Q_DECLARE_METATYPE(GLHandler::Texture)
Q_DECLARE_METATYPE(GLHandler::RenderTarget)

#endif // GLHANDLER_H
