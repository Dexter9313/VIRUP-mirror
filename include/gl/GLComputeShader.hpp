/*
    Copyright (C) 2020 Florian Cabot <florian.cabot@hotmail.fr>

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

#ifndef GLCOMPUTESHADER_HPP
#define GLCOMPUTESHADER_HPP

#include "GLShaderProgram.hpp"
#include "GLTexture.hpp"

class GLComputeShader : public GLShaderProgram
{
  public:
	enum DataAccessMode
	{
		R  = GL_READ_ONLY,
		W  = GL_WRITE_ONLY,
		RW = GL_READ_WRITE
	};

	GLComputeShader(QString const& name,
	                QMap<QString, QString> const& defines = {});
	// waitForFinish is non-blocking for the CPU but is for the GPU (to be
	// confirmed !)
	void exec(std::vector<std::pair<GLTexture const*, DataAccessMode>> const&
	              textures,
	          std::array<unsigned int, 3> const& workGroupSize,
	          bool waitForFinish = true) const;
};

#endif // GLCOMPUTESHADER_HPP
