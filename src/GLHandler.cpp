#include "GLHandler.hpp"

QOpenGLFunctions_4_0_Core GLHandler::glf = QOpenGLFunctions_4_0_Core();

GLint GLHandler::defaultRenderTargetFormat = GL_RGBA8;

QMatrix4x4 GLHandler::fullTransform_             = QMatrix4x4();
QMatrix4x4 GLHandler::fullCameraSpaceTransform_  = QMatrix4x4();
QMatrix4x4 GLHandler::fullTrackedSpaceTransform_ = QMatrix4x4();
QMatrix4x4 GLHandler::fullHmdSpaceTransform_     = QMatrix4x4();

bool GLHandler::init()
{
	glf.initializeOpenGLFunctions();

	// enable depth test
	glf.glEnable(GL_DEPTH_TEST);

	return true;
}

void GLHandler::setPointSize(unsigned int size)
{
	glf.glPointSize(size);
}

GLHandler::RenderTarget GLHandler::newRenderTarget(unsigned int width,
                                                   unsigned int height)
{
	RenderTarget result;
	result.width  = width;
	result.height = height;

	glf.glGenFramebuffers(1, &result.frameBuffer);
	glf.glBindFramebuffer(GL_FRAMEBUFFER, result.frameBuffer);

	// generate texture
	glf.glGenTextures(1, &result.texColorBuffer);
	glf.glBindTexture(GL_TEXTURE_2D, result.texColorBuffer);
	glf.glTexImage2D(GL_TEXTURE_2D, 0, defaultRenderTargetFormat, width, height,
	                 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glf.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glf.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glf.glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glf.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	                           GL_TEXTURE_2D, result.texColorBuffer, 0);

	// render buffer for depth and stencil
	glf.glGenRenderbuffers(1, &result.renderBuffer);
	glf.glBindRenderbuffer(GL_RENDERBUFFER, result.renderBuffer);
	glf.glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width,
	                          height);
	glf.glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach it to currently bound framebuffer object
	glf.glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
	                              GL_RENDERBUFFER, result.renderBuffer);

	return result;
}

void GLHandler::deleteRenderTarget(RenderTarget const& renderTarget)
{
	glf.glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glf.glDeleteTextures(1, &renderTarget.texColorBuffer);
	glf.glDeleteBuffers(1, &renderTarget.renderBuffer);
	glf.glDeleteFramebuffers(1, &renderTarget.frameBuffer);
}

void GLHandler::beginRendering(RenderTarget const& renderTarget)
{
	glf.glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.frameBuffer);
	glf.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glf.glViewport(0, 0, renderTarget.width, renderTarget.height);
}

void GLHandler::postProcess(ShaderProgram shader, RenderTarget const& from,
                            RenderTarget const& to)
{
	Mesh quad(newMesh());
	setVertices(quad,
	            {-1.f, -1.f, -1.f, 1.f, 1.f, -1.f, 1.f, 1.f},
	            shader, {{"position", 2}});

	beginRendering(to);
	useShader(shader);
	useTextures({from.texColorBuffer});
	render(quad, PrimitiveType::TRIANGLE_STRIP);

	deleteMesh(quad);
}

