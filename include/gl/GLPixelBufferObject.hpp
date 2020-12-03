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

#ifndef GLPIXELBUFFEROBJECT_HPP
#define GLPIXELBUFFEROBJECT_HPP

#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_0_Core>
#include <QSize>

#include "GLTexture.hpp"

class GLHandler;

class GLPixelBufferObject
{
  public:
	// implement those in protected if and only if they're needed for the Python
	// API
	GLPixelBufferObject(GLPixelBufferObject const& other) = delete;
	GLPixelBufferObject& operator=(GLPixelBufferObject const& other) = delete;
	/**
	 * @brief Returns the number of allocated OpenGL textures.
	 */
	static unsigned int getInstancesCount() { return instancesCount(); };

	GLPixelBufferObject(GLPixelBufferObject&& other)
	    : id(other.id)
	    , size(other.size)
	    , mappedData(other.mappedData)
	    , doClean(other.doClean)
	{
		// prevent other from cleaning shader if it destroys itself
		other.doClean = false;
	};

	explicit GLPixelBufferObject(QSize const& size);
	GLPixelBufferObject(unsigned int width, unsigned int height)
	    : GLPixelBufferObject(QSize(width, height)){};
	QSize getSize() { return size; };
	unsigned char* getMappedData() { return mappedData; };
	GLTexture* copyContentToNewTex(bool sRGB = true) const;

	virtual ~GLPixelBufferObject() { cleanUp(); };

  protected:
	/**
	 * @brief Frees the underlying OpenGL buffers.
	 */
	void cleanUp();

  private:
	GLuint id = 0;
	QSize size;
	unsigned char* mappedData;

	bool doClean = true;
	static unsigned int& instancesCount();
};

#endif // GLPIXELBUFFEROBJECT_HPP
