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

#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "gl/GLHandler.hpp"

class Primitives
{
  public:
	Primitives() = delete;

	static void setAsQuad(GLMesh& mesh, GLShaderProgram const& shader,
	                      PrimitiveType primitiveType
	                      = PrimitiveType::TRIANGLE_STRIP);

	// dim 2
	static void setAsGrid(GLMesh& mesh, GLShaderProgram const& shader,
	                      unsigned int size = 1024,
	                      PrimitiveType primitiveType
	                      = PrimitiveType::TRIANGLE_STRIP);

	static void setAsUnitCube(GLMesh& mesh, GLShaderProgram const& shader,
	                          PrimitiveType primitiveType
	                          = PrimitiveType::TRIANGLE_STRIP);

	// TRIANGLE_STRIP NOT SUPPORTED !
	static void setAsUnitSphere(GLMesh& mesh, GLShaderProgram const& shader,
	                            unsigned int latDivisions = 10,
	                            unsigned int lonDivisions = 10,
	                            PrimitiveType primitiveType
	                            = PrimitiveType::TRIANGLES);
};

#endif // PRIMITIVES_H
