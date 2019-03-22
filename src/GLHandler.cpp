#include "GLHandler.hpp"

QOpenGLFunctions_4_0_Core& GLHandler::glf()
{
	static QOpenGLFunctions_4_0_Core glf;
	return glf;
}

GLint& GLHandler::defaultRenderTargetFormat()
{
	static GLint defaultRenderTargetFormat = GL_RGBA8;
	return defaultRenderTargetFormat;
}

QMatrix4x4& GLHandler::fullTransform()
{
	static QMatrix4x4 fullTransform;
	return fullTransform;
}

QMatrix4x4& GLHandler::fullCameraSpaceTransform()
{
	static QMatrix4x4 fullCameraSpaceTransform;
	return fullCameraSpaceTransform;
}

QMatrix4x4& GLHandler::fullTrackedSpaceTransform()
{
	static QMatrix4x4 fullTrackedSpaceTransform;
	return fullTrackedSpaceTransform;
}

QMatrix4x4& GLHandler::fullHmdSpaceTransform()
{
	static QMatrix4x4 fullHmdSpaceTransform;
	return fullHmdSpaceTransform;
}

QMatrix4x4& GLHandler::fullSkyboxSpaceTransform()
{
	static QMatrix4x4 fullSkyboxSpaceTransform;
	return fullSkyboxSpaceTransform;
}

GLHandler::GLHandler()
{
	PythonQtHandler::addClass<Mesh>("Mesh");
	PythonQtHandler::addClass<Texture>("Texture");
	PythonQtHandler::addClass<RenderTarget>("RenderTarget");
	PythonQtHandler::addClass<ShaderProgram>("ShaderProgram");
}

bool GLHandler::init()
{
	glf().initializeOpenGLFunctions();

	// enable depth test
	glf().glEnable(GL_DEPTH_TEST);

	return true;
}

void GLHandler::setPointSize(unsigned int size)
{
	glf().glPointSize(size);
}

GLHandler::RenderTarget GLHandler::newRenderTarget(unsigned int width,
                                                   unsigned int height)
{
	return newRenderTarget(width, height, defaultRenderTargetFormat());
}

GLHandler::RenderTarget GLHandler::newRenderTarget(unsigned int width,
                                                   unsigned int height,
                                                   GLint format)
{
	RenderTarget result = {0, 0, 0, width, height};

	glf().glGenFramebuffers(1, &result.frameBuffer);
	glf().glBindFramebuffer(GL_FRAMEBUFFER, result.frameBuffer);

	// generate texture
	glf().glGenTextures(1, &result.texColorBuffer);
	glf().glBindTexture(GL_TEXTURE_2D, result.texColorBuffer);
	glf().glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA,
	                   GL_UNSIGNED_BYTE, nullptr);
	glf().glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glf().glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glf().glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glf().glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	                             GL_TEXTURE_2D, result.texColorBuffer, 0);

	// render buffer for depth and stencil
	glf().glGenRenderbuffers(1, &result.renderBuffer);
	glf().glBindRenderbuffer(GL_RENDERBUFFER, result.renderBuffer);
	glf().glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width,
	                            height);
	glf().glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach it to currently bound framebuffer object
	glf().glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
	                                GL_RENDERBUFFER, result.renderBuffer);

	return result;
}

GLHandler::Texture
    GLHandler::getColorAttachmentTexture(RenderTarget const& renderTarget)
{
	Texture tex   = {};
	tex.glTexture = renderTarget.texColorBuffer;
	tex.glTarget  = GL_TEXTURE_2D;
	return tex;
}

void GLHandler::deleteRenderTarget(RenderTarget const& renderTarget)
{
	glf().glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glf().glDeleteTextures(1, &renderTarget.texColorBuffer);
	glf().glDeleteBuffers(1, &renderTarget.renderBuffer);
	glf().glDeleteFramebuffers(1, &renderTarget.frameBuffer);
}

