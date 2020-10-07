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

#include "AsyncTexture.hpp"

QList<QPair<at::WorkerThread*, GLHandler::PixelBufferObject>>&
    AsyncTexture::waitingForDeletion()
{
	static QList<QPair<at::WorkerThread*, GLHandler::PixelBufferObject>>
	    waitingForDeletion = {};
	return waitingForDeletion;
}

AsyncTexture::AsyncTexture(QString const& path, QColor const& defaultColor,
                           bool sRGB)
    : sRGB(sRGB)
    , averageColor(defaultColor)
{
	unsigned char color[4];
	color[0]   = defaultColor.red();
	color[1]   = defaultColor.green();
	color[2]   = defaultColor.blue();
	color[3]   = defaultColor.alpha();
	defaultTex = GLHandler::newTexture(1, 1, &(color[0]), sRGB);

	if(path.isEmpty())
	{
		emptyPath = true;
		return;
	}

	QImageReader imReader(path);
	QSize size(imReader.size());

	pbo = GLHandler::newPixelBufferObject(size.width(), size.height());
	unsigned char* data(pbo.mappedData);

	thread = new at::WorkerThread(path, data);
	thread->start();
}

AsyncTexture::AsyncTexture(QString const& path, unsigned int width,
                           unsigned int height, QColor const& defaultColor,
                           bool sRGB)
    : sRGB(sRGB)
    , averageColor(defaultColor)
{
	unsigned char color[4];
	color[0]   = defaultColor.red();
	color[1]   = defaultColor.green();
	color[2]   = defaultColor.blue();
	color[3]   = defaultColor.alpha();
	defaultTex = GLHandler::newTexture(1, 1, &(color[0]), sRGB);

	if(path.isEmpty())
	{
		emptyPath = true;
		return;
	}

	pbo = GLHandler::newPixelBufferObject(width, height);
	unsigned char* data(pbo.mappedData);

	thread = new at::WorkerThread(path, data, width, height);
	thread->start();
}

GLHandler::Texture AsyncTexture::getTexture()
{
	if(emptyPath)
	{
		return defaultTex;
	}

	if(loaded)
	{
		return tex;
	}

	if(!thread->isFinished())
	{
		return defaultTex;
	}

	tex = GLHandler::copyPBOToTex(pbo, sRGB);
	GLHandler::deletePixelBufferObject(pbo);
	GLHandler::generateMipmap(tex);
	unsigned int lastMipmap(GLHandler::getHighestMipmapLevel(tex));
	averageColor
	    = GLHandler::getTextureContentAsImage(tex, lastMipmap).pixelColor(0, 0);
	delete thread;
	loaded = true;

	return tex;
}

AsyncTexture::~AsyncTexture()
{
	GLHandler::deleteTexture(defaultTex);
	if(!emptyPath)
	{
		if(loaded)
		{
			GLHandler::deleteTexture(tex);
		}
		else if(thread->isFinished())
		{
			GLHandler::deletePixelBufferObject(pbo);
			delete thread;
		}
		else
		{
			thread->setPriority(QThread::LowestPriority);
			waitingForDeletion().push_back({thread, pbo});
		}
	}
}

void AsyncTexture::garbageCollect(bool force)
{
	// go in reverse because of possible deletions
	for(int i(waitingForDeletion().size() - 1); i >= 0; --i)
	{
		if(force)
		{
			waitingForDeletion()[i].first->wait();
		}
		if(waitingForDeletion()[i].first->isFinished())
		{
			GLHandler::deletePixelBufferObject(waitingForDeletion()[i].second);
			delete waitingForDeletion()[i].first;
			waitingForDeletion().removeAt(i);
		}
	}
}
