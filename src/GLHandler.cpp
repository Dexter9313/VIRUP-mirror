#include "GLHandler.hpp"

unsigned int& GLHandler::renderTargetCount()
{
	static unsigned int renderTargetCount = 0;
	return renderTargetCount;
}

unsigned int& GLHandler::shaderCount()
{
	static unsigned int shaderCount = 0;
	return shaderCount;
}

unsigned int& GLHandler::meshCount()
{
	static unsigned int meshCount = 0;
	return meshCount;
}

unsigned int& GLHandler::texCount()
{
	static unsigned int texCount = 0;
	return texCount;
}

unsigned int& GLHandler::PBOCount()
{
	static unsigned int PBOCount = 0;
	return PBOCount;
}

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

QMatrix4x4& GLHandler::fullSeatedTrackedSpaceTransform()
{
	static QMatrix4x4 fullSeatedTrackedSpaceTransform;
	return fullSeatedTrackedSpaceTransform;
}

QMatrix4x4& GLHandler::fullStandingTrackedSpaceTransform()
{
	static QMatrix4x4 fullStandingTrackedSpaceTransform;
	return fullStandingTrackedSpaceTransform;
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

	// enable backface culling for optimization
	setBackfaceCulling(true);

	return true;
}

void GLHandler::setPointSize(unsigned int size)
{
	glf().glPointSize(size);
}

GLHandler::RenderTarget GLHandler::newRenderTarget(unsigned int width,
                                                   unsigned int height,
                                                   bool cubemap)
{
	return newRenderTarget(width, height, defaultRenderTargetFormat(), cubemap);
}

GLHandler::RenderTarget GLHandler::newRenderTarget(unsigned int width,
                                                   unsigned int height,
                                                   GLint format, bool cubemap)
{
	++renderTargetCount();
	RenderTarget result = {width, height};

	glf().glGenFramebuffers(1, &result.frameBuffer);
	glf().glBindFramebuffer(GL_FRAMEBUFFER, result.frameBuffer);

	// generate texture
	if(!cubemap)
	{
		result.texColorBuffer
		    = newTexture2D(width, height, nullptr, format, GL_RGBA,
		                   GL_TEXTURE_2D, GL_LINEAR, GL_MIRRORED_REPEAT);
	}
	else
	{
		result.texColorBuffer = newTextureCubemap(
		    width, {{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
		    format, GL_RGBA, GL_TEXTURE_CUBE_MAP, GL_LINEAR,
		    GL_MIRRORED_REPEAT);
	}

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

GLHandler::RenderTarget GLHandler::newDepthMap(unsigned int width,
                                               unsigned int height,
                                               bool /*cubemap*/)
{
	++renderTargetCount();
	RenderTarget result = {width, height};
	result.isDepthMap   = true;

	glf().glGenFramebuffers(1, &result.frameBuffer);

	result.texColorBuffer = newTexture2D(
	    width, height, nullptr, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT,
	    GL_TEXTURE_2D, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_FLOAT);
	// add depth specific texture parameters for sampler2DShadow
	glf().glBindTexture(GL_TEXTURE_2D, result.texColorBuffer.glTexture);
	glf().glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
	                      GL_COMPARE_REF_TO_TEXTURE);
	glf().glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glf().glBindFramebuffer(GL_FRAMEBUFFER, result.frameBuffer);
	glf().glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
	                             GL_TEXTURE_2D, result.texColorBuffer.glTexture,
	                             0);
	glf().glDrawBuffer(GL_NONE);
	glf().glReadBuffer(GL_NONE);
	glf().glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return result;
}

GLHandler::Texture
    GLHandler::getColorAttachmentTexture(RenderTarget const& renderTarget)
{
	return renderTarget.texColorBuffer;
}

void GLHandler::blitColorBuffer(RenderTarget const& from,
                                RenderTarget const& to)
{
	GLHandler::glf().glBindFramebuffer(GL_READ_FRAMEBUFFER, from.frameBuffer);
	GLHandler::glf().glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to.frameBuffer);
	GLHandler::glf().glBlitFramebuffer(0, 0, from.width, from.height, 0, 0,
	                                   to.width, to.height, GL_COLOR_BUFFER_BIT,
	                                   GL_LINEAR);
}

