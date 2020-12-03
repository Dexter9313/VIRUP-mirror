/*
    Copyright (C) 2020 Florian Cabot <florian.cabot@hotmail.fr>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include "gl/GLHandler.hpp"

#include "gl/GLFramebufferObject.hpp"

unsigned int& GLFramebufferObject::instancesCount()
{
	static unsigned int instancesCount = 0;
	return instancesCount;
}

GLFramebufferObject::GLFramebufferObject(
    GLTexture::Tex1DProperties const& properties,
    GLTexture::Sampler const& sampler)
    : width(properties.width)
{
	++instancesCount();

	GLHandler::glf().glGenFramebuffers(1, &fbo);
	GLHandler::glf().glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// generate texture
	texColorBuffer = new GLTexture(properties, sampler);
}

GLFramebufferObject::GLFramebufferObject(
    GLTexture::Tex2DProperties const& properties,
    GLTexture::Sampler const& sampler)
    : width(properties.width)
    , height(properties.height)
{
	++instancesCount();
	if(properties.internalFormat == GL_DEPTH_COMPONENT
	   || properties.internalFormat == GL_DEPTH_COMPONENT16
	   || properties.internalFormat == GL_DEPTH_COMPONENT24
	   || properties.internalFormat == GL_DEPTH_COMPONENT32)
	{
		isDepthMap = true;
	}

	GLHandler::glf().glGenFramebuffers(1, &fbo);
	GLHandler::glf().glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	if(isDepthMap)
	{
		// generate texture
		texColorBuffer = new GLTexture(properties, sampler,
		                               {nullptr, GL_FLOAT, GL_DEPTH_COMPONENT});

		// add depth specific texture parameters for sampler2DShadow
		GLHandler::glf().glBindTexture(GL_TEXTURE_2D,
		                               texColorBuffer->getGLTexture());
		GLHandler::glf().glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		                                 GL_COMPARE_REF_TO_TEXTURE);
		GLHandler::glf().glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC,
		                                 GL_LEQUAL);

		GLHandler::glf().glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		GLHandler::glf().glFramebufferTexture2D(
		    GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		    texColorBuffer->getGLTexture(), 0);
		GLHandler::glf().glDrawBuffer(GL_NONE);
		GLHandler::glf().glReadBuffer(GL_NONE);
		GLHandler::glf().glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else
	{
		// generate texture
		texColorBuffer = new GLTexture(properties, sampler);

		// render buffer for depth and stencil
		GLHandler::glf().glGenRenderbuffers(1, &renderBuffer);
		GLHandler::glf().glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
		GLHandler::glf().glRenderbufferStorage(
		    GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		GLHandler::glf().glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// attach it to currently bound framebuffer object
		GLHandler::glf().glFramebufferRenderbuffer(
		    GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
		    renderBuffer);
	}
}

GLFramebufferObject::GLFramebufferObject(
    GLTexture::TexMultisampleProperties const& properties,
    GLTexture::Sampler const& sampler)
    : width(properties.width)
    , height(properties.height)
{
	++instancesCount();

	GLHandler::glf().glGenFramebuffers(1, &fbo);
	GLHandler::glf().glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// generate texture
	texColorBuffer = new GLTexture(properties, sampler);

	// render buffer for depth and stencil
	GLHandler::glf().glGenRenderbuffers(1, &renderBuffer);
	GLHandler::glf().glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	GLHandler::glf().glRenderbufferStorageMultisample(
	    GL_RENDERBUFFER, properties.samples, GL_DEPTH24_STENCIL8, width,
	    height);
	GLHandler::glf().glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach it to currently bound framebuffer object
	GLHandler::glf().glFramebufferRenderbuffer(GL_FRAMEBUFFER,
	                                           GL_DEPTH_STENCIL_ATTACHMENT,
	                                           GL_RENDERBUFFER, renderBuffer);
}

GLFramebufferObject::GLFramebufferObject(
    GLTexture::Tex3DProperties const& properties,
    GLTexture::Sampler const& sampler)
    : width(properties.width)
    , height(properties.height)
    , depth(properties.depth)
{
	++instancesCount();

	GLHandler::glf().glGenFramebuffers(1, &fbo);
	GLHandler::glf().glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// generate texture
	texColorBuffer = new GLTexture(properties, sampler);
}

GLFramebufferObject::GLFramebufferObject(
    GLTexture::TexCubemapProperties const& properties,
    GLTexture::Sampler const& sampler)
    : width(properties.side)
    , height(properties.side)
{
	++instancesCount();

	GLHandler::glf().glGenFramebuffers(1, &fbo);
	GLHandler::glf().glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// generate texture
	texColorBuffer = new GLTexture(properties, sampler);

	// render buffer for depth and stencil
	GLHandler::glf().glGenRenderbuffers(1, &renderBuffer);
	GLHandler::glf().glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	GLHandler::glf().glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
	                                       width, height);
	GLHandler::glf().glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach it to currently bound framebuffer object
	GLHandler::glf().glFramebufferRenderbuffer(GL_FRAMEBUFFER,
	                                           GL_DEPTH_STENCIL_ATTACHMENT,
	                                           GL_RENDERBUFFER, renderBuffer);
}

void GLFramebufferObject::bind(GLTexture::CubemapFace face, GLint layer) const
{
	GLHandler::glf().glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	if(fbo != 0 && !isDepthMap)
	{
		if(texColorBuffer->getGLTarget() == GL_TEXTURE_CUBE_MAP)
		{
			GLHandler::glf().glFramebufferTexture2D(
			    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			    static_cast<unsigned int>(face)
			        + GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			    texColorBuffer->getGLTexture(), 0);
		}
		else if(texColorBuffer->getGLTarget() == GL_TEXTURE_1D)
		{
			GLHandler::glf().glFramebufferTexture1D(
			    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			    texColorBuffer->getGLTarget(), texColorBuffer->getGLTexture(),
			    0);
		}
		else if(texColorBuffer->getGLTarget() == GL_TEXTURE_3D)
		{
			GLHandler::glf().glFramebufferTexture3D(
			    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			    texColorBuffer->getGLTarget(), texColorBuffer->getGLTexture(),
			    0, layer);
		}
		else
		{
			GLHandler::glf().glFramebufferTexture2D(
			    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			    texColorBuffer->getGLTarget(), texColorBuffer->getGLTexture(),
			    0);
		}
	}
}

void GLFramebufferObject::blitColorBufferTo(GLFramebufferObject const& to) const
{
	blitColorBufferTo(to, 0, 0, width, height, 0, 0, to.width, to.height);
}

void GLFramebufferObject::blitColorBufferTo(GLFramebufferObject const& to,
                                            int srcX0, int srcY0, int srcX1,
                                            int srcY1, int dstX0, int dstY0,
                                            int dstX1, int dstY1) const
{
	GLHandler::glf().glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	GLHandler::glf().glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to.fbo);
	GLHandler::glf().glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0,
	                                   dstX1, dstY1, GL_COLOR_BUFFER_BIT,
	                                   GL_LINEAR);
}
void GLFramebufferObject::blitDepthBufferTo(GLFramebufferObject const& to) const
{
	GLHandler::glf().glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	GLHandler::glf().glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to.fbo);
	GLHandler::glf().glBlitFramebuffer(0, 0, width, height, 0, 0, to.width,
	                                   to.height, GL_DEPTH_BUFFER_BIT,
	                                   GL_NEAREST);
}

void GLFramebufferObject::showOnScreen(int screenx0, int screeny0, int screenx1,
                                       int screeny1) const
{
	blitColorBufferTo({}, 0, 0, width, height, screenx0, screeny0, screenx1,
	                  screeny1);
}

QImage GLFramebufferObject::copyColorBufferToQImage() const
{
	auto data(new uchar[width * height * 4]);

	GLHandler::glf().glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	GLHandler::glf().glReadPixels(0, 0, width, height, GL_RGBA,
	                              GL_UNSIGNED_BYTE, static_cast<GLvoid*>(data));

	return QImage(data, width, height, width * 4,
	              QImage::Format::Format_RGBA8888,
	              [](void* data) { delete static_cast<uchar*>(data); }, data);
}

void GLFramebufferObject::cleanUp()
{
	if(!doClean)
	{
		return;
	}
	--instancesCount();
	GLHandler::glf().glBindFramebuffer(GL_FRAMEBUFFER, 0);
	delete texColorBuffer;
	GLHandler::glf().glDeleteRenderbuffers(1, &renderBuffer);
	GLHandler::glf().glDeleteFramebuffers(1, &fbo);
	doClean = false;
}
