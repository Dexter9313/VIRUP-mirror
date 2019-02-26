#ifndef GLHANDLER_H
#define GLHANDLER_H

#include <QColor>
#include <QDebug>
#include <QFile>
#include <QImage>
#include <QtMath>
#include <QMatrix4x4>
#include <QOpenGLFunctions_4_0_Core>
#include <QSettings>
#include <QString>
#include <QVector3D>
#include <QVector>

#include "PythonQtHandler.hpp"
#include "utils.hpp"

class GLHandler : public QObject
{
	Q_OBJECT
  public: // useful types
	class Mesh
	{
		friend GLHandler;
		GLuint vao;
		GLuint vbo;
		GLuint ebo;
		unsigned int vboSize;
		unsigned int eboSize;
	};

	class RenderTarget
	{
		friend GLHandler;
		RenderTarget(GLuint _0, GLuint _1, GLuint _2, unsigned int _3, unsigned int _4) : frameBuffer(_0), texColorBuffer(_1), renderBuffer(_2), width(_3), height(_4) {};
		GLuint frameBuffer;
		GLuint texColorBuffer;
		GLuint renderBuffer;
		unsigned int width;
		unsigned int height;
	  public:
		RenderTarget() = default;
		RenderTarget(RenderTarget const&) = default;
		RenderTarget(RenderTarget&&) = default;
		RenderTarget& operator=(RenderTarget const&) = default;
	};

	typedef GLuint Texture;
	typedef GLuint ShaderProgram;

	enum class PrimitiveType
	{
		POINTS    = GL_POINTS,
		LINES     = GL_LINES,
		TRIANGLES = GL_TRIANGLES,
		TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
		AUTO // if no ebo, POINTS, else TRIANGLES
	};
	Q_ENUM(PrimitiveType)

	enum class GeometricSpace
	{
		WORLD,
		CAMERA,
		TRACKED,
		HMD
	};
	Q_ENUM(GeometricSpace)

  public:
	// should only be used within PythonQtHandler
	GLHandler();

	static bool init();

	static GLint defaultRenderTargetFormat;

  public slots:
	static void setPointSize(unsigned int size);

	// rendering
	static RenderTarget newRenderTarget(unsigned int width,
	                                    unsigned int height);
	static RenderTarget newRenderTarget(unsigned int width, unsigned int height,
	                                    GLint format);
	static Texture getColorAttachmentTexture(GLHandler::RenderTarget const& renderTarget);
	static void deleteRenderTarget(GLHandler::RenderTarget const& renderTarget);
	static void beginRendering(GLHandler::RenderTarget const& renderTarget
	                           = {0, 0, 0,
	                              QSettings().value("window/width").toUInt(),
	                              QSettings().value("window/height").toUInt()});
	static void postProcess(ShaderProgram shader, GLHandler::RenderTarget const& from,
	                        RenderTarget const& to
	                        = {0, 0, 0,
	                           QSettings().value("window/width").toUInt(),
	                           QSettings().value("window/height").toUInt()});
	static void showOnScreen(GLHandler::RenderTarget const& renderTarget,
	                         int screenx0, int screeny0, int screenx1, int screeny1);
	static void beginTransparent();
	static void endTransparent();
	static void setUpTransforms(QMatrix4x4 const& fullTransform,
	                            QMatrix4x4 const& fullCameraSpaceTransform,
	                            QMatrix4x4 const& fullTrackedSpaceTransform,
	                            QMatrix4x4 const& fullHmdSpaceTransform);

	// shaders
	static ShaderProgram newShader(QString const& shadersCommonName);
	static ShaderProgram newShader(QString vertex, QString fragment,
	                               QString geometry = "");
	static void setShaderParam(ShaderProgram shader, const char* paramName,
	                           float value);
	static void setShaderParam(ShaderProgram shader, const char* paramName,
	                           QVector3D const& value);
	static void setShaderParam(ShaderProgram shader, const char* paramName,
	                           QMatrix4x4 const& value);
	static void setShaderParam(ShaderProgram shader, const char* paramName,
	                           QColor const& value, bool sRGB = true);
	static void useShader(ShaderProgram shader);
	static void deleteShader(ShaderProgram shaderProgram);

	// meshes
	static Mesh newMesh();
  public: // doesn't work in PythonQt
	static void setVertices(GLHandler::Mesh& mesh, std::vector<float> const& vertices,
	                        ShaderProgram const& shaderProgram,
	                        std::vector<QPair<const char*, unsigned int>> const& mapping,
	                        std::vector<unsigned int> const& elements = {});
  public slots:
	static void setVertices(GLHandler::Mesh& mesh, std::vector<float> const& vertices,
	                        ShaderProgram const& shaderProgram,
	                        QStringList const& mappingNames,
	                        std::vector<unsigned int> const& mappingSizes,
	                        std::vector<unsigned int> const& elements = {});
	static void updateVertices(GLHandler::Mesh& mesh, std::vector<float> const& vertices);
	static void setUpRender(ShaderProgram shader,
	                        QMatrix4x4 const& model = QMatrix4x4(),
	                        GeometricSpace space    = GeometricSpace::WORLD);
	static void render(GLHandler::Mesh const& mesh,
	                   PrimitiveType primitiveType = PrimitiveType::AUTO);
	static void deleteMesh(GLHandler::Mesh const& mesh);

	// textures
	static Texture newTexture(const char* texturePath, bool sRGB = true);
	static Texture newTexture(unsigned int width, unsigned int height,
	                          const GLvoid* data, bool sRGB = true);
	static void useTextures(std::vector<Texture> const& textures);
	static void deleteTexture(Texture const& texture);

	// http://entropymine.com/imageworsener/srgbformula/
	static QColor sRGBToLinear(QColor const& srgb);
	static QColor linearTosRGB(QColor const& linear);

  private:
	static GLuint loadShader(QString const& path, GLenum shaderType);

	static QOpenGLFunctions_4_0_Core glf;

	// object to screen transforms
	// transform for any world object
	static QMatrix4x4 fullTransform_;
	// transform for any Camera space object (follows Camera)
	static QMatrix4x4 fullCameraSpaceTransform_;
	// transform for any Tracked space object
	static QMatrix4x4 fullTrackedSpaceTransform_;
	// transform for any HMD space object (follows HMD)
	static QMatrix4x4 fullHmdSpaceTransform_;
};

Q_DECLARE_METATYPE(GLHandler::Mesh)
Q_DECLARE_METATYPE(GLHandler::RenderTarget)
Q_DECLARE_METATYPE(GLuint) // for typedefs Texture and ShaderProgram

#endif // GLHANDLER_H