void GLHandler::blitDepthBuffer(RenderTarget const& from,
                                RenderTarget const& to)
{
	GLHandler::glf().glBindFramebuffer(GL_READ_FRAMEBUFFER, from.frameBuffer);
	GLHandler::glf().glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to.frameBuffer);
	GLHandler::glf().glBlitFramebuffer(0, 0, from.width, from.height, 0, 0,
	                                   to.width, to.height, GL_DEPTH_BUFFER_BIT,
	                                   GL_NEAREST);
}

void GLHandler::deleteRenderTarget(RenderTarget const& renderTarget)
{
	--renderTargetCount();
	glf().glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLHandler::deleteTexture(renderTarget.texColorBuffer);
	glf().glDeleteRenderbuffers(1, &renderTarget.renderBuffer);
	glf().glDeleteFramebuffers(1, &renderTarget.frameBuffer);
}

void GLHandler::setClearColor(QColor const& color)
{
	glf().glClearColor(color.redF(), color.greenF(), color.blueF(),
	                   color.alphaF());
}

void GLHandler::beginRendering(RenderTarget const& renderTarget, CubeFace face)
{
	beginRendering(static_cast<GLuint>(GL_COLOR_BUFFER_BIT)
	                   | static_cast<GLuint>(GL_DEPTH_BUFFER_BIT),
	               renderTarget, face);
}

void GLHandler::beginRendering(GLbitfield clearMask,
                               RenderTarget const& renderTarget, CubeFace face)
{
	glf().glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.frameBuffer);
	if(renderTarget.frameBuffer != 0 && !renderTarget.isDepthMap)
	{
		if(renderTarget.texColorBuffer.glTarget == GL_TEXTURE_CUBE_MAP)
		{
			glf().glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			                             static_cast<unsigned int>(face)
			                                 + GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			                             renderTarget.texColorBuffer.glTexture,
			                             0);
		}
		else
		{
			glf().glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			                             renderTarget.texColorBuffer.glTarget,
			                             renderTarget.texColorBuffer.glTexture,
			                             0);
		}
	}
	glf().glClear(clearMask);
	glf().glViewport(0, 0, renderTarget.width, renderTarget.height);
}

void GLHandler::postProcess(ShaderProgram shader, RenderTarget const& from,
                            RenderTarget const& to,
                            std::vector<Texture> const& uniformTextures)
{
	Mesh quad(newMesh());
	setVertices(quad, {-1.f, -1.f, 1.f, -1.f, -1.f, 1.f, 1.f, 1.f}, shader,
	            {{"position", 2}});

	beginRendering(to);
	useShader(shader);
	std::vector<Texture> texs;
	texs.push_back(getColorAttachmentTexture(from));
	// TODO(florian) performance
	for(auto tex : uniformTextures)
	{
		texs.push_back(tex);
	}
	useTextures(texs);
	setBackfaceCulling(false);
	render(quad, PrimitiveType::TRIANGLE_STRIP);
	setBackfaceCulling(true);

	deleteMesh(quad);
}