void GLHandler::showOnScreen(RenderTarget const& renderTarget,
                             Rect const& screenRect)
{
	glf.glBindFramebuffer(GL_READ_FRAMEBUFFER, renderTarget.frameBuffer);
	glf.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glf.glBlitFramebuffer(0, 0, renderTarget.width, renderTarget.height,
	                      screenRect.x0, screenRect.y0, screenRect.x1,
	                      screenRect.y1, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void GLHandler::beginTransparent()
{
	glf.glDepthMask(GL_FALSE);
	// enable transparency
	glf.glEnable(GL_BLEND);
	glf.glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

void GLHandler::endTransparent()
{
	glf.glDepthMask(GL_TRUE);
	glf.glDisable(GL_BLEND);
}

void GLHandler::setUpTransforms(QMatrix4x4 const& fullTransform,
                                QMatrix4x4 const& fullCameraSpaceTransform,
                                QMatrix4x4 const& fullTrackedSpaceTransform,
                                QMatrix4x4 const& fullHmdSpaceTransform)
{
	fullTransform_             = fullTransform;
	fullCameraSpaceTransform_  = fullCameraSpaceTransform;
	fullTrackedSpaceTransform_ = fullTrackedSpaceTransform;
	fullHmdSpaceTransform_     = fullHmdSpaceTransform;
}

GLHandler::ShaderProgram GLHandler::newShader(QString const& shadersCommonName)
{
	return newShader(shadersCommonName, shadersCommonName, shadersCommonName);
}

GLHandler::ShaderProgram GLHandler::newShader(QString vertex, QString fragment,
                                              QString geometry)
{
	// ignoring geometry shader for now
	(void) geometry;

	if(!vertex.contains('.'))
		vertex = "data/shaders/" + vertex + ".vert";
	if(!fragment.contains('.'))
		fragment = "data/shaders/" + fragment + ".frag";

	ShaderProgram result;

	// vertex shader
	GLuint vertexShader = loadShader(vertex, GL_VERTEX_SHADER);
	// fragment shader
	GLuint fragmentShader = loadShader(fragment, GL_FRAGMENT_SHADER);

	// program
	result = glf.glCreateProgram();
	glf.glAttachShader(result, vertexShader);
	glf.glAttachShader(result, fragmentShader);
	glf.glDeleteShader(vertexShader);
	glf.glDeleteShader(fragmentShader);

	glf.glBindFragDataLocation(result, 0,
	                           "outColor"); // optional for one buffer
	glf.glLinkProgram(result);
	glf.glValidateProgram(result);

	return result;
}

void GLHandler::setShaderParam(ShaderProgram shader, const char* paramName,
                               float value)
{
	useShader(shader);
	glf.glUniform1f(glf.glGetUniformLocation(shader, paramName), value);
}

void GLHandler::setShaderParam(ShaderProgram shader, const char* paramName,
                               QVector3D const& value)
{
	useShader(shader);
	glf.glUniform3f(glf.glGetUniformLocation(shader, paramName), value.x(),
	                value.y(), value.z());
}

void GLHandler::setShaderParam(ShaderProgram shader, const char* paramName,
                               QMatrix4x4 const& value)
{
	useShader(shader);
	glf.glUniformMatrix4fv(glf.glGetUniformLocation(shader, paramName), 1,
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
	glf.glUseProgram(shader);
}

void GLHandler::deleteShader(ShaderProgram shader)
{
	glf.glUseProgram(0);

	glf.glDeleteProgram(shader);
}

GLuint GLHandler::loadShader(QString const& path, GLenum shaderType)
{
	QFile f(path);
	f.open(QFile::ReadOnly | QFile::Text);
	QTextStream in(&f);
	QByteArray bytes(in.readAll().toLocal8Bit());
	const char* source = bytes.data();

	GLuint shader = glf.glCreateShader(shaderType);
	glf.glShaderSource(shader, 1, &source, NULL);
	glf.glCompileShader(shader);
	// checks
	GLint status;
	glf.glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	char buffer[512];
	glf.glGetShaderInfoLog(shader, 512, NULL, buffer);
	if(status != GL_TRUE)
		qWarning() << "SHADER ERROR : " << buffer << '\n';

	return shader;
}

GLHandler::Mesh GLHandler::newMesh()
{
	Mesh mesh;
	glf.glGenVertexArrays(1, &mesh.vao);
	glf.glGenBuffers(1, &mesh.vbo);
	glf.glBindVertexArray(mesh.vao);
	glf.glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glf.glGenBuffers(1, &mesh.ebo);
	glf.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
	glf.glBindVertexArray(0);

	mesh.vboSize = 0;
	mesh.eboSize = 0;

	return mesh;
}

void GLHandler::setVertices(Mesh& mesh, std::vector<float> const& vertices,
                            ShaderProgram const& shaderProgram,
                            std::vector<VertexMapping> const& mapping,
                            std::vector<unsigned int> const& elements)
{
	glf.glBindVertexArray(mesh.vao);
	glf.glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	// put data in buffer (it is now sent to graphics card)
	glf.glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]),
	                 &(vertices[0]), GL_STATIC_DRAW);

	size_t offset = 0, stride = 0;
	for(unsigned int i(0); i < mapping.size(); ++i)
	{
		stride += mapping[i].inputSize;
	}
	for(unsigned int i(0); i < mapping.size(); ++i)
	{
		// map position
		GLint posAttrib
		    = glf.glGetAttribLocation(shaderProgram, mapping[i].inputName);
		if(posAttrib != -1)
		{
			glf.glEnableVertexAttribArray(posAttrib);
			glf.glVertexAttribPointer(posAttrib, mapping[i].inputSize, GL_FLOAT,
			                          GL_FALSE, stride * sizeof(float),
			                          (void*) (offset * sizeof(float)));
		}
		offset += mapping[i].inputSize;
	}
	mesh.vboSize = vertices.size() / offset;
	if(elements.size() != 0)
	{
		glf.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
		glf.glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		                 elements.size() * sizeof(elements[0]), &(elements[0]),
		                 GL_STATIC_DRAW);
		mesh.eboSize = elements.size();
	}

	glf.glBindVertexArray(0);
}

void GLHandler::updateVertices(Mesh& mesh, std::vector<float> const& vertices)
{
	glf.glBindVertexArray(mesh.vao);
	glf.glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	// put data in buffer (it is now sent to graphics card)
	glf.glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]),
	                 &(vertices[0]), GL_DYNAMIC_DRAW);
	glf.glBindVertexArray(0);
}

