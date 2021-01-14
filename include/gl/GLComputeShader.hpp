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

/*
 * defines accessible from shader :
 * LOCAL_SIZE_1D_X : optimal local group size x component if using a 1D layout
 * LOCAL_SIZE_2D_X : optimal local group size x component if using a 2D layout
 * LOCAL_SIZE_2D_Y : optimal local group size y component if using a 2D layout
 * LOCAL_SIZE_3D_X : optimal local group size x component if using a 3D layout
 * LOCAL_SIZE_3D_Y : optimal local group size y component if using a 3D layout
 * LOCAL_SIZE_3D_Z : optimal local group size z component if using a 3D layout
 */
class GLComputeShader : public GLShaderProgram
{
  public:
	enum DataAccessMode
	{
		R  = GL_READ_ONLY,
		W  = GL_WRITE_ONLY,
		RW = GL_READ_WRITE
	};

	explicit GLComputeShader(QString const& fileName,
	                         QMap<QString, QString> const& defines = {});
	// /!\ if globalGroupSize is not a multiple of the shader's local group
	// size, the immediatly higher multiple of the
	// shader's local group size will be used as globalGroupSize ; do the
	// appropriate checks in the shader if needed
	void exec(std::vector<std::pair<GLTexture const*, DataAccessMode>> const&
	              textures,
	          std::array<unsigned int, 3> const& globalGroupSize,
	          bool waitForFinish = true) const;

	static QMap<QString, QString>
	    addDefines(QMap<QString, QString> const& userDefines);

  private:
	std::array<GLint, 3> workGroupSize = {};
};

#endif // GLCOMPUTESHADER_HPP
