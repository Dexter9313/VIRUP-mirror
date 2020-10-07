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

#ifndef TEXT3D_H
#define TEXT3D_H

#include <QFontDatabase>
#include <QImage>
#include <QPainter>

#include "GLHandler.hpp"
#include "Primitives.hpp"

class Text3D
{
  public:
	Text3D(unsigned int width, unsigned int height);
	// takes ownership of shader
	Text3D(unsigned int width, unsigned int height,
	       GLHandler::ShaderProgram const& shader);
	QMatrix4x4 const& getModel() const { return model; };
	QMatrix4x4& getModel() { return model; };
	QImage const getImage() const { return image; };
	QString getText() const { return text; };
	QColor getColor() const { return color; };
	float getAlpha() const { return alpha; };
	QFont getFont() const { return font; };
	QColor getBackgroundColor() const { return backgroundColor; };
	QRect getRectangle() const { return rectangle; };
	float getSuperSampling() const { return superSampling; };
	int getFlags() const { return flags; };
	GLHandler::ShaderProgram getShader() { return shader; };
	void setText(QString const& text);
	void setColor(QColor const& color);
	void setAlpha(float alpha);
	void setFont(QFont const& font);
	void setBackgroundColor(QColor const& backgroundColor);
	void setRectangle(QRect const& rectangle);
	void setFlags(int flags);
	void setSuperSampling(float superSampling);
	void render(GLHandler::GeometricSpace geometricSpace
	            = GLHandler::GeometricSpace::WORLD);
	~Text3D();

	static QRect
	    paintText(QImage& image, QString const& text, QColor const& color,
	              QFont const& font
	              = QFontDatabase::systemFont(QFontDatabase::GeneralFont),
	              QColor const& backgroundColor = QColor(0, 0, 0, 0),
	              QRect const& rectangle = QRect(), int flags = Qt::AlignLeft);

  private:
	void updateTex();

	GLHandler::ShaderProgram shader;
	GLHandler::Mesh quad   = Primitives::newQuad(shader);
	GLHandler::Texture tex = GLHandler::newTexture(0, 0, nullptr);

	QMatrix4x4 model;

	QMatrix4x4 aspectratio;

	QSize originalSize;
	QImage image;

	QString text = "";
	QColor color = QColor(0, 0, 0, 255);
	float alpha  = 1.f;
	QFont font   = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
	QColor backgroundColor = QColor(0, 0, 0, 0);
	QRect rectangle        = QRect();
	int flags              = Qt::AlignLeft;

	float superSampling = 1.f;
};

#endif // TEXT3D_H