void GLHandler::beginRendering(RenderTarget const& renderTarget)
{
	glf().glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.frameBuffer);
	glf().glClear(static_cast<GLuint>(GL_COLOR_BUFFER_BIT)
	              | static_cast<GLuint>(GL_DEPTH_BUFFER_BIT));
	glf().glViewport(0, 0, renderTarget.width, renderTarget.height);
}

void GLHandler::postProcess(ShaderProgram shader, RenderTarget const& from,
                            RenderTarget const& to)
{
	Mesh quad(newMesh());
	setVertices(quad, {-1.f, -1.f, -1.f, 1.f, 1.f, -1.f, 1.f, 1.f}, shader,
	            {{"position", 2}});

	beginRendering(to);
	useShader(shader);
	useTextures({getColorAttachmentTexture(from)});
	render(quad, PrimitiveType::TRIANGLE_STRIP);

	deleteMesh(quad);
}

void GLHandler::showOnScreen(RenderTarget const& renderTarget, int screenx0,
                             int screeny0, int screenx1, int screeny1)
{
	glf().glBindFramebuffer(GL_READ_FRAMEBUFFER, renderTarget.frameBuffer);
	glf().glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glf().glBlitFramebuffer(0, 0, renderTarget.width, renderTarget.height,
	                        screenx0, screeny0, screenx1, screeny1,
	                        GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void GLHandler::beginTransparent()
{
	glf().glDepthMask(GL_FALSE);
	// enable transparency
	glf().glEnable(GL_BLEND);
	glf().glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

void GLHandler::endTransparent()
{
	glf().glDepthMask(GL_TRUE);
	glf().glDisable(GL_BLEND);
}

void GLHandler::clearDepthBuffer()
{
	glf().glClear(GL_DEPTH_BUFFER_BIT);
}

void GLHandler::setUpTransforms(QMatrix4x4 const& fullTransform,
                                QMatrix4x4 const& fullCameraSpaceTransform,
                                QMatrix4x4 const& fullTrackedSpaceTransform,
                                QMatrix4x4 const& fullHmdSpaceTransform,
                                QMatrix4x4 const& fullSkyboxSpaceTransform)
{
	GLHandler::fullTransform()             = fullTransform;
	GLHandler::fullCameraSpaceTransform()  = fullCameraSpaceTransform;
	GLHandler::fullTrackedSpaceTransform() = fullTrackedSpaceTransform;
	GLHandler::fullHmdSpaceTransform()     = fullHmdSpaceTransform;
	GLHandler::fullSkyboxSpaceTransform()  = fullSkyboxSpaceTransform;
}

GLHandler::ShaderProgram GLHandler::newShader(QString const& shadersCommonName)
{
	return newShader(shadersCommonName, shadersCommonName, shadersCommonName);
}

GLHandler::ShaderProgram GLHandler::newShader(QString vertexName,
                                              QString fragmentName,
                                              QString geometryName)
{
	// ignoring geometry shader for now
	(void) geometryName;

	if(!vertexName.contains('.'))
	{
		vertexName = "shaders/" + vertexName + ".vert";
	}
	if(!fragmentName.contains('.'))
	{
		fragmentName = "shaders/" + fragmentName + ".frag";
	}

	ShaderProgram result;

	// vertex shader
	GLuint vertexShader
	    = loadShader(getAbsoluteDataPath(vertexName), GL_VERTEX_SHADER);
	// fragment shader
	GLuint fragmentShader
	    = loadShader(getAbsoluteDataPath(fragmentName), GL_FRAGMENT_SHADER);

	// program
	result = glf().glCreateProgram();
	glf().glAttachShader(result, vertexShader);
	glf().glAttachShader(result, fragmentShader);
	glf().glDeleteShader(vertexShader);
	glf().glDeleteShader(fragmentShader);

	glf().glBindFragDataLocation(result, 0,
	                             "outColor"); // optional for one buffer
	glf().glLinkProgram(result);
	glf().glValidateProgram(result);

	return result;
}

void GLHandler::setShaderParam(ShaderProgram shader, const char* paramName,
                               float value)
{
	useShader(shader);
	glf().glUniform1f(glf().glGetUniformLocation(shader, paramName), value);
}

void GLHandler::setShaderParam(ShaderProgram shader, const char* paramName,
                               QVector2D const& value)
{
	useShader(shader);
	glf().glUniform2f(glf().glGetUniformLocation(shader, paramName), value.x(),
	                  value.y());
}

void GLHandler::setShaderParam(ShaderProgram shader, const char* paramName,
                               QVector3D const& value)
{
	useShader(shader);
	glf().glUniform3f(glf().glGetUniformLocation(shader, paramName), value.x(),
	                  value.y(), value.z());
}

void GLHandler::setShaderParam(ShaderProgram shader, const char* paramName,
                               QMatrix4x4 const& value)
{
	useShader(shader);
	glf().glUniformMatrix4fv(glf().glGetUniformLocation(shader, paramName), 1,
	                         GL_FALSE, value.data());
}

void GLHandler::setShaderParam(ShaderProgram shader, const char* paramName,
                               QColor const& value, bool sRGB)
{
	QColor linVal(sRGB ? sRGBToLinear(value) : value);
	setShaderParam(shader, paramName,
	               QVector3D(linVal.redF(), linVal.greenF(), linVal.blueF()));
}
void GLHandler::useShader(ShaderProgram shader)
{
	glf().glUseProgram(shader);
}

void GLHandler::deleteShader(ShaderProgram shader)
{
	glf().glUseProgram(0);

	glf().glDeleteProgram(shader);
}

GLuint GLHandler::loadShader(QString const& path, GLenum shaderType)
{
	QFile f(path);
	f.open(QFile::ReadOnly | QFile::Text);
	QTextStream in(&f);
	QByteArray bytes(in.readAll().toLocal8Bit());
	const char* source = bytes.data();

	GLuint shader = glf().glCreateShader(shaderType);
	glf().glShaderSource(shader, 1, &source, nullptr);
	glf().glCompileShader(shader);
	// checks
	GLint status;
	glf().glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	std::array<char, 512> buffer = {};
	glf().glGetShaderInfoLog(shader, 512, nullptr, &buffer[0]);
	if(status != GL_TRUE)
	{
		// NOLINTNEXTLINE(hicpp-no-array-decay)
		qWarning() << "SHADER ERROR : " << &buffer[0] << '\n';
	}

	return shader;
}

GLHandler::Mesh GLHandler::newMesh()
{
	Mesh mesh = {};
	glf().glGenVertexArrays(1, &mesh.vao);
	glf().glGenBuffers(1, &mesh.vbo);
	glf().glBindVertexArray(mesh.vao);
	glf().glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glf().glGenBuffers(1, &mesh.ebo);
	glf().glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
	glf().glBindVertexArray(0);

	mesh.vboSize = 0;
	mesh.eboSize = 0;

	return mesh;
}

void GLHandler::setVertices(
    Mesh& mesh, std::vector<float> const& vertices,
    ShaderProgram const& shaderProgram,
    std::vector<QPair<const char*, unsigned int>> const& mapping,
    std::vector<unsigned int> const& elements)
{
	glf().glBindVertexArray(mesh.vao);
	glf().glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	// put data in buffer (it is now sent to graphics card)
	glf().glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]),
	                   &(vertices[0]), GL_STATIC_DRAW);

	size_t offset = 0, stride = 0;
	for(auto map : mapping)
	{
		stride += map.second;
	}
	for(auto map : mapping)
	{
		// map position
		GLint posAttrib = glf().glGetAttribLocation(shaderProgram, map.first);
		if(posAttrib != -1)
		{
			glf().glEnableVertexAttribArray(posAttrib);
			glf().glVertexAttribPointer(
			    posAttrib, map.second, GL_FLOAT, GL_FALSE,
			    stride * sizeof(float),
			    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
			    reinterpret_cast<void*>(offset * sizeof(float)));
		}
		offset += map.second;
	}
	if(offset != 0)
	{
		mesh.vboSize = vertices.size() / offset;
	}
	if(!elements.empty())
	{
		glf().glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
		glf().glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		                   elements.size() * sizeof(elements[0]),
		                   &(elements[0]), GL_STATIC_DRAW);
		mesh.eboSize = elements.size();
	}

	glf().glBindVertexArray(0);
}

