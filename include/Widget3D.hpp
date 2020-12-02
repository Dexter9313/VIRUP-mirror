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

#ifndef WIDGET3D_H
#define WIDGET3D_H

#include <QImage>
#include <QPainter>
#include <QWidget>

#include "Primitives.hpp"
#include "gl/GLHandler.hpp"

class Widget3D
{
  public:
	explicit Widget3D(QWidget* widget);
	QMatrix4x4 const& getModel() const { return model; };
	QMatrix4x4& getModel() { return model; };
	QImage const getImage() const { return image; };
	QWidget& getWidget() const { return *widget; };
	void setWidget(QWidget* widget);
	void render();
	~Widget3D();

	static void paintWidget(QImage& image, QWidget& widget);

  private:
	void updateTex();

	GLShaderProgram shader;
	GLMesh quad;
	GLTexture* tex = nullptr;

	QMatrix4x4 model;

	QMatrix4x4 aspectratio;

	QSize originalSize;
	QImage image;

	QWidget* widget = nullptr;
};

#endif // WIDGET3D_H
