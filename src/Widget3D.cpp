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

#include "Widget3D.hpp"

Widget3D::Widget3D(QWidget* widget)
    : shader("billboard")
{
	Primitives::setAsQuad(quad, shader);
	setWidget(widget);
}

void Widget3D::setWidget(QWidget* widget)
{
	this->widget = widget;
	widget->setVisible(true);
	widget->setVisible(false);
	unsigned int width(widget->width()), height(widget->height());
	originalSize = widget->size();
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

void Widget3D::render()
{
	GLHandler::setUpRender(shader, model * aspectratio);
	GLHandler::useTextures({tex});
	quad.render(PrimitiveType::TRIANGLE_STRIP);
}

void Widget3D::updateTex()
{
	GLHandler::deleteTexture(tex);

	image = QImage(originalSize, QImage::Format_ARGB32);

	paintWidget(image, *widget);

	tex = GLHandler::newTexture(image);
}

Widget3D::~Widget3D()
{
	GLHandler::deleteTexture(tex);
}

void Widget3D::paintWidget(QImage& image, QWidget& widget)
{
	auto painter = new QPainter(&image);
	painter->setRenderHint(QPainter::Antialiasing);

	// image.fill(QColor(0, 0, 0, 0));
	widget.render(painter);

	// The QPainter doesn't like its QImage to be changed
	delete painter;
	image = image.mirrored(false, true);
}