void GLHandler::setVertices(GLHandler::Mesh& mesh,
                            std::vector<float> const& vertices,
                            ShaderProgram const& shaderProgram,
                            QStringList const& mappingNames,
                            std::vector<unsigned int> const& mappingSizes,
                            std::vector<unsigned int> const& elements)
{
	std::vector<QPair<const char*, unsigned int>> mapping;
	for(unsigned int i(0); i < mappingSizes.size(); ++i)
	{
		mapping.emplace_back(mappingNames[i].toLatin1().constData(),
		                     mappingSizes[i]);
	}
	setVertices(mesh, vertices, shaderProgram, mapping, elements);
}

void GLHandler::updateVertices(Mesh& mesh, std::vector<float> const& vertices)
{
	glf().glBindVertexArray(mesh.vao);
	glf().glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	// put data in buffer (it is now sent to graphics card)
	glf().glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]),
	                   &(vertices[0]), GL_DYNAMIC_DRAW);
	glf().glBindVertexArray(0);
}

void GLHandler::setUpRender(ShaderProgram shader, QMatrix4x4 const& model,
                            GeometricSpace space)
{
	switch(space)
	{
		case GeometricSpace::WORLD:
			setShaderParam(shader, "camera", fullTransform() * model);
			break;
		case GeometricSpace::CAMERA:
			setShaderParam(shader, "camera",
			               fullCameraSpaceTransform() * model);
			break;
		case GeometricSpace::TRACKED:
			setShaderParam(shader, "camera",
			               fullTrackedSpaceTransform() * model);
			break;
		case GeometricSpace::HMD:
			setShaderParam(shader, "camera", fullHmdSpaceTransform() * model);
			break;
		case GeometricSpace::SKYBOX:
			setShaderParam(shader, "camera",
			               fullSkyboxSpaceTransform() * model);
			break;
		default:
			break;
	};
}

