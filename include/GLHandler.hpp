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
	static unsigned int& shaderCount();
	static unsigned int& meshCount();
	static unsigned int& texCount();
	static unsigned int& PBOCount();
	/** @ingroup pywrap
	 * @brief Opaque class that represents a mesh. Use the mesh related methods
	 * to handle it.
	 *
	 * A mesh is a set of vertices that can be rendered by the GPU. These
	 * vertices can be described as positions or by more than that (colors,
	 * normals, etc...).
	 *
	 * To be more specific, a Mesh contains a Vertex Array Object, a Vertex
	 * Buffer Object and an optional Element Buffer Object along with their
	 * sizes. It doesn't handle model matrix or shaders, it is a very basic
	 * vertex data storage class.
	 */
	class Mesh
	{
		friend GLHandler;
		GLuint vao;
		GLuint vbo;
		GLuint ebo;
		unsigned int vboSize;
		unsigned int eboSize;
	};
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
		    , height(height){};
		RenderTarget(GLuint _0, Texture _1, GLuint _2, unsigned int _3,
		             unsigned int _4)
		    : frameBuffer(_0)
		    , texColorBuffer(_1)
		    , renderBuffer(_2)
		    , width(_3)
		    , height(_4){};
		GLuint frameBuffer     = 0;
		Texture texColorBuffer = {};
		GLuint renderBuffer    = 0;
		unsigned int width;
		unsigned int height;

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
	};

	/** @ingroup pywrap
	 * @brief Opaque type that represents a Shader Program. Use the shaders
	 * related methods to handle it.
	 *
	 * A valid shader program is at least composed out of a vertex shader and a
	 * fragment shader.
	 */
	typedef GLuint ShaderProgram;

	/**
	 * @brief Represents a type of primitive for rasterization.
	 *
	 * AUTO is set to POINTS if no elements are given when setting the mesh's
	 * vertices, and TRIANGLES otherwise.
	 *
	 * See <a href="https://www.khronos.org/opengl/wiki/Primitive">OpenGL
	 * Primitive</a>.
	 */
	enum class PrimitiveType
	{
		POINTS         = GL_POINTS,
		LINES          = GL_LINES,
		TRIANGLES      = GL_TRIANGLES,
		TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
		AUTO // if no ebo, POINTS, else TRIANGLES
	};
	Q_ENUM(PrimitiveType)

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
	 * Adds Mesh, RenderTarget, #Texture and #ShaderProgram classes to the
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
	 * uses this particular format (if HDR rendering is enabled or disabled for
	 * exemple).
	 *
	 * By default, if HDR rendering if off, will contain GL_RGBA8, and if HDR
	 * rendering is on, will contain GL_RGBA16F.
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
	/**
	 * @brief Returns the color attachment of a @p renderTarget.
	 *
	 * This is mainly used for post-processing to be passed to a post-processing
	 * fragment shader.
	 */
	static Texture
	    getColorAttachmentTexture(GLHandler::RenderTarget const& renderTarget);
	/**
	 * @brief Frees a @ref RenderTarget and any buffer it allocated.
	 */
	static void deleteRenderTarget(GLHandler::RenderTarget const& renderTarget);
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
	                           CubeFace face = CubeFace::FRONT);
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
	 * The @ref ShaderProgram @p shader must comply with the following :
	 * * Its vertex shader must get a vec2 input named "position" to get the
	 * quad coordinates.
	 * * Its fragment shader must have a uniform sampler2D to get the @p from
	 * color attachment texture into. First sampler2D found in the fragment
	 * shader will be used.
	 * Additional textures can be sent to following sampler2Ds via the @p
	 * uniformTextures parameter.
	 */
	static void postProcess(ShaderProgram shader,
	                        GLHandler::RenderTarget const& from,
	                        RenderTarget const& to
	                        = {QSettings().value("window/width").toUInt(),
	                           QSettings().value("window/height").toUInt()},
	                        std::vector<Texture> const& uniformTextures = {});

	static RenderTarget getScreenRenderTarget()
	{
		return {QSettings().value("window/width").toUInt(),
		        QSettings().value("window/height").toUInt()};
	};

	static void
	    generateEnvironmentMap(GLHandler::RenderTarget const& renderTarget,
	                           std::function<void()> const& renderFunction,
	                           QVector3D const& position
	                           = QVector3D(0.f, 0.f, 0.f));
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

	// SHADERS
	/**
	 * @brief Convenient shortcut for newShader(@p shadersCommonName, @p
	 * shadersCommonName, @p shadersCommonName).
	 */
	static ShaderProgram newShader(QString const& shadersCommonName,
	                               QMap<QString, QString> const& defines = {});
	/**
	 * @brief Allocates an OpenGL shader program consisting of a vertex shader
	 * and a fragment shader.
	 *
	 * @warning Geometry shaders aren't supported as of now.
	 *
	 * @warning If something is wrong with the shader code, you won't be able to
	 * know in-code as of this version. A qWarning() will be issued, so watch
	 * the standard output if something doesn't render.
	 *
	 * @attention The fragment shader output color must be called "outColor".
	 *
	 * The returned shader program isn't shared anywhere so it is safe to set
	 * its constant uniforms once in its entire lifetime, they won't be
	 * overriden by any side effect.
	 *
	 * You can specify shader names as any path, any data path or only provide
	 * the file name without its extension if it is stored in the data folder
	 * data/ * /shaders and has .vert or .frag extension for vertex shader or
	 * fragment shader respectively. For example, if you want to refer to the
	 * vertex shader located in data/core/shaders/default.vert, you can pass
	 * "default" as the @p vertexName parameter or "shaders/default.vertex" or
	 * its complete path. This allows usage of the convenient function @ref
	 * newShader(QString const&) when both vertex and fragment shader share the
	 * same name : newShader("default") will search for both
	 * "shaders/default.vert" and "shader/default.frag".
	 *
	 * As of this version, the vertex and fragment shaders linked in the shader
	 * program are freed and will have to be compiled again if @ref newShader is
	 * called another time with the same parameters.
	 */
	static ShaderProgram newShader(QString vertexName, QString fragmentName,
	                               QMap<QString, QString> const& defines = {},
	                               QString geometryName                  = "");

  public: // doesn't work in PythonQt
	/** @brief Sets values for vertex attributes that aren't provided by a
	 * vertex array.
	 *
	 * Useful if one shader is used for different rendering methods that
	 * don't provide the same level of information.
	 *
	 * For example, if your shader takes "in float luminosity;" as input but
	 * your vertex buffer only contains vertex positions, you have to use this
	 * method to set luminosity to 1.f for example. It will disable the vertex
	 * attribute array for "luminosity" and use 1.f instead.
	 *
	 * @param shader Shader for which to set values.
	 * @param defaultValues List of pairs of attribute name + default value (ex:
	 * ("luminosity", {1.f}) or ("normal", {1.f, 0.f, 0.f})). First element of
	 * the pair is the name of the attribute, and second element is the
	 * multidimensional value. The size of the given vector will determine the
	 * type (1 = float, 2 = vec2, 3 = vec3 and 4 = vec4).
	 */
	static void setShaderUnusedAttributesValues(
	    ShaderProgram shader,
	    std::vector<QPair<const char*, std::vector<float>>> const&
	        defaultValues);
  public slots:
	/**
	 * @brief Convenient version of the @ref
	 * setShaderUnusedAttributesValues(ShaderProgram, std::vector<QPair<const
	 * char*, std::vector<float>>>const&) method to be used in Python.
	 *
	 * Behaves the same way as its other version, but the attribute mapping is
	 * specified differently, as it is harder to construct a QVector<QPair>
	 * object in Python. Instead of an array of pairs (name, values), the
	 * mapping is specified by all the ordered names in the @p names
	 * parameter and all their corresponding values in the same order in the @p
	 * values parameter.
	 */
	static void setShaderUnusedAttributesValues(
	    ShaderProgram shader, QStringList const& names,
	    std::vector<std::vector<float>> const& values);
	/**
	 * @brief Sets the @p shader program's uniform @p paramName to a certain @p
	 * value.
	 *
	 * The uniform must be of type int.
	 */
	static void setShaderParam(ShaderProgram shader, const char* paramName,
	                           int value);
	/**
	 * @brief Sets the @p shader program's uniform @p paramName to a certain @p
	 * value.
	 *
	 * The uniform must be of type float.
	 */
	static void setShaderParam(ShaderProgram shader, const char* paramName,
	                           float value);
	/**
	 * @brief Sets the @p shader program's uniform @p paramName to a certain @p
	 * value.
	 *
	 * The uniform must be of type vec2.
	 */
	static void setShaderParam(ShaderProgram shader, const char* paramName,
	                           QVector2D const& value);
	/**
	 * @brief Sets the @p shader program's uniform @p paramName to a certain @p
	 * value.
	 *
	 * The uniform must be of type vec3.
	 */
	static void setShaderParam(ShaderProgram shader, const char* paramName,
	                           QVector3D const& value);
	/**
	 * @brief Sets the @p shader program's uniform @p paramName to a certain @p
	 * array of values of size @size.
	 *
	 * The uniform must be an array of type vec3.
	 */
	static void setShaderParam(ShaderProgram shader, const char* paramName,
	                           unsigned int size, QVector3D const* values);
	/**
	 * @brief Sets the @p shader program's uniform @p paramName to a certain @p
	 * value.
	 *
	 * The uniform must be of type vec4.
	 */
	static void setShaderParam(ShaderProgram shader, const char* paramName,
	                           QVector4D const& value);
	/**
	 * @brief Sets the @p shader program's uniform @p paramName to a certain @p
	 * array of values of size @size.
	 *
	 * The uniform must be an array of type vec4.
	 */
	static void setShaderParam(ShaderProgram shader, const char* paramName,
	                           unsigned int size, QVector4D const* values);
	/**
	 * @brief Sets the @p shader program's uniform @p paramName to a certain @p
	 * value.
	 *
	 * The uniform must be of type mat4.
	 */
	static void setShaderParam(ShaderProgram shader, const char* paramName,
	                           QMatrix4x4 const& value);
	/**
	 * @brief Sets the @p shader program's uniform @p paramName to a certain @p
	 * value.
	 *
	 * The uniform must be of type vec3. If the color is from @p sRGB space, it
	 * will be converted to linear space before being set.
	 */
	static void setShaderParam(ShaderProgram shader, const char* paramName,
	                           QColor const& value, bool sRGB = true);
	/**
	 * @brief Tells OpenGL to use this @p shader program for rendering.
	 *
	 * If you exclusively use @ref GLHandler methods, this method will be called
	 * automatically when necessary and you shouldn't use it.
	 */
	static void useShader(ShaderProgram shader);
	/**
	 * @brief Frees a @ref ShaderProgram.
	 */
	static void deleteShader(ShaderProgram shader);

	// MESHES
	/**
	 * @brief Allocates a new @ref Mesh.
	 */
	static Mesh newMesh();

  public: // doesn't work in PythonQt
	/**
	 * @brief Sets vertices data for a mesh.
	 *
	 * The vertices are specified in the "array of structures" order, "structure
	 * of arrays" isn't supported as of this version.
	 *
	 * @param mesh @ref Mesh for which the vertices are set.
	 *
	 * @param vertices Vertices data. Can be any set of floating point numbers.
	 * Usually, it will at least contain tridimentional positional information.
	 * The data must be linearized. For example, if a vertex must have two
	 * attributes, a 3D position p and a 2D texture coordinate tc, one vertex
	 * will be represented by the 5 values { p0, p1, p2, tc0, tc1 }. Vertices
	 * are stored in a contiguous way : { vertex 0, vertex 1, ... } or more
	 * precisely {p0,0, p0,1, p0,2, tc0,0, tc0,1, p1,0, p1,1, p1,2, tc1,0,
	 * tc1,1} to follow the previous example (where pi,j is j-th component of
	 * the ith vertex position and tci,j is the j-th component of the ith vertex
	 * texture coordinates). Therefore @p vertices is a 1D array of size (number
	 * of vertices) * (sum of the dimensions of one vertex attributes), in our
	 * example (number of vertices) * 5.
	 *
	 * @param shaderProgram Which shader program will be used to render the
	 * mesh. This is used to get the attributes locations to set in the Vertex
	 * Array Object.
	 *
	 * @param mapping An array of ordered pairs which informs on how to map
	 * attributes from the @p vertices parameter to the @p shaderProgram inputs.
	 * Each pair's first element is the name of the attribute as stated in the
	 * @p shaderProgram and its second element is the dimension of this
	 * attribute. For the earlier example given, @p mapping should be set as
	 * {{"position", 3}, {"texcoord", 2}}, if @p shaderProgram's vertex array
	 * contains the two inputs <code>in vec3 position;</code> and <code>in vec2
	 * texcoord;</code> and positions are specified in @p vertices before
	 * texture coordiantes are (which is the case - {p0, tc0, p1, tc1, ...}).
	 *
	 * @param elements Optional. Content of the element buffer object that will
	 * specify vertices rendering order. If none is provided, the order of @p
	 * vertices will be followed. More technically, glDrawArrays will be called
	 * if this parameter is empty, and glDrawElements will be called otherwise.
	 * Also, if this parameter is empty, the automatic @ref PrimitiveType for
	 * the mesh will be POINTS.
	 */
	static void setVertices(
	    GLHandler::Mesh& mesh, std::vector<float> const& vertices,
	    ShaderProgram const& shaderProgram,
	    std::vector<QPair<const char*, unsigned int>> const& mapping,
	    std::vector<unsigned int> const& elements = {});
  public slots:
	/**
	 * @brief Convenient version of the @ref setVertices(GLHandler::Mesh&,
	 * std::vector<float>const&, ShaderProgram const&, std::vector<QPair<const
	 * char*, unsigned int>>const&, std::vector<unsigned int>const&) method to
	 * be used in Python.
	 *
	 * Behaves the same way as its other version, but the attribute mapping is
	 * specified differently, as it is harder to construct a QVector<QPair>
	 * object in Python. Instead of an array of pairs (name, size), the
	 * mapping is specified by all the ordered names in the @p mappingNames
	 * parameter and all their corresponding sizes in the same order in the @p
	 * mappingSizes parameter.
	 */
	static void setVertices(GLHandler::Mesh& mesh,
	                        std::vector<float> const& vertices,
	                        ShaderProgram const& shaderProgram,
	                        QStringList const& mappingNames,
	                        std::vector<unsigned int> const& mappingSizes,
	                        std::vector<unsigned int> const& elements = {});
	/**
	 * @brief Updates a mesh vertices data.
	 *
	 * @ref setVertices must have already been called once before to set the
	 * attributes mapping with the rendering shader. If the attributes mapping
	 * has to change, use @ref setVertices instead.
	 *
	 * The main use case of this method is to update each vertex data for
	 * animations for example. The data isn't supposed to be too different than
	 * it was when @ref setVertices was called (same number of vertices with the
	 * same elements buffer and same way to specify the attributes...).
	 *
	 * @param mesh @ref Mesh for which the vertices data are updated.
	 * @param vertices See @ref setVertices. This parameter must be the same
	 * format as the @p vertices parameter of @ref setVertices.
	 */
	static void updateVertices(GLHandler::Mesh& mesh,
	                           std::vector<float> const& vertices);
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
	static void setUpRender(ShaderProgram shader,
	                        QMatrix4x4 const& model = QMatrix4x4(),
	                        GeometricSpace space    = GeometricSpace::WORLD);
	/**
	 * @brief Draws a mesh on the current render target.
	 *
	 * @attention Make sure you called @ref setUpRender accordingly before
	 * calling this method.
	 *
	 * @attention This rendering will use the last @ref ShaderProgram passed to
	 * @ref setUpRender to draw the mesh. You can override the used shader
	 * program by usinga @ref useShader. Just make sure the shader you want to
	 * use has already been passed to @ref setUpRender with the correct
	 * parameters before.
	 *
	 * @param mesh @ref Mesh to be drawn.
	 * @param primitiveType @ref PrimitiveType of the mesh. If AUTO and the mesh
	 * doesn't have elements, POINTS will be assumed, but if the mesh has
	 * elements, TRIANGLES will be assumed.
	 */
	static void render(GLHandler::Mesh const& mesh,
	                   PrimitiveType primitiveType = PrimitiveType::AUTO);
	/**
	 * @brief Frees a @ref Mesh and all the vertex-related OpenGL objects it
	 * allocated before.
	 */
	static void deleteMesh(GLHandler::Mesh const& mesh);

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
	                 GLint filter = GL_LINEAR, GLint wrap = GL_CLAMP_TO_EDGE);
	static Texture newTexture2D(unsigned int width, unsigned int height,
	                            GLvoid const* data   = nullptr,
	                            GLint internalFormat = GL_SRGB8_ALPHA8,
	                            GLenum format        = GL_RGBA,
	                            GLenum target        = GL_TEXTURE_2D,
	                            GLint filter         = GL_LINEAR,
	                            GLint wrap           = GL_CLAMP_TO_EDGE);
	static Texture newTextureCubemap(
	    unsigned int side,
	    std::array<GLvoid const*, 6> data
	    = {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
	    GLint internalFormat = GL_SRGB8_ALPHA8, GLenum format = GL_RGBA,
	    GLenum target = GL_TEXTURE_CUBE_MAP, GLint filter = GL_LINEAR,
	    GLint wrap = GL_CLAMP_TO_EDGE);
	static GLuint getGLTexture(Texture const& tex) { return tex.glTexture; };
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

Q_DECLARE_METATYPE(GLHandler::Mesh)
Q_DECLARE_METATYPE(GLHandler::Texture)
Q_DECLARE_METATYPE(GLHandler::RenderTarget)
Q_DECLARE_METATYPE(GLuint) // for typedefs Texture and ShaderProgram

#endif // GLHANDLER_H
