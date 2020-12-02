/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@epfl.ch>

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

#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "BasicCamera.hpp"
#include "Primitives.hpp"
#include "gl/GLHandler.hpp"

class Billboard
{
  public:
	explicit Billboard(const char* texPath);
	explicit Billboard(QImage const& image);
	Billboard(const char* texPath, GLShaderProgram&& shader);
	Billboard(QImage const& image, GLShaderProgram&& shader);
	void render(BasicCamera const& camera);

	QVector3D position = QVector3D();
	float width        = 1.f;

  private:
	GLTexture tex;
	GLMesh quad;
	GLShaderProgram shader;
};

#endif // BILLBOARD_H