void GLHandler::render(Mesh const& mesh, PrimitiveType primitiveType)
{
	if(primitiveType == PrimitiveType::AUTO)
	{
		primitiveType = (mesh.eboSize == 0) ? PrimitiveType::POINTS
		                                    : PrimitiveType::TRIANGLES;
	}

	glf().glBindVertexArray(mesh.vao);
	if(mesh.eboSize == 0)
	{
		glf().glDrawArrays(static_cast<GLenum>(primitiveType), 0, mesh.vboSize);
	}
	else
	{
		glf().glDrawElements(static_cast<GLenum>(primitiveType), mesh.eboSize,
		                     GL_UNSIGNED_INT, nullptr);
	}
	glf().glBindVertexArray(0);
}

void GLHandler::deleteMesh(Mesh const& mesh)
{
	glf().glDeleteBuffers(1, &mesh.vbo);
	glf().glDeleteBuffers(1, &mesh.ebo);
	glf().glDeleteVertexArrays(1, &mesh.vao);
}

GLHandler::Texture GLHandler::newTexture(const char* texturePath, bool sRGB)
{
	Texture tex   = {};
	tex.glTexture = 0;
	tex.glTarget  = GL_TEXTURE_2D;

	QImage img_data;
	if(!img_data.load(texturePath))
	{
		// NOLINTNEXTLINE(hicpp-no-array-decay)
		qWarning() << "Could not load Texture \"" << texturePath << "\""
		           << '\n';
		return tex;
	}
	img_data = img_data.convertToFormat(QImage::Format_RGBA8888);

	glf().glGenTextures(1, &tex.glTexture);
	glf().glActiveTexture(GL_TEXTURE0);
	glf().glBindTexture(GL_TEXTURE_2D, tex.glTexture);
	glf().glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glf().glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glf().glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glf().glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glf().glTexImage2D(GL_TEXTURE_2D, 0, sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA,
	                   img_data.width(), img_data.height(), 0, GL_RGBA,
	                   GL_UNSIGNED_BYTE, img_data.bits());
	glf().glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

GLHandler::Texture GLHandler::newTexture(unsigned int width, const GLvoid* data,
                                         bool sRGB)
{
	Texture tex  = {};
	tex.glTarget = GL_TEXTURE_1D;
	glf().glGenTextures(1, &tex.glTexture);
	// glActiveTexture(GL_TEXTURE0);
	glf().glBindTexture(GL_TEXTURE_1D, tex.glTexture);
	glf().glTexImage1D(GL_TEXTURE_1D, 0, sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA,
	                   width, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	// glGenerateMipmap(GL_TEXTURE_2D);
	glf().glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glf().glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glf().glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	/*GLfloat fLargest;
	glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest );*/
	glf().glBindTexture(GL_TEXTURE_1D, 0);

	return tex;
}

GLHandler::Texture GLHandler::newTexture(unsigned int width,
                                         const unsigned char* red,
                                         const unsigned char* green,
                                         const unsigned char* blue,
                                         const unsigned char* alpha, bool sRGB)
{
	auto image = new GLubyte[width * 4];
	for(unsigned int i(0); i < width; ++i)
	{
		image[4 * i]     = red[i];
		image[4 * i + 1] = green[i];
		image[4 * i + 2] = blue[i];
		image[4 * i + 3] = alpha != nullptr ? alpha[i] : 255;
	}
	Texture tex = newTexture(width, image, sRGB);
	delete[] image;
	return tex;
}

GLHandler::Texture GLHandler::newTexture(unsigned int width,
                                         unsigned int height,
                                         const GLvoid* data, bool sRGB)
{
	Texture tex  = {};
	tex.glTarget = GL_TEXTURE_2D;
	glf().glGenTextures(1, &tex.glTexture);
	// glActiveTexture(GL_TEXTURE0);
	glf().glBindTexture(GL_TEXTURE_2D, tex.glTexture);
	glf().glTexImage2D(GL_TEXTURE_2D, 0, sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA,
	                   width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	// glGenerateMipmap(GL_TEXTURE_2D);
	glf().glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glf().glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glf().glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glf().glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	/*GLfloat fLargest;
	glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest );*/
	glf().glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

void GLHandler::useTextures(std::vector<Texture> const& textures)
{
	for(unsigned int i(0); i < textures.size(); ++i)
	{
		glf().glActiveTexture(i);
		glf().glBindTexture(textures[i].glTarget, textures[i].glTexture);
	}
}

void GLHandler::deleteTexture(Texture const& texture)
{
	glf().glDeleteTextures(1, &texture.glTexture);
}

QColor GLHandler::sRGBToLinear(QColor const& srgb)
{
	QColor lin(srgb);
	lin.setRedF(lin.redF() <= 0.04045
	                ? lin.redF() / 12.92
	                : qPow((lin.redF() + 0.055) / 1.055, 2.4));
	lin.setGreenF(lin.greenF() <= 0.04045
	                  ? lin.greenF() / 12.92
	                  : qPow((lin.greenF() + 0.055) / 1.055, 2.4));
	lin.setBlueF(lin.blueF() <= 0.04045
	                 ? lin.blueF() / 12.92
	                 : qPow((lin.blueF() + 0.055) / 1.055, 2.4));
	return lin;
}

QColor GLHandler::linearTosRGB(QColor const& linear)
{
	QColor srgb(linear);
	srgb.setRedF(srgb.redF() <= 0.0031308
	                 ? srgb.redF() * 12.92
	                 : (1.055 * qPow(srgb.redF(), 1.0 / 2.4)) - 0.055);
	srgb.setGreenF(srgb.greenF() <= 0.0031308
	                   ? srgb.greenF() * 12.92
	                   : (1.055 * qPow(srgb.greenF(), 1.0 / 2.4)) - 0.055);
	srgb.setBlueF(srgb.blueF() <= 0.0031308
	                  ? srgb.blueF() * 12.92
	                  : (1.055 * qPow(srgb.blueF(), 1.0 / 2.4)) - 0.055);
	return srgb;
}
