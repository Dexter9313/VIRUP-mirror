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

#include "utils.hpp"

class GLHandler
{
  public: // public types
	struct Mesh
	{
		GLuint vao;
		GLuint vbo;
		GLuint ebo;
		unsigned int vboSize;
		unsigned int eboSize;
	};

	struct RenderTarget
	{
		GLuint frameBuffer;
		GLuint texColorBuffer;
		GLuint renderBuffer;
		unsigned int width;
		unsigned int height;
	};

	struct Rect
	{
		int x0;
		int y0;
		int x1;
		int y1;
	};

	struct VertexMapping
	{
		const char* inputName;
		unsigned int inputSize;
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

	enum class GeometricSpace
	{
		WORLD,
		CAMERA,
		TRACKED,
		HMD
	};

  public:
	GLHandler() = delete;
	static bool init();
	static void setPointSize(unsigned int size);

	// rendering
	static RenderTarget newRenderTarget(unsigned int width,
	                                    unsigned int height);
	static RenderTarget newRenderTarget(unsigned int width, unsigned int height,
	                                    GLint format);
	static void deleteRenderTarget(RenderTarget const& renderTarget);
	static void beginRendering(RenderTarget const& renderTarget
	                           = {0, 0, 0,
	                              QSettings().value("window/width").toUInt(),
	                              QSettings().value("window/height").toUInt()});
	static void postProcess(ShaderProgram shader, RenderTarget const& from,
	                        RenderTarget const& to
	                        = {0, 0, 0,
	                           QSettings().value("window/width").toUInt(),
	                           QSettings().value("window/height").toUInt()});
	static void showOnScreen(RenderTarget const& renderTarget,
	                         Rect const& screenRect
	                         = {0, 0, QSettings().value("window/width").toInt(),
	                            QSettings().value("window/height").toInt()});
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
	static void setVertices(Mesh& mesh, std::vector<float> const& vertices,
	                        ShaderProgram const& shaderProgram,
	                        std::vector<VertexMapping> const& mapping,
	                        std::vector<unsigned int> const& elements = {});
	static void updateVertices(Mesh& mesh, std::vector<float> const& vertices);
	static void setUpRender(ShaderProgram shader,
	                        QMatrix4x4 const& model = QMatrix4x4(),
	                        GeometricSpace space    = GeometricSpace::WORLD);
	static void render(Mesh const& mesh,
	                   PrimitiveType primitiveType = PrimitiveType::AUTO);
	static void deleteMesh(Mesh const& mesh);

	// textures
	static Texture newTexture(const char* texturePath, bool sRGB = true);
	static Texture newTexture(unsigned int width, unsigned int height,
	                          const GLvoid* data, bool sRGB = true);
	static void useTextures(std::vector<Texture> const& textures);
	static void deleteTexture(Texture const& texture);

	// http://entropymine.com/imageworsener/srgbformula/
	static QColor sRGBToLinear(QColor const& srgb);
	QColor linearTosRGB(QColor const& linear);

	static GLint defaultRenderTargetFormat;

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

#endif // GLHANDLER_H
