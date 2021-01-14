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

	buff       = new GLBuffer(GL_PIXEL_UNPACK_BUFFER,
                        size.width() * size.height() * 4, GL_STREAM_DRAW);
	mappedData = static_cast<unsigned char*>(buff->map(GL_WRITE_ONLY));
	buff->unbind();
}

GLTexture* GLPixelBufferObject::copyContentToNewTex(bool sRGB) const
{
	buff->unmap();
	buff->bind(); // be sure it is bound before the call to glTexImage2D
	// NOLINTNEXTLINE(hicpp-use-nullptr, modernize-use-nullptr)
	auto result = new GLTexture(
	    GLTexture::Tex2DProperties(size.width(), size.height(), sRGB));
	buff->unbind();

	return result;
}

void GLPixelBufferObject::cleanUp()
{
	if(!doClean)
	{
		return;
	}
	--instancesCount();
	buff->unbind();
	delete buff;
	doClean = false;
}
