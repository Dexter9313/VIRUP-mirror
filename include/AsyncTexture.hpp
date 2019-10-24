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
#include <QThread>
#include <cstring>

#include "GLHandler.hpp"

// TODO(florian) when Qt 5.10 is available, use QThread::create
namespace at
{
class WorkerThread : public QThread
{
	Q_OBJECT
  public:
	WorkerThread(QString const& path, unsigned char* data)
	    : path(path)
	    , data(data)
	{
	}

	WorkerThread(QString const& path, unsigned char* data, unsigned int width,
	             unsigned int height)
	    : path(path)
	    , data(data)
	    , resize(true)
	    , width(width)
	    , height(height)
	{
	}

  private:
	QString path;
	unsigned char* data;
	bool resize         = false;
	unsigned int width  = 0;
	unsigned int height = 0;
	void run() override
	{
		QImageReader imReader(path);
		if(resize)
		{
			imReader.setScaledSize(QSize(width, height));
		}
		QImage img(imReader.read());
		if(img.isNull())
		{
			// NOLINTNEXTLINE(hicpp-no-array-decay)
			qWarning() << "Could not load Texture '" + path
			                  + "' : " + imReader.errorString();
			return;
		}
		img = img.convertToFormat(QImage::Format_RGBA8888);
		std::memcpy(data, img.bits(), std::size_t(img.byteCount()));
	}
};

} // namespace at

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

	static void garbageCollect(bool force = false);

  private:
	GLHandler::Texture defaultTex = {};
	GLHandler::Texture tex        = {};

	GLHandler::PixelBufferObject pbo = {};
	at::WorkerThread* thread;

	bool loaded    = false;
	bool emptyPath = false;
	bool sRGB;

	// never wait for futures to finish within destructor ! if you need to
	// release resources and the future didn't finish, push it here and other
	// AsyncTextures will take care of it later
	static QList<QPair<at::WorkerThread*, GLHandler::PixelBufferObject>>&
	    waitingForDeletion();
};

#endif // ASYNCTEXTURE_HPP
