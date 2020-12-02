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
	    = new GLTexture(GLTexture::Tex1DProperties(width, format),
	                    {GL_LINEAR, GL_MIRRORED_REPEAT});

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
		    = new GLTexture(GLTexture::Tex2DProperties(width, height, format),
		                    {GL_LINEAR, GL_MIRRORED_REPEAT});
	}
	else
	{
		result.texColorBuffer
		    = new GLTexture(GLTexture::TexCubemapProperties(width, format),
		                    {GL_LINEAR, GL_MIRRORED_REPEAT});
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
	result.texColorBuffer = new GLTexture(
	    GLTexture::TexMultisampleProperties(width, height, samples, format),
	    {GL_LINEAR, GL_MIRRORED_REPEAT});

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
	result.texColorBuffer = new GLTexture(
	    GLTexture::Tex3DProperties(width, height, depth, format),
	    {GL_LINEAR, GL_MIRRORED_REPEAT});

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

	result.texColorBuffer = new GLTexture(
	    GLTexture::Tex2DProperties(width, height, GL_DEPTH_COMPONENT32),
	    {GL_LINEAR, GL_CLAMP_TO_EDGE}, {nullptr, GL_FLOAT, GL_DEPTH_COMPONENT});
	// add depth specific texture parameters for sampler2DShadow
	glf().glBindTexture(GL_TEXTURE_2D, result.texColorBuffer->glTexture);
	glf().glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
	                      GL_COMPARE_REF_TO_TEXTURE);
	glf().glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glf().glBindFramebuffer(GL_FRAMEBUFFER, result.frameBuffer);
	glf().glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
	                             GL_TEXTURE_2D,
	                             result.texColorBuffer->glTexture, 0);
	glf().glDrawBuffer(GL_NONE);
	glf().glReadBuffer(GL_NONE);
	glf().glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return result;
}

GLTexture const&
    GLHandler::getColorAttachmentTexture(RenderTarget const& renderTarget)
{
	return *renderTarget.texColorBuffer;
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

void GLHandler::deleteRenderTarget(RenderTarget& renderTarget)
{
	--renderTargetCount();
	glf().glBindFramebuffer(GL_FRAMEBUFFER, 0);
	delete renderTarget.texColorBuffer;
	glf().glDeleteRenderbuffers(1, &renderTarget.renderBuffer);
	glf().glDeleteFramebuffers(1, &renderTarget.frameBuffer);
}

void GLHandler::setClearColor(QColor const& color)
{
	glf().glClearColor(color.redF(), color.greenF(), color.blueF(),
	                   color.alphaF());
}

void GLHandler::beginRendering(RenderTarget const& renderTarget,
                               GLTexture::CubemapFace face, GLint layer)
{
	beginRendering(static_cast<GLuint>(GL_COLOR_BUFFER_BIT)
	                   | static_cast<GLuint>(GL_DEPTH_BUFFER_BIT),
	               renderTarget, face, layer);
}

void GLHandler::beginRendering(GLbitfield clearMask,
                               RenderTarget const& renderTarget,
                               GLTexture::CubemapFace face, GLint layer)
{
	glf().glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.frameBuffer);
	if(renderTarget.frameBuffer != 0 && !renderTarget.isDepthMap)
	{
		if(renderTarget.texColorBuffer->glTarget == GL_TEXTURE_CUBE_MAP)
		{
			glf().glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			                             static_cast<unsigned int>(face)
			                                 + GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			                             renderTarget.texColorBuffer->glTexture,
			                             0);
		}
		else if(renderTarget.texColorBuffer->glTarget == GL_TEXTURE_1D)
		{
			glf().glFramebufferTexture1D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			                             renderTarget.texColorBuffer->glTarget,
			                             renderTarget.texColorBuffer->glTexture,
			                             0);
		}
		else if(renderTarget.texColorBuffer->glTarget == GL_TEXTURE_3D)
		{
			glf().glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			                             renderTarget.texColorBuffer->glTarget,
			                             renderTarget.texColorBuffer->glTexture,
			                             0, layer);
		}
		else
		{
			glf().glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			                             renderTarget.texColorBuffer->glTarget,
			                             renderTarget.texColorBuffer->glTexture,
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

void GLHandler::postProcess(
    GLShaderProgram const& shader, RenderTarget const& from,
    RenderTarget const& to,
    std::vector<GLTexture const*> const& uniformTextures)
{
	GLMesh quad;
	quad.setVertices({-1.f, -1.f, 1.f, -1.f, -1.f, 1.f, 1.f, 1.f}, shader,
	                 {{"position", 2}});

	beginRendering(to);
	shader.use();
	std::vector<GLTexture const*> texs;
	texs.push_back(&getColorAttachmentTexture(from));
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
			GLHandler::beginRendering(to, GLTexture::CubemapFace::FRONT, i);
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

	std::vector<GLTexture::CubemapFace> faces = {
	    GLTexture::CubemapFace::FRONT,  GLTexture::CubemapFace::BACK,
	    GLTexture::CubemapFace::LEFT,   GLTexture::CubemapFace::RIGHT,
	    GLTexture::CubemapFace::BOTTOM, GLTexture::CubemapFace::TOP,
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

void GLHandler::useTextures(std::vector<GLTexture const*> const& textures)
{
	for(unsigned int i(0); i < textures.size(); ++i)
	{
		if(textures[i] != nullptr)
		{
			textures[i]->use(GL_TEXTURE0 + i);
		}
	}
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

GLTexture* GLHandler::copyPBOToTex(PixelBufferObject const& pbo, bool sRGB)
{
	glf().glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo.id);
	glf().glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	// NOLINTNEXTLINE(hicpp-use-nullptr, modernize-use-nullptr)
	auto result
	    = new GLTexture(GLTexture::Tex2DProperties(pbo.width, pbo.height, sRGB),
	                    {}, {static_cast<GLvoid*>(nullptr)});
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
