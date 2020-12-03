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

#ifndef MAINRENDERTARGET_HPP
#define MAINRENDERTARGET_HPP

#include "gl/GLHandler.hpp"

class MainRenderTarget
{
  public:
	enum class Projection
	{
		DEFAULT     = 0,
		PANORAMA360 = 1,
		VR360       = 2,
	};

	MainRenderTarget(unsigned int width, unsigned int height,
	                 unsigned int samples, Projection projection)
	    : sceneTarget(constructSceneTarget(width, height, samples, projection))
	    , postProcessingTargets({GLFramebufferObject(GLTexture::Tex2DProperties(
	                                 width, height, GL_RGBA32F)),
	                             GLFramebufferObject(GLTexture::Tex2DProperties(
	                                 width, height, GL_RGBA32F))}){};

	static GLFramebufferObject constructSceneTarget(unsigned int width,
	                                                unsigned int height,
	                                                unsigned int samples,
	                                                Projection projection)
	{
		if(projection == Projection::DEFAULT)
		{
			if(samples > 1)
			{
				return GLFramebufferObject(GLTexture::TexMultisampleProperties(
				    width, height, samples, GL_RGBA32F));
			}
			return GLFramebufferObject(
			    GLTexture::Tex2DProperties(width, height, GL_RGBA32F));
		}
		return GLFramebufferObject(
		    GLTexture::TexCubemapProperties(width / 3, GL_RGBA32F));
	}

  public:
	GLFramebufferObject sceneTarget;
	std::array<GLFramebufferObject, 2> postProcessingTargets;
};

#endif // MAINRENDERTARGET_HPP
