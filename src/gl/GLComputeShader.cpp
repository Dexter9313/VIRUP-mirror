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

#include "gl/GLHandler.hpp"

#include "gl/GLComputeShader.hpp"

GLComputeShader::GLComputeShader(QString const& name,
                                 QMap<QString, QString> const& defines)
    : GLShaderProgram({{name, Stage::COMPUTE}}, defines)
{
}

void GLComputeShader::exec(
    std::vector<std::pair<GLTexture const*, DataAccessMode>> const& textures,
    std::array<unsigned int, 3> const& workGroupSize, bool waitForFinish) const
{
	use();

	for(unsigned int i(0); i < textures.size(); ++i)
	{
		textures[i].first->use(GL_TEXTURE0 + i);

		GLint format;
		GLHandler::glf().glGetTexLevelParameteriv(
		    textures[i].first->getGLTarget(), 0, GL_TEXTURE_INTERNAL_FORMAT,
		    &format);
		GLHandler::glf().glBindImageTexture(
		    i, textures[i].first->getGLTexture(), 0, GL_FALSE, 0,
		    textures[i].second, format);
	}
	GLHandler::glf_ARB_compute_shader().glDispatchCompute(
	    workGroupSize[0], workGroupSize[1], workGroupSize[2]);

	if(waitForFinish)
	{
		// add GL_SHADER_STORAGE_BARRIER_BIT for SSBO later
		GLHandler::glf().glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}
}