void GLHandler::generateEnvironmentMap(
    GLHandler::RenderTarget const& renderTarget,
    std::function<void()> const& renderFunction, QVector3D const& position)
{
	QMatrix4x4 translation;
	translation.translate(-1.f * position);
	QMatrix4x4 perspective;
	perspective.perspective(90.f, 1.f, 0.1f, 10.f);

	std::vector<QVector3D> vecs = {
	    QVector3D(1, 0, 0),  QVector3D(0, -1, 0), QVector3D(-1, 0, 0),
	    QVector3D(0, -1, 0), QVector3D(0, 1, 0),  QVector3D(0, 0, 1),
	    QVector3D(0, -1, 0), QVector3D(0, 0, -1), QVector3D(0, 0, -1),
	    QVector3D(0, -1, 0), QVector3D(0, 0, 1),  QVector3D(0, -1, 0),
	};

	std::vector<GLHandler::CubeFace> faces = {
	    GLHandler::CubeFace::FRONT,  GLHandler::CubeFace::BACK,
	    GLHandler::CubeFace::LEFT,   GLHandler::CubeFace::RIGHT,
	    GLHandler::CubeFace::BOTTOM, GLHandler::CubeFace::TOP,
	};

	for(unsigned int i(0); i < 6; ++i)
	{
		QMatrix4x4 cubeCamera;
		cubeCamera.lookAt(QVector3D(0, 0, 0), vecs[2 * i], vecs[(2 * i) + 1]);
		QMatrix4x4 c = perspective * cubeCamera * translation;
		GLHandler::setUpTransforms(c, c, c, c, c, c);
		GLHandler::beginRendering(renderTarget, faces[i]);
		renderFunction();
	}
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

QImage GLHandler::generateScreenshot(RenderTarget const& renderTarget)
{
	auto data(new uchar[renderTarget.width * renderTarget.height * 4]);

	glf().glBindFramebuffer(GL_READ_FRAMEBUFFER, renderTarget.frameBuffer);
	glf().glReadPixels(0, 0, renderTarget.width, renderTarget.height, GL_RGBA,
	                   GL_UNSIGNED_BYTE, static_cast<GLvoid*>(data));

	return QImage(data, renderTarget.width, renderTarget.height,
	              renderTarget.width * 4, QImage::Format::Format_RGBA8888,
	              [](void* data) { delete static_cast<uchar*>(data); }, data);
}

void GLHandler::beginWireframe()
{
	GLHandler::glf().glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void GLHandler::endWireframe()
{
	GLHandler::glf().glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void GLHandler::beginTransparent(GLenum blendfuncSfactor,
                                 GLenum blendfuncDfactor)
{
	glf().glDepthMask(GL_FALSE);
	// enable transparency
	glf().glEnable(GL_BLEND);
	glf().glBlendFunc(blendfuncSfactor, blendfuncDfactor);
}

void GLHandler::endTransparent()
{
	glf().glDepthMask(GL_TRUE);
	glf().glDisable(GL_BLEND);
}

void GLHandler::setBackfaceCulling(bool on, GLenum faceToCull,
                                   GLenum frontFaceWindingOrder)
{
	if(on)
	{
		glf().glEnable(GL_CULL_FACE);
		glf().glCullFace(faceToCull);
		glf().glFrontFace(frontFaceWindingOrder);
	}
	else
	{
		glf().glDisable(GL_CULL_FACE);
	}
}

void GLHandler::clearDepthBuffer()
{
	glf().glClear(GL_DEPTH_BUFFER_BIT);
}

void GLHandler::setUpTransforms(
    QMatrix4x4 const& fullTransform, QMatrix4x4 const& fullCameraSpaceTransform,
    QMatrix4x4 const& fullSeatedTrackedSpaceTransform,
    QMatrix4x4 const& fullStandingTrackedSpaceTransform,
    QMatrix4x4 const& fullHmdSpaceTransform,
    QMatrix4x4 const& fullSkyboxSpaceTransform)
{
	GLHandler::fullTransform()            = fullTransform;
	GLHandler::fullCameraSpaceTransform() = fullCameraSpaceTransform;
	GLHandler::fullSeatedTrackedSpaceTransform()
	    = fullSeatedTrackedSpaceTransform;
	GLHandler::fullStandingTrackedSpaceTransform()
	    = fullStandingTrackedSpaceTransform;
	GLHandler::fullHmdSpaceTransform()    = fullHmdSpaceTransform;
	GLHandler::fullSkyboxSpaceTransform() = fullSkyboxSpaceTransform;
}

GLHandler::ShaderProgram
    GLHandler::newShader(QString const& shadersCommonName,
                         QMap<QString, QString> const& defines)
{
	return newShader(shadersCommonName, shadersCommonName, defines,
	                 shadersCommonName);
}

GLHandler::ShaderProgram
    GLHandler::newShader(QString vertexName, QString fragmentName,
                         QMap<QString, QString> const& defines,
                         QString geometryName)
{
	++shaderCount();
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
	GLuint vertexShader = loadShader(vertexName, GL_VERTEX_SHADER, defines);
	// fragment shader
	GLuint fragmentShader
	    = loadShader(fragmentName, GL_FRAGMENT_SHADER, defines);

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

void GLHandler::setShaderUnusedAttributesValues(
    ShaderProgram shader,
    std::vector<QPair<const char*, std::vector<float>>> const& defaultValues)
{
	for(auto attribute : defaultValues)
	{
		GLint posAttrib = glf().glGetAttribLocation(shader, attribute.first);
		if(posAttrib != -1)
		{
			glf().glDisableVertexAttribArray(posAttrib);
			// special case where we have to do it, see :
			// https://bugreports.qt.io/browse/QTBUG-40090?jql=text%20~%20%22glvertexattrib%22
			QOpenGLFunctions glf_base;
			glf_base.initializeOpenGLFunctions();
			switch(attribute.second.size())
			{
				case 1:
					glf_base.glVertexAttrib1fv(posAttrib, &attribute.second[0]);
					break;
				case 2:
					glf_base.glVertexAttrib2fv(posAttrib, &attribute.second[0]);
					break;
				case 3:
					glf_base.glVertexAttrib3fv(posAttrib, &attribute.second[0]);
					break;
				case 4:
					glf_base.glVertexAttrib4fv(posAttrib, &attribute.second[0]);
					break;
				default:
					break;
			}
		}
	}
}

void GLHandler::setShaderUnusedAttributesValues(
    ShaderProgram shader, QStringList const& names,
    std::vector<std::vector<float>> const& values)
{
	std::vector<QPair<const char*, std::vector<float>>> defaultValues;
	for(unsigned int i(0); i < values.size(); ++i)
	{
		defaultValues.emplace_back(names[i].toLatin1().constData(), values[i]);
	}
	setShaderUnusedAttributesValues(shader, defaultValues);
}

void GLHandler::setShaderParam(ShaderProgram shader, const char* paramName,
                               int value)
{
	useShader(shader);
	glf().glUniform1i(glf().glGetUniformLocation(shader, paramName), value);
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
                               unsigned int size, QVector3D const* values)
{
	useShader(shader);
	auto data = new GLfloat[3 * size];
	for(unsigned int i(0); i < size; ++i)
	{
		for(unsigned int j(0); j < 3; ++j)
		{
			data[i * 3 + j] = values[i][j];
		}
	}
	glf().glUniform3fv(glf().glGetUniformLocation(shader, paramName), size,
	                   &(data[0]));
	delete[] data;
}

void GLHandler::setShaderParam(ShaderProgram shader, const char* paramName,
                               QVector4D const& value)
{
	useShader(shader);
	glf().glUniform4f(glf().glGetUniformLocation(shader, paramName), value.x(),
	                  value.y(), value.z(), value.w());
}

void GLHandler::setShaderParam(ShaderProgram shader, const char* paramName,
                               unsigned int size, QVector4D const* values)
{
	useShader(shader);
	auto data = new GLfloat[4 * size];
	for(unsigned int i(0); i < size; ++i)
	{
		for(unsigned int j(0); j < 4; ++j)
		{
			data[i * 4 + j] = values[i][j];
		}
	}
	glf().glUniform4fv(glf().glGetUniformLocation(shader, paramName), size,
	                   &(data[0]));
	delete[] data;
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
	--shaderCount();
	glf().glUseProgram(0);

	glf().glDeleteProgram(shader);
}

QString
    GLHandler::getFullPreprocessedSource(QString const& path,
                                         QMap<QString, QString> const& defines)
{
	// Read source
	QFile f(getAbsoluteDataPath(path));
	if(!f.exists())
	{
		f.setFileName(getAbsoluteDataPath("shaders/" + path));
	}
	f.open(QFile::ReadOnly | QFile::Text);
	QTextStream in(&f);
	QString source(in.readAll().toLocal8Bit());

	// Strip comments
	// One-liners
	int commentPos(source.indexOf("//"));
	while(commentPos != -1)
	{
		int endOfComment(source.indexOf('\n', commentPos));
		source.replace(commentPos, endOfComment - commentPos, "");
		commentPos = source.indexOf("//", commentPos);
	}
	// Blocks
	commentPos = source.indexOf("/*");
	while(commentPos != -1)
	{
		int endOfComment(source.indexOf("*/", commentPos));
		source.replace(commentPos, endOfComment - commentPos + 2, "");
		commentPos = source.indexOf("/*", commentPos);
	}

	// include other preprocessed sources within source
	int includePos(source.indexOf("#include"));
	while(includePos != -1)
	{
		int beginPath(source.indexOf('<', includePos));
		int endPath(source.indexOf('>', includePos));
		int endOfLine(source.indexOf('\n', includePos));

		QString includedSrc(getFullPreprocessedSource(
		    source.mid(beginPath + 1, endPath - beginPath - 1), defines));
		source.replace(includePos, endOfLine - includePos, includedSrc);

		includePos = source.indexOf("#include", includePos);
	}

	// add defines after #version
	int definesInsertPoint(source.indexOf("#version"));
	definesInsertPoint = source.indexOf('\n', definesInsertPoint) + 1;
	for(auto const& key : defines.keys())
	{
		source.insert(definesInsertPoint, QString("#define ") + key + " "
		                                      + defines.value(key) + "\n");
	}

	return source;
}

GLuint GLHandler::loadShader(QString const& path, GLenum shaderType,
                             QMap<QString, QString> const& defines)
{
	QString source(getFullPreprocessedSource(path, defines));
	QByteArray ba     = source.toLatin1();
	const char* bytes = ba.data();

	GLuint shader = glf().glCreateShader(shaderType);
	glf().glShaderSource(shader, 1, &bytes, nullptr);
	glf().glCompileShader(shader);
	// checks
	GLint status;
	glf().glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	std::array<char, 512> buffer = {};
	glf().glGetShaderInfoLog(shader, 512, nullptr, &buffer[0]);
	if(status != GL_TRUE)
	{
		// NOLINTNEXTLINE(hicpp-no-array-decay)
		qWarning() << "SHADER ERROR (" << path << "-" << shader
		           << ") :" << &buffer[0] << '\n';
	}

	return shader;
}

GLHandler::Mesh GLHandler::newMesh()
{
	++meshCount();
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
    Mesh& mesh, float const* vertices, size_t size,
    ShaderProgram const& shaderProgram,
    std::vector<QPair<const char*, unsigned int>> const& mapping,
    std::vector<unsigned int> const& elements)
{
	glf().glBindVertexArray(mesh.vao);
	glf().glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	// put data in buffer (it is now sent to graphics card)
	glf().glBufferData(GL_ARRAY_BUFFER, size * sizeof(vertices[0]), vertices,
	                   GL_STATIC_DRAW);

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
		mesh.vboSize = size / offset;
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

void GLHandler::setVertices(
    Mesh& mesh, std::vector<float> const& vertices,
    ShaderProgram const& shaderProgram,
    std::vector<QPair<const char*, unsigned int>> const& mapping,
    std::vector<unsigned int> const& elements)
{
	setVertices(mesh, &(vertices[0]), vertices.size(), shaderProgram, mapping,
	            elements);
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

void GLHandler::updateVertices(GLHandler::Mesh& mesh, float const* vertices,
                               size_t size)
{
	glf().glBindVertexArray(mesh.vao);
	glf().glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	// put data in buffer (it is now sent to graphics card)
	glf().glBufferData(GL_ARRAY_BUFFER, size * sizeof(vertices[0]), vertices,
	                   GL_DYNAMIC_DRAW);
	glf().glBindVertexArray(0);
}

void GLHandler::updateVertices(Mesh& mesh, std::vector<float> const& vertices)
{
	updateVertices(mesh, &(vertices[0]), vertices.size());
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
		case GeometricSpace::SEATEDTRACKED:
			setShaderParam(shader, "camera",
			               fullSeatedTrackedSpaceTransform() * model);
			break;
		case GeometricSpace::STANDINGTRACKED:
			setShaderParam(shader, "camera",
			               fullStandingTrackedSpaceTransform() * model);
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
	--meshCount();
	glf().glDeleteBuffers(1, &mesh.vbo);
	glf().glDeleteBuffers(1, &mesh.ebo);
	glf().glDeleteVertexArrays(1, &mesh.vao);
}

GLHandler::Texture GLHandler::newTexture(unsigned int width, const GLvoid* data,
                                         bool sRGB)
{
	return newTexture1D(width, data, sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA);
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

GLHandler::Texture GLHandler::newTexture(const char* texturePath, bool sRGB)
{
	QImage img_data;
	if(!img_data.load(texturePath))
	{
		// NOLINTNEXTLINE(hicpp-no-array-decay)
		qWarning() << "Could not load Texture \"" << texturePath << "\""
		           << '\n';
		return {};
	}
	return newTexture(img_data, sRGB);
}

GLHandler::Texture GLHandler::newTexture(QImage const& image, bool sRGB)
{
	QImage img_data = image.convertToFormat(QImage::Format_RGBA8888);
	return newTexture2D(img_data.width(), img_data.height(), img_data.bits(),
	                    sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA);
}

GLHandler::Texture GLHandler::newTexture(unsigned int width,
                                         unsigned int height,
                                         const GLvoid* data, bool sRGB)
{
	return newTexture2D(width, height, data, sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA);
}

GLHandler::Texture
    GLHandler::newTexture(std::array<const char*, 6> const& texturesPaths,
                          bool sRGB)
{
	std::array<QImage, 6> images;
	for(unsigned int i(0); i < 6; ++i)
	{
		if(!images.at(i).load(texturesPaths.at(i)))
		{
			// NOLINTNEXTLINE(hicpp-no-array-decay)
			qWarning() << "Could not load Texture \"" << texturesPaths.at(i)
			           << "\"" << '\n';
			return {};
		}
	}

	return newTexture(images, sRGB);
}

GLHandler::Texture GLHandler::newTexture(std::array<QImage, 6> const& images,
                                         bool sRGB)
{
	std::array<GLvoid const*, 6> data = {};

	std::array<QImage, 6> img_data = {};

	for(unsigned int i(0); i < 6; ++i)
	{
		switch(i)
		{
			case static_cast<int>(CubeFace::FRONT):
			case static_cast<int>(CubeFace::TOP):
			case static_cast<int>(CubeFace::BOTTOM):
			{
				QImage im(images.at(i).height(), images.at(i).width(),
				          QImage::Format_RGBA8888);
				for(int j(0); j < im.height(); ++j)
				{
					for(int k(0); k < im.width(); ++k)
					{
						im.setPixel(k, j, images.at(i).pixel(j, k));
					}
				}
				img_data.at(i) = im;
			}
			break;
			case static_cast<int>(CubeFace::BACK):
			{
				QImage im(images.at(i).height(), images.at(i).width(),
				          QImage::Format_RGBA8888);
				for(int j(0); j < im.height(); ++j)
				{
					for(int k(0); k < im.width(); ++k)
					{
						im.setPixel(im.width() - k - 1, im.height() - j - 1,
						            images.at(i).pixel(j, k));
					}
				}
				img_data.at(i) = im;
			}
			break;
			case static_cast<int>(CubeFace::LEFT):
				img_data.at(i) = images.at(i)
				                     .mirrored(false, true)
				                     .convertToFormat(QImage::Format_RGBA8888);
				break;
			case static_cast<int>(CubeFace::RIGHT):
				img_data.at(i) = images.at(i)
				                     .mirrored(true, false)
				                     .convertToFormat(QImage::Format_RGBA8888);
				break;
			default:
				break;
		}
	}
	for(unsigned int i(0); i < 6; ++i)
	{
		data.at(i) = img_data.at(i).bits();
	}

	return newTextureCubemap(images.at(0).width(), data,
	                         sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA);
}

GLHandler::Texture GLHandler::newTexture1D(unsigned int width,
                                           GLvoid const* data,
                                           GLint internalFormat, GLenum format,
                                           GLenum target, GLint filter,
                                           GLint wrap)
{
	++texCount();
	Texture tex  = {};
	tex.glTarget = target;
	glf().glGenTextures(1, &tex.glTexture);
	// glActiveTexture(GL_TEXTURE0);
	glf().glBindTexture(target, tex.glTexture);
	glf().glTexImage1D(target, 0, internalFormat, width, 0, format,
	                   GL_UNSIGNED_BYTE, data);
	// glGenerateMipmap(format);
	glf().glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
	glf().glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
	glf().glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
	/*GLfloat fLargest;
	glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest );
	glTexParameterf( format, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest );*/
	glf().glBindTexture(target, 0);

	return tex;
}

GLHandler::Texture GLHandler::newTexture2D(unsigned int width,
                                           unsigned int height,
                                           GLvoid const* data,
                                           GLint internalFormat, GLenum format,
                                           GLenum target, GLint filter,
                                           GLint wrap, GLenum type)
{
	++texCount();
	Texture tex  = {};
	tex.glTarget = target;
	glf().glGenTextures(1, &tex.glTexture);
	// glActiveTexture(GL_TEXTURE0);
	glf().glBindTexture(target, tex.glTexture);
	glf().glTexImage2D(target, 0, internalFormat, width, height, 0, format,
	                   type, data);
	// GL_UNSIGNED_BYTE, data);
	// glGenerateMipmap(target);
	glf().glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
	glf().glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
	glf().glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
	glf().glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
	/*GLfloat fLargest;
	glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest );
	glTexParameterf( target, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest );*/
	glf().glBindTexture(target, 0);

	return tex;
}

GLHandler::Texture GLHandler::newTextureCubemap(
    unsigned int side, std::array<GLvoid const*, 6> data, GLint internalFormat,
    GLenum format, GLenum target, GLint filter, GLint wrap)
{
	++texCount();
	Texture tex  = {};
	tex.glTarget = target;
	glf().glGenTextures(1, &tex.glTexture);
	// glActiveTexture(GL_TEXTURE0);
	glf().glBindTexture(target, tex.glTexture);
	for(unsigned int i(0); i < 6; ++i)
	{
		glf().glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
		                   internalFormat, side, side, 0, format,
		                   GL_UNSIGNED_BYTE, data.at(i));
	}
	// glGenerateMipmap(target);
	glf().glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
	glf().glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
	glf().glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
	glf().glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
	glf().glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap);
	/*GLfloat fLargest;
	glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest );
	glTexParameterf( target, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest );*/
	glf().glBindTexture(target, 0);

	return tex;
}

void GLHandler::useTextures(std::vector<Texture> const& textures)
{
	for(unsigned int i(0); i < textures.size(); ++i)
	{
		glf().glActiveTexture(GL_TEXTURE0 + i);
		glf().glBindTexture(textures[i].glTarget, textures[i].glTexture);
	}
}

void GLHandler::deleteTexture(Texture const& texture)
{
	--texCount();
	glf().glDeleteTextures(1, &texture.glTexture);
}

GLHandler::PixelBufferObject
    GLHandler::newPixelBufferObject(unsigned int width, unsigned int height)
{
	++PBOCount();
	PixelBufferObject result = {};
	result.width             = width;
	result.height            = height;

	glf().glGenBuffers(1, &result.id);
	glf().glBindBuffer(GL_PIXEL_UNPACK_BUFFER, result.id);
	glf().glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * 4, nullptr,
	                   GL_STREAM_DRAW);
	result.mappedData = static_cast<unsigned char*>(
	    glf().glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY));
	glf().glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	return result;
}

GLHandler::Texture GLHandler::copyPBOToTex(PixelBufferObject const& pbo,
                                           bool sRGB)
{
	Texture result = {};

	glf().glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo.id);
	glf().glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	// NOLINTNEXTLINE(hicpp-use-nullptr, modernize-use-nullptr)
	result = newTexture(pbo.width, pbo.height, static_cast<GLvoid*>(0), sRGB);
	glf().glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	return result;
}

void GLHandler::deletePixelBufferObject(PixelBufferObject const& pbo)
{
	--PBOCount();
	glf().glDeleteBuffers(1, &pbo.id);
	glf().glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
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
