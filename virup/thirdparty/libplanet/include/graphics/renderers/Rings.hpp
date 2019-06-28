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

#ifndef RINGS_H
#define RINGS_H

#include "GLHandler.hpp"

class Rings
{
  public:
	Rings(float seed, float innerRing, float outerRing, float planetradius,
	      QVector3D const& planetoblateness, QString const& texturePath = "");
	GLHandler::Texture getTexture();
	void update(float seed);
	void render(QMatrix4x4 const& model, QVector3D const& lightpos,
	            float lightradius, QColor const& lightcolor,
	            std::array<QVector4D, 5> const& neighborsPosRadius,
	            std::array<QVector3D, 5> const& neighborsOblateness,
	            QMatrix4x4 const& properRotation);
	~Rings();

  private:
	GLHandler::ShaderProgram shader = {};
	GLHandler::Mesh mesh            = {};

	bool textured                     = false;
	GLHandler::Texture tex            = {};
	GLHandler::RenderTarget texTarget = {};
};

#endif // RINGS_H
