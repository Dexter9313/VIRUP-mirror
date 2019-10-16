/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@hotmail.fr>

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

#ifndef ASYNCTEXTURE_HPP
#define ASYNCTEXTURE_HPP

#include <QImageReader>
#include <QtConcurrent>
#include <cstring>

#include "GLHandler.hpp"

class AsyncTexture
{
  public:
	AsyncTexture(QString const& path, QColor const& defaultColor,
	             bool sRGB = true);
	// will override the file texture size to load more (interpolate) or less
	// pixels
	AsyncTexture(QString const& path, unsigned int width, unsigned int height,
	             QColor const& defaultColor, bool sRGB = true);
	bool isLoaded() const { return loaded; };
	GLHandler::Texture getDefaultTexture() const { return defaultTex; };
	GLHandler::Texture getTexture();
	~AsyncTexture();

  private:
	GLHandler::Texture defaultTex = {};
	GLHandler::Texture tex        = {};

	GLHandler::PixelBufferObject pbo = {};
	QFuture<void> future;

	bool loaded    = false;
	bool emptyPath = false;
	bool sRGB;
};

#endif // ASYNCTEXTURE_HPP
