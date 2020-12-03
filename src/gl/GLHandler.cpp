#include "gl/GLHandler.hpp"

QOpenGLFunctions_4_0_Core& GLHandler::glf()
{
	static QOpenGLFunctions_4_0_Core glf;
	return glf;
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

void GLHandler::setClearColor(QColor const& color)
{
	glf().glClearColor(color.redF(), color.greenF(), color.blueF(),
	                   color.alphaF());
}

void GLHandler::beginRendering(GLFramebufferObject const& renderTarget,
                               GLTexture::CubemapFace face, GLint layer)
{
	beginRendering(static_cast<GLuint>(GL_COLOR_BUFFER_BIT)
	                   | static_cast<GLuint>(GL_DEPTH_BUFFER_BIT),
	               renderTarget, face, layer);
}

void GLHandler::beginRendering(GLbitfield clearMask,
                               GLFramebufferObject const& renderTarget,
                               GLTexture::CubemapFace face, GLint layer)
{
	renderTarget.bind(face, layer);
	glf().glClear(clearMask);
	glf().glViewport(0, 0, renderTarget.getSize().width(),
	                 renderTarget.getSize().height());
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
    GLShaderProgram const& shader, GLFramebufferObject const& from,
    GLFramebufferObject const& to,
    std::vector<GLTexture const*> const& uniformTextures)
{
	GLMesh quad;
	quad.setVertices({-1.f, -1.f, 1.f, -1.f, -1.f, 1.f, 1.f, 1.f}, shader,
	                 {{"position", 2}});

	beginRendering(to);
	shader.use();
	std::vector<GLTexture const*> texs;
	texs.push_back(&from.getColorAttachmentTexture());
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
                                  GLFramebufferObject const& to)
{
	GLMesh quad;
	quad.setVertices({-1.f, -1.f, 1.f, -1.f, -1.f, 1.f, 1.f, 1.f}, shader,
	                 {{"position", 2}});

	if(to.getDepth() == 1)
	{
		beginRendering(to);
		shader.use();
		setBackfaceCulling(false);
		quad.render(PrimitiveType::TRIANGLE_STRIP);
		setBackfaceCulling(true);
	}
	else
	{
		for(unsigned int i(0); i < to.getDepth(); ++i)
		{
			GLHandler::beginRendering(to, GLTexture::CubemapFace::FRONT, i);
			shader.setUniform("z", i / static_cast<float>(to.getDepth()));
			GLHandler::setBackfaceCulling(false);
			quad.render(PrimitiveType::TRIANGLE_STRIP);
			setBackfaceCulling(true);
		}
	}
}

void GLHandler::generateEnvironmentMap(
    GLFramebufferObject const& renderTarget,
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
