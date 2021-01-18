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

#include "gl/GLBuffer.hpp"

unsigned int& GLBuffer::instancesCount()
{
	static unsigned int instancesCount = 0;
	return instancesCount;
}

GLBuffer::GLBuffer(GLenum target, size_t size, GLenum usage)
    : currentTarget(target)
{
	++instancesCount();

	GLHandler::glf().glGenBuffers(1, &id);
	if(size != 0)
	{
		resize(size, usage);
	}
}

void GLBuffer::resize(size_t size, GLenum usage)
{
	setData(static_cast<char*>(nullptr), size, usage);
}

void GLBuffer::bind() const
{
	GLHandler::glf().glBindBuffer(currentTarget, id);
}

void GLBuffer::bind(GLenum target)
{
	currentTarget = target;
	bind();
}

void GLBuffer::unbind() const
{
	GLHandler::glf().glBindBuffer(currentTarget, 0);
}

void GLBuffer::bindBase(unsigned int index) const
{
	GLHandler::glf().glBindBufferBase(currentTarget, index, id);
}

void* GLBuffer::map(GLenum access) const
{
	bind();
	return GLHandler::glf().glMapBuffer(currentTarget, access);
}

void* GLBuffer::mapRange(size_t offset, size_t subSize, GLenum access) const
{
	bind();
	return GLHandler::glf().glMapBufferRange(currentTarget, offset, subSize,
	                                         access);
}

void GLBuffer::unmap() const
{
	bind();
	GLHandler::glf().glUnmapBuffer(currentTarget);
}

void GLBuffer::cleanUp()
{
	if(!doClean)
	{
		return;
	}
	--instancesCount();
	GLHandler::glf().glDeleteBuffers(1, &id);
	doClean = false;
}

void GLBuffer::glBufferData(GLenum target, size_t size, void const* data,
                            GLenum usage)
{
	GLHandler::glf().glBufferData(target, size, data, usage);
}

void GLBuffer::glBufferSubData(GLenum target, size_t offset, size_t size,
                               void const* data)
{
	GLHandler::glf().glBufferSubData(target, offset, size, data);
}
