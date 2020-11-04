#include "gl/GLHandler.hpp"

unsigned int& GLHandler::renderTargetCount()
{
	static unsigned int renderTargetCount = 0;
	return renderTargetCount;
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
	static GLint defaultRenderTargetFormat = GL_RGBA32F;
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
	PythonQtHandler::addClass<Texture>("Texture");
	PythonQtHandler::addClass<RenderTarget>("RenderTarget");
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

GLHandler::RenderTarget GLHandler::newRenderTarget1D(unsigned int width)
{
	return newRenderTarget1D(width, defaultRenderTargetFormat());
}

GLHandler::RenderTarget GLHandler::newRenderTarget1D(unsigned int width,
                                                     GLint format)
{
	++renderTargetCount();
	RenderTarget result = {width, 1};

	glf().glGenFramebuffers(1, &result.frameBuffer);
	glf().glBindFramebuffer(GL_FRAMEBUFFER, result.frameBuffer);

	// generate texture
	result.texColorBuffer
	    = newTexture1D(width, nullptr, format, GL_RGBA, GL_TEXTURE_1D,
	                   GL_LINEAR, GL_MIRRORED_REPEAT);

	return result;
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

GLHandler::RenderTarget GLHandler::newRenderTargetMultisample(
    unsigned int width, unsigned int height, unsigned int samples, GLint format)
{
	++renderTargetCount();
	RenderTarget result = {width, height};

	glf().glGenFramebuffers(1, &result.frameBuffer);
	glf().glBindFramebuffer(GL_FRAMEBUFFER, result.frameBuffer);

	// generate texture
	result.texColorBuffer = newTextureMultisample(
	    width, height, samples, format, GL_LINEAR, GL_MIRRORED_REPEAT);

	// render buffer for depth and stencil
	glf().glGenRenderbuffers(1, &result.renderBuffer);
	glf().glBindRenderbuffer(GL_RENDERBUFFER, result.renderBuffer);
	glf().glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples,
	                                       GL_DEPTH24_STENCIL8, width, height);
	glf().glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach it to currently bound framebuffer object
	glf().glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
	                                GL_RENDERBUFFER, result.renderBuffer);

	return result;
}

GLHandler::RenderTarget GLHandler::newRenderTarget3D(unsigned int width,
                                                     unsigned int height,
                                                     unsigned int depth)
{
	return newRenderTarget3D(width, height, depth, defaultRenderTargetFormat());
}

GLHandler::RenderTarget GLHandler::newRenderTarget3D(unsigned int width,
                                                     unsigned int height,
                                                     unsigned int depth,
                                                     GLint format)
{
	++renderTargetCount();
	RenderTarget result = {width, height, depth};

	glf().glGenFramebuffers(1, &result.frameBuffer);
	glf().glBindFramebuffer(GL_FRAMEBUFFER, result.frameBuffer);

	// generate texture
	result.texColorBuffer
	    = newTexture3D(width, height, depth, nullptr, format, GL_RGBA,
	                   GL_TEXTURE_3D, GL_LINEAR, GL_MIRRORED_REPEAT);

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
	blitColorBuffer(from, to, 0, 0, from.width, from.height, 0, 0, to.width,
	                to.height);
}

void GLHandler::blitColorBuffer(RenderTarget const& from,
                                RenderTarget const& to, int srcX0, int srcY0,
                                int srcX1, int srcY1, int dstX0, int dstY0,
                                int dstX1, int dstY1)
{
	glf().glBindFramebuffer(GL_READ_FRAMEBUFFER, from.frameBuffer);
	glf().glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to.frameBuffer);
	glf().glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1,
	                        dstY1, GL_COLOR_BUFFER_BIT, GL_LINEAR);
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

void GLHandler::beginRendering(RenderTarget const& renderTarget, CubeFace face,
                               GLint layer)
{
	beginRendering(static_cast<GLuint>(GL_COLOR_BUFFER_BIT)
	                   | static_cast<GLuint>(GL_DEPTH_BUFFER_BIT),
	               renderTarget, face, layer);
}

void GLHandler::beginRendering(GLbitfield clearMask,
                               RenderTarget const& renderTarget, CubeFace face,
                               GLint layer)
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
		else if(renderTarget.texColorBuffer.glTarget == GL_TEXTURE_1D)
		{
			glf().glFramebufferTexture1D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			                             renderTarget.texColorBuffer.glTarget,
			                             renderTarget.texColorBuffer.glTexture,
			                             0);
		}
		else if(renderTarget.texColorBuffer.glTarget == GL_TEXTURE_3D)
		{
			glf().glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			                             renderTarget.texColorBuffer.glTarget,
			                             renderTarget.texColorBuffer.glTexture,
			                             0, layer);
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

