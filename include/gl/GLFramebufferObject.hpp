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

#ifndef GLFRAMEBUFFEROBJECT_HPP
#define GLFRAMEBUFFEROBJECT_HPP

#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_0_Core>
#include <QSize>

#include "GLTexture.hpp"

class GLHandler;

/** @ingroup pywrap
 * @brief Represents an OpenGL Framebuffer Object.
 *
 * An FBO is a collection of buffers in video memory on which a
 * scene is rendered. The default FBO is the screen. Having
 * several other FBOs than the screen can be useful for VR
 * rendering (one per eye) or post-processing for example (render the scene
 * on a FBO then pass it to a post-processing shader that is
 * rendering to the screen buffer). About post-processing, @ref GLHandler as
 * high level methods to do it, you shouldn't have to do post-processing
 * manually.
 *
 * To be more specific, a FBO contains pointers to an OpenGL FBO, a
 * color attachment buffer (GLTexture) and a render buffer (depth + stencil).
 * Deferred rendering is therefore not supported yet. It also keeps its
 * attachments size (width + height).
 */
class GLFramebufferObject
{
  public:
	// implement those in protected if and only if they're needed for the Python
	// API
	GLFramebufferObject(GLFramebufferObject const& other) = delete;
	GLFramebufferObject& operator=(GLFramebufferObject const& other) = delete;
	/**
	 * @brief Returns the number of allocated OpenGL textures.
	 */
	static unsigned int getInstancesCount() { return instancesCount(); };

	GLFramebufferObject(GLFramebufferObject&& other)
	    : fbo(other.fbo)
	    , texColorBuffer(other.texColorBuffer)
	    , renderBuffer(other.renderBuffer)
	    , width(other.width)
	    , depth(other.depth)
	    , isDepthMap(other.isDepthMap)
	    , doClean(other.doClean)
	{
		// prevent other from cleaning fbo if it destroys itself
		other.doClean = false;
	};

	// screen
	GLFramebufferObject() { ++instancesCount(); };
	explicit GLFramebufferObject(GLTexture::Tex1DProperties const& properties,
	                             GLTexture::Sampler const& sampler
	                             = {GL_LINEAR, GL_MIRRORED_REPEAT});
	// if properties.internalFormat is GL_DEPTH_COMPONENT{i}, considered a depth
	// map
	explicit GLFramebufferObject(GLTexture::Tex2DProperties const& properties,
	                             GLTexture::Sampler const& sampler
	                             = {GL_LINEAR, GL_MIRRORED_REPEAT});
	explicit GLFramebufferObject(
	    GLTexture::TexMultisampleProperties const& properties,
	    GLTexture::Sampler const& sampler = {GL_LINEAR, GL_MIRRORED_REPEAT});
	explicit GLFramebufferObject(GLTexture::Tex3DProperties const& properties,
	                             GLTexture::Sampler const& sampler
	                             = {GL_LINEAR, GL_MIRRORED_REPEAT});
	explicit GLFramebufferObject(
	    GLTexture::TexCubemapProperties const& properties,
	    GLTexture::Sampler const& sampler = {GL_LINEAR, GL_MIRRORED_REPEAT});

	QSize getSize() const { return QSize(width, height); };
	unsigned int getDepth() const { return depth; };
	GLTexture const& getColorAttachmentTexture() const
	{
		return *texColorBuffer;
	};
	void bind(GLTexture::CubemapFace face, GLint layer) const;
	void blitColorBufferTo(GLFramebufferObject const& to) const;
	void blitColorBufferTo(GLFramebufferObject const& to, int srcX0, int srcY0,
	                       int srcX1, int srcY1, int dstX0, int dstY0,
	                       int dstX1, int dstY1) const;
	void blitDepthBufferTo(GLFramebufferObject const& to) const;
	/**
	 * @brief Shows this FBO's color attachment content on screen.
	 *
	 * It will be displayed as rectangle which top-left coordinates are (@p
	 * screenx0, @p screeny0) and bottom-right coordinates are (@p screenx1, @p
	 * screeny1).
	 *
	 * Coordinates are from window space (0->width, 0->height).
	 */
	void showOnScreen(int screenx0, int screeny0, int screenx1,
	                  int screeny1) const;
	QImage copyColorBufferToQImage() const;

	virtual ~GLFramebufferObject() { cleanUp(); };

  protected:
	/**
	 * @brief Frees the underlying OpenGL buffers.
	 */
	void cleanUp();

  private:
	GLuint fbo = 0;
	// if depth map, will be the depth buffer instead
	GLTexture* texColorBuffer = nullptr;
	GLuint renderBuffer       = 0;
	unsigned int width        = 1;
	unsigned int height       = 1;
	unsigned int depth        = 1;
	bool isDepthMap           = false;

	bool doClean = true;
	static unsigned int& instancesCount();
};

#endif // GLFRAMEBUFFEROBJECT_HPP
