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

#ifndef GLBUFFER_HPP
#define GLBUFFER_HPP

#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_2_Core>
#include <QSize>

#include "GLTexture.hpp"

class GLHandler;

class GLBuffer
{
  public:
	// implement those in protected if and only if they're needed for the Python
	// API
	GLBuffer(GLBuffer const& other) = delete;
	GLBuffer& operator=(GLBuffer const& other) = delete;
	/**
	 * @brief Returns the number of allocated OpenGL textures.
	 */
	static unsigned int getInstancesCount() { return instancesCount(); };

	GLBuffer(GLBuffer&& other)
	    : id(other.id)
	    , currentTarget(other.currentTarget)
	    , size(other.size)
	    , doClean(other.doClean)
	{
		// prevent other from cleaning shader if it destroys itself
		other.doClean = false;
	};

	explicit GLBuffer(GLenum target, size_t size = 0,
	                  GLenum usage = GL_STATIC_DRAW);
	GLenum getCurrentTarget() const { return currentTarget; };
	void setCurrentTarget(int currentTarget)
	{
		this->currentTarget = currentTarget;
	};
	// in bytes
	size_t getSize() const { return size; };
	void resize(size_t size, GLenum usage = GL_STATIC_DRAW);
	void bind() const;
	void bind(GLenum target); // resets currentTarget
	void unbind() const;
	void bindBase(unsigned int index) const;
	// size = number of elements of size sizeof(T)
	template <typename T>
	void setData(T const* data, size_t size, GLenum usage = GL_STATIC_DRAW);
	template <typename T>
	void setData(std::vector<T> const& data, GLenum usage = GL_STATIC_DRAW);
	// subSize and offset in elements of size sizeof(T)
	template <typename T>
	void setSubData(size_t offset, T const* data, size_t subSize);
	// offset in elements of size sizeof(T)
	template <typename T>
	void setSubData(size_t offset, std::vector<T> const& data);
	void* map(GLenum access) const;
	void* mapRange(size_t offset, size_t subSize, GLenum access) const;
	void unmap() const;
	virtual ~GLBuffer() { cleanUp(); };

  protected:
	/**
	 * @brief Frees the underlying OpenGL buffers.
	 */
	void cleanUp();

  private:
	GLuint id = 0;
	GLenum currentTarget;
	size_t size = 0;

	bool doClean = true;
	static unsigned int& instancesCount();

	static void glBufferData(GLenum target, size_t size, void const* data,
	                         GLenum usage);
	static void glBufferSubData(GLenum target, size_t offset, size_t size,
	                            void const* data);
};

template <typename T>
void GLBuffer::setData(T const* data, size_t size, GLenum usage)
{
	if(this->size * sizeof(T) == size && size != 0)
	{
		// optimize, don't reallocate
		setSubData(0, data, size);
		return;
	}
	bind();
	glBufferData(currentTarget, size * sizeof(T), data, usage);
	this->size = size * sizeof(T);
}

template <typename T>
void GLBuffer::setData(std::vector<T> const& data, GLenum usage)
{
	setData(&data[0], data.size(), usage);
}

template <typename T>
void GLBuffer::setSubData(size_t offset, T const* data, size_t subSize)
{
	if(this->size == 0)
	{
		if(data != nullptr)
		{
			// not optimal but user messed up anyway
			std::vector<T> newData(offset);
			for(size_t i(0); i < subSize; ++i)
			{
				newData.push_back(data[i]);
			}
			setData(newData);
			return;
		}
		setData(data, offset + subSize);
		return;
	}
	bind();
	glBufferSubData(currentTarget, offset * sizeof(T), subSize * sizeof(T),
	                data);
}

template <typename T>
void GLBuffer::setSubData(size_t offset, std::vector<T> const& data)
{
	setSubData(offset, &data[0], data.size());
}

#endif // GLBUFFER_HPP