void GLHandler::setUpRender(GLShaderProgram const& shader,
                            QMatrix4x4 const& model, GeometricSpace space)
{
	switch(space)
	{
		case GeometricSpace::CLIP:
			shader.setUniform("camera", model);
			break;
		case GeometricSpace::WORLD:
			shader.setUniform("camera", fullTransform() * model);
			break;
		case GeometricSpace::CAMERA:
			shader.setUniform("camera", fullCameraSpaceTransform() * model);
			break;
		case GeometricSpace::SEATEDTRACKED:
			shader.setUniform("camera",
			                  fullSeatedTrackedSpaceTransform() * model);
			break;
		case GeometricSpace::STANDINGTRACKED:
			shader.setUniform("camera",
			                  fullStandingTrackedSpaceTransform() * model);
			break;
		case GeometricSpace::HMD:
			shader.setUniform("camera", fullHmdSpaceTransform() * model);
			break;
		case GeometricSpace::SKYBOX:
			shader.setUniform("camera", fullSkyboxSpaceTransform() * model);
			break;
		default:
			break;
	};
}

void GLHandler::postProcess(GLShaderProgram const& shader,
                            RenderTarget const& from, RenderTarget const& to,
                            std::vector<Texture> const& uniformTextures)
{
	GLMesh quad;
	quad.setVertices({-1.f, -1.f, 1.f, -1.f, -1.f, 1.f, 1.f, 1.f}, shader,
	                 {{"position", 2}});

	beginRendering(to);
	shader.use();
	std::vector<Texture> texs;
	texs.push_back(getColorAttachmentTexture(from));
	// TODO(florian) performance
	for(auto tex : uniformTextures)
	{
		texs.push_back(tex);
	}
	useTextures(texs);
	setBackfaceCulling(false);
	quad.render(PrimitiveType::TRIANGLE_STRIP);
	setBackfaceCulling(true);
}

void GLHandler::renderFromScratch(GLShaderProgram const& shader,
                                  RenderTarget const& to)
{
	GLMesh quad;
	quad.setVertices({-1.f, -1.f, 1.f, -1.f, -1.f, 1.f, 1.f, 1.f}, shader,
	                 {{"position", 2}});

	if(to.depth == 1)
	{
		beginRendering(to);
		shader.use();
		setBackfaceCulling(false);
		quad.render(PrimitiveType::TRIANGLE_STRIP);
		setBackfaceCulling(true);
	}
	else
	{
		for(unsigned int i(0); i < to.depth; ++i)
		{
			GLHandler::beginRendering(to, GLHandler::CubeFace::FRONT, i);
			shader.setUniform("z", i / static_cast<float>(to.depth));
			GLHandler::setBackfaceCulling(false);
			quad.render(PrimitiveType::TRIANGLE_STRIP);
			setBackfaceCulling(true);
		}
	}
}

