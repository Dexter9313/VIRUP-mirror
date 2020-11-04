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

#include "Text3D.hpp"

Text3D::Text3D(unsigned int width, unsigned int height)
    : Text3D(width, height, GLShaderProgram("billboard"))
{
}

Text3D::Text3D(unsigned int width, unsigned int height,
               GLShaderProgram&& shader)
    : shader(std::move(shader))
    , originalSize(width, height)
{
	Primitives::setAsQuad(quad, shader);
	if(width > height)
	{
		aspectratio.scale(1.f, static_cast<float>(height) / width);
	}
	else
	{
		aspectratio.scale(static_cast<float>(width) / height, 1.f);
	}
	updateTex();
}

void Text3D::setText(QString const& text)
{
	this->text = text;
	updateTex();
}

void Text3D::setColor(QColor const& color)
{
	this->color = color;
	updateTex();
}

void Text3D::setAlpha(float alpha)
{
	this->alpha = alpha;
	shader.setUniform("alpha", alpha);
}

void Text3D::setFont(QFont const& font)
{
	this->font = font;
	updateTex();
}

void Text3D::setBackgroundColor(QColor const& backgroundColor)
{
	this->backgroundColor = backgroundColor;
	updateTex();
}

void Text3D::setRectangle(QRect const& rectangle)
{
	this->rectangle = rectangle;
	updateTex();
}

void Text3D::setFlags(int flags)
{
	this->flags = flags;
	updateTex();
}

void Text3D::setSuperSampling(float superSampling)
{
	this->superSampling = superSampling;
	updateTex();
}

void Text3D::render(GLHandler::GeometricSpace geometricSpace)
{
	if(alpha < 0.01)
	{
		return;
	}

	GLHandler::beginTransparent();
	GLHandler::setUpRender(shader, model * aspectratio, geometricSpace);
	GLHandler::useTextures({tex});
	quad.render(PrimitiveType::TRIANGLE_STRIP);
	GLHandler::endTransparent();
}

void Text3D::updateTex()
{
	GLHandler::deleteTexture(tex);

	image = QImage(superSampling * originalSize, QImage::Format_RGBA8888);

	bool sizeInPixels(true);
	int fontSize(font.pixelSize());
	if(fontSize == -1)
	{
		sizeInPixels = false;
		fontSize     = font.pointSize();
	}

	if(sizeInPixels)
	{
		font.setPixelSize(static_cast<int>(superSampling * fontSize));
	}
	else
	{
		font.setPointSize(static_cast<int>(superSampling * fontSize));
	}

	QRect adjustedRect(static_cast<int>(superSampling * rectangle.x()),
	                   static_cast<int>(superSampling * rectangle.y()),
	                   static_cast<int>(superSampling * rectangle.width()),
	                   static_cast<int>(superSampling * rectangle.height()));

	paintText(image, text, color, font, backgroundColor, adjustedRect, flags);

	if(sizeInPixels)
	{
		font.setPixelSize(fontSize);
	}
	else
	{
		font.setPointSize(fontSize);
	}

	tex = GLHandler::newTexture(image);
}

Text3D::~Text3D()
{
	GLHandler::deleteTexture(tex);
}

QRect Text3D::paintText(QImage& image, QString const& text, QColor const& color,
                        QFont const& font, QColor const& backgroundColor,
                        QRect const& rectangle, int flags)
{
	QRect boundingRect;

	auto painter = new QPainter(&image);
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setRenderHint(QPainter::TextAntialiasing);

	image.fill(backgroundColor);

	painter->setFont(font);
	painter->setPen(color);
	painter->drawText(rectangle.isNull() ? image.rect() : rectangle, flags,
	                  text, &boundingRect);

	// The QPainter doesn't like its QImage to be changed
	delete painter;
	image = image.mirrored(false, true);

	return boundingRect;
}