void GLHandler::setUpRender(ShaderProgram shader, QMatrix4x4 const& model,
                            GeometricSpace space)
{
	switch(space)
	{
		case GeometricSpace::WORLD:
			setShaderParam(shader, "camera", fullTransform_ * model);
			break;
		case GeometricSpace::CAMERA:
			setShaderParam(shader, "camera", fullCameraSpaceTransform_ * model);
			break;
		case GeometricSpace::TRACKED:
			setShaderParam(shader, "camera",
			               fullTrackedSpaceTransform_ * model);
			break;
		case GeometricSpace::HMD:
			setShaderParam(shader, "camera", fullHmdSpaceTransform_ * model);
			break;
		default:
			break;
	};
}

void GLHandler::render(Mesh const& mesh, PrimitiveType primitiveType)
{
	if(primitiveType == PrimitiveType::AUTO)
		primitiveType = (mesh.eboSize == 0) ? PrimitiveType::POINTS
		                                    : PrimitiveType::TRIANGLES;

	glf.glBindVertexArray(mesh.vao);
	if(mesh.eboSize == 0)
		glf.glDrawArrays(static_cast<GLenum>(primitiveType), 0, mesh.vboSize);
	else
		glf.glDrawElements(static_cast<GLenum>(primitiveType), mesh.eboSize,
		                   GL_UNSIGNED_INT, 0);
	glf.glBindVertexArray(0);
}

void GLHandler::deleteMesh(Mesh const& mesh)
{
	glf.glDeleteBuffers(1, &mesh.vbo);
	glf.glDeleteBuffers(1, &mesh.ebo);
	glf.glDeleteVertexArrays(1, &mesh.vao);
}

GLHandler::Texture GLHandler::newTexture(const char* texturePath, bool sRGB)
{
	QImage img_data;
	if(!img_data.load(texturePath))
	{
		qWarning() << "Could not load Texture \"" << texturePath << "\""
		           << '\n';
		return 0;
	}
	img_data = img_data.convertToFormat(QImage::Format_RGBA8888);

	Texture tex;
	glf.glGenTextures(1, &tex);
	glf.glActiveTexture(GL_TEXTURE0);
	glf.glBindTexture(GL_TEXTURE_2D, tex);
	glf.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glf.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glf.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glf.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glf.glTexImage2D(GL_TEXTURE_2D, 0, sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA, img_data.width(),
	                 img_data.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
	                 img_data.bits());
	glf.glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

GLHandler::Texture GLHandler::newTexture(unsigned int width,
                                         unsigned int height,
                                         const GLvoid* data,
                                         bool sRGB)
{
	Texture tex;
	glf.glGenTextures(1, &tex);
	// glActiveTexture(GL_TEXTURE0);
	glf.glBindTexture(GL_TEXTURE_2D, tex);
	glf.glTexImage2D(GL_TEXTURE_2D, 0, sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA, width, height, 0, GL_RGBA,
	                 GL_UNSIGNED_BYTE, data);
	// glGenerateMipmap(GL_TEXTURE_2D);
	glf.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glf.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glf.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glf.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	/*GLfloat fLargest;
	glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest );*/
	glf.glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

void GLHandler::useTextures(std::vector<Texture> const& textures)
{
	for(unsigned int i(0); i < textures.size(); ++i)
	{
		glf.glActiveTexture(i);
		glf.glBindTexture(GL_TEXTURE_2D, textures[i]);
	}
}

void GLHandler::deleteTexture(Texture const& texture)
{
	glf.glDeleteTextures(1, &texture);
}

QColor GLHandler::sRGBToLinear(QColor const& srgb)
{
	QColor lin(srgb);
	lin.setRedF(lin.redF() <= 0.04045 ? lin.redF() / 12.92 : qPow((lin.redF() + 0.055)/1.055, 2.4));
	lin.setGreenF(lin.greenF() <= 0.04045 ? lin.greenF() / 12.92 : qPow((lin.greenF() + 0.055)/1.055, 2.4));
	lin.setBlueF(lin.blueF() <= 0.04045 ? lin.blueF() / 12.92 : qPow((lin.blueF() + 0.055)/1.055, 2.4));
	return lin;
}

QColor GLHandler::linearTosRGB(QColor const& linear)
{
	QColor srgb(linear);
	srgb.setRedF(srgb.redF() <= 0.0031308 ? srgb.redF() * 12.92 : (1.055 * qPow(srgb.redF(), 1.0/2.4)) - 0.055);
	srgb.setGreenF(srgb.greenF() <= 0.0031308 ? srgb.greenF() * 12.92 : (1.055 * qPow(srgb.greenF(), 1.0/2.4)) - 0.055);
	srgb.setBlueF(srgb.blueF() <= 0.0031308 ? srgb.blueF() * 12.92 : (1.055 * qPow(srgb.blueF(), 1.0/2.4)) - 0.055);
	return srgb;
}

