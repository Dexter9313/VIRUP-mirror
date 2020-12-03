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

#include "gl/GLPixelBufferObject.hpp"

unsigned int& GLPixelBufferObject::instancesCount()
{
	static unsigned int instancesCount = 0;
	return instancesCount;
}

GLPixelBufferObject::GLPixelBufferObject(QSize const& size)
    : size(size)
{
	++instancesCount();

	GLHandler::glf().glGenBuffers(1, &id);
	GLHandler::glf().glBindBuffer(GL_PIXEL_UNPACK_BUFFER, id);
	GLHandler::glf().glBufferData(GL_PIXEL_UNPACK_BUFFER,
	                              size.width() * size.height() * 4, nullptr,
	                              GL_STREAM_DRAW);
	mappedData = static_cast<unsigned char*>(
	    GLHandler::glf().glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY));
	GLHandler::glf().glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

GLTexture* GLPixelBufferObject::copyContentToNewTex(bool sRGB) const
{
	GLHandler::glf().glBindBuffer(GL_PIXEL_UNPACK_BUFFER, id);
	GLHandler::glf().glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	// NOLINTNEXTLINE(hicpp-use-nullptr, modernize-use-nullptr)
	auto result = new GLTexture(
	    GLTexture::Tex2DProperties(size.width(), size.height(), sRGB), {},
	    {static_cast<GLvoid*>(nullptr)});
	GLHandler::glf().glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	return result;
}

void GLPixelBufferObject::cleanUp()
{
	if(!doClean)
	{
		return;
	}
	--instancesCount();
	GLHandler::glf().glDeleteBuffers(1, &id);
	GLHandler::glf().glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	doClean = false;
}