void GLHandler::generateEnvironmentMap(
    GLHandler::RenderTarget const& renderTarget,
    std::function<void(bool, QMatrix4x4, QMatrix4x4)> const& renderFunction,
    QVector3D const& shift)
{
	QMatrix4x4 perspective;
	perspective.perspective(90.f, 1.f, 0.1f, 10000.f);

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
		cubeCamera.translate(-1.f * shift);
		GLHandler::beginRendering(renderTarget, faces[i]);
		renderFunction(true, cubeCamera, perspective);
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
                                           GLint wrap, GLenum type)
{
	++texCount();
	Texture tex  = {};
	tex.glTarget = target;
	glf().glGenTextures(1, &tex.glTexture);
	// glActiveTexture(GL_TEXTURE0);
	glf().glBindTexture(target, tex.glTexture);
	glf().glTexImage1D(target, 0, internalFormat, width, 0, format, type, data);
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

GLHandler::Texture GLHandler::newTextureMultisample(unsigned int width,
                                                    unsigned int height,
                                                    unsigned int samples,
                                                    GLint internalFormat,
                                                    GLint filter, GLint wrap)
{
	++texCount();
	Texture tex  = {};
	tex.glTarget = GL_TEXTURE_2D_MULTISAMPLE;
	glf().glGenTextures(1, &tex.glTexture);
	// glActiveTexture(GL_TEXTURE0);
	glf().glBindTexture(tex.glTarget, tex.glTexture);
	glf().glTexImage2DMultisample(tex.glTarget, samples, internalFormat, width,
	                              height, GL_TRUE);
	// GL_UNSIGNED_BYTE, data);
	// glGenerateMipmap(target);
	glf().glTexParameteri(tex.glTarget, GL_TEXTURE_MIN_FILTER, filter);
	glf().glTexParameteri(tex.glTarget, GL_TEXTURE_MAG_FILTER, filter);
	glf().glTexParameteri(tex.glTarget, GL_TEXTURE_WRAP_S, wrap);
	glf().glTexParameteri(tex.glTarget, GL_TEXTURE_WRAP_T, wrap);
	/*GLfloat fLargest;
	glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest );
	glTexParameterf( tex.glTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest );*/
	glf().glBindTexture(tex.glTarget, 0);

	return tex;
}

GLHandler::Texture
    GLHandler::newTexture3D(unsigned int width, unsigned int height,
                            unsigned int depth, GLvoid const* data,
                            GLint internalFormat, GLenum format, GLenum target,
                            GLint filter, GLint wrap, GLenum type)
{
	++texCount();
	Texture tex  = {};
	tex.glTarget = target;
	glf().glGenTextures(1, &tex.glTexture);
	// glActiveTexture(GL_TEXTURE0);
	glf().glBindTexture(target, tex.glTexture);
	glf().glTexImage3D(target, 0, internalFormat, width, height, depth, 0,
	                   format, type, data);
	// GL_UNSIGNED_BYTE, data);
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

QSize GLHandler::getTextureSize(Texture const& tex, unsigned int level)
{
	GLint width, height;
	glf().glBindTexture(tex.glTarget, tex.glTexture);
	glf().glGetTexLevelParameteriv(tex.glTarget, level, GL_TEXTURE_WIDTH,
	                               &width);
	glf().glGetTexLevelParameteriv(tex.glTarget, level, GL_TEXTURE_HEIGHT,
	                               &height);
	glf().glBindTexture(tex.glTarget, 0);

	return {width, height};
}

void GLHandler::generateMipmap(Texture const& tex)
{
	glf().glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	glf().glBindTexture(tex.glTarget, tex.glTexture);
	glf().glTexParameteri(tex.glTarget, GL_TEXTURE_MIN_FILTER,
	                      GL_LINEAR_MIPMAP_LINEAR);
	glf().glGenerateMipmap(tex.glTarget);
	glf().glBindTexture(tex.glTarget, 0);
}

unsigned int GLHandler::getHighestMipmapLevel(Texture const& tex)
{
	QSize size(GLHandler::getTextureSize(tex));
	return static_cast<unsigned int>(
	    log2(size.width() > size.height() ? size.width() : size.height()));
}

QImage GLHandler::getTextureContentAsImage(Texture const& tex,
                                           unsigned int level)
{
	QSize size(getTextureSize(tex, level));

	GLint internalFormat;
	glf().glBindTexture(tex.glTarget, tex.glTexture);
	glf().glGetTexLevelParameteriv(
	    tex.glTarget, level, GL_TEXTURE_INTERNAL_FORMAT,
	    &internalFormat);  // get internal format type of GL texture
	switch(internalFormat) // determine what type GL texture has...
	{
		case GL_RGB:
		{
			QImage result(size, QImage::Format::Format_RGB888);
			glf().glGetTexImage(tex.glTarget, level, GL_RGBA, GL_UNSIGNED_BYTE,
			                    result.bits());
			return result;
		}
		break;
		case GL_RGBA:
		{
			QImage result(size, QImage::Format::Format_RGBA8888);
			glf().glGetTexImage(tex.glTarget, level, GL_RGBA, GL_UNSIGNED_BYTE,
			                    result.bits());
			return result;
		}
		break;
		case GL_SRGB8_ALPHA8:
		{
			QImage result(size, QImage::Format::Format_RGBA8888);
			glf().glGetTexImage(tex.glTarget, level, GL_RGBA, GL_UNSIGNED_BYTE,
			                    result.bits());
			return result;
		}
		default: // unsupported type for now
			break;
	}

	return {};
}

unsigned int GLHandler::getTextureContentAsData(GLfloat** buff,
                                                Texture const& tex,
                                                unsigned int level)
{
	QSize size(getTextureSize(tex, level));

	GLint internalFormat;
	glf().glBindTexture(tex.glTarget, tex.glTexture);
	glf().glGetTexLevelParameteriv(
	    tex.glTarget, level, GL_TEXTURE_INTERNAL_FORMAT,
	    &internalFormat); // get internal format type of GL texture
	GLint numFloats = 0;
	if(internalFormat == GL_RGBA32F) // determine what type GL texture has...
	{
		numFloats = size.width() * size.height() * 4;
		*buff     = new GLfloat[numFloats];
		glf().glGetTexImage(tex.glTarget, level, GL_RGBA, GL_FLOAT, *buff);
	}
	return numFloats;
}

float GLHandler::getTextureAverageLuminance(Texture const& tex)
{
	GLHandler::generateMipmap(tex);
	unsigned int lvl = GLHandler::getHighestMipmapLevel(tex) - 3;
	auto size        = GLHandler::getTextureSize(tex, lvl);
	GLfloat* buff;
	unsigned int allocated(GLHandler::getTextureContentAsData(&buff, tex, lvl));
	float lastFrameAverageLuminance = 0.f;
	if(allocated > 0)
	{
		float coeffSum = 0.f;
		float halfWidth((size.width() - 1) / 2.f);
		float halfHeight((size.height() - 1) / 2.f);
		for(int i(0); i < size.width(); ++i)
		{
			for(int j(0); j < size.height(); ++j)
			{
				unsigned int id(j * size.width() + i);
				float lum(0.2126 * buff[4 * id] + 0.7152 * buff[4 * id + 1]
				          + 0.0722 * buff[4 * id + 2]);
				float coeff
				    = exp(-1 * pow((i - halfWidth) * 4.5 / halfWidth, 2));
				coeff *= exp(-1 * pow((j - halfHeight) * 4.5 / halfHeight, 2));
				coeffSum += coeff;
				lastFrameAverageLuminance += coeff * lum;
			}
		}
		lastFrameAverageLuminance /= coeffSum;
		delete[] buff;
	}
	return lastFrameAverageLuminance;
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
