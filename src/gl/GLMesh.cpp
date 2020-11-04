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

#include "gl/GLMesh.hpp"

unsigned int& GLMesh::instancesCount()
{
	static unsigned int instancesCount = 0;
	return instancesCount;
}

GLMesh::GLMesh()
{
	++instancesCount();
	GLHandler::glf().glGenVertexArrays(1, &vao);
	GLHandler::glf().glGenBuffers(1, &vbo);
	GLHandler::glf().glBindVertexArray(vao);
	GLHandler::glf().glBindBuffer(GL_ARRAY_BUFFER, vbo);
	GLHandler::glf().glGenBuffers(1, &ebo);
	GLHandler::glf().glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	GLHandler::glf().glBindVertexArray(0);
}

void GLMesh::cleanUp()
{
	if(!doClean)
	{
		return;
	}
	--instancesCount();
	GLHandler::glf().glDeleteBuffers(1, &vbo);
	GLHandler::glf().glDeleteBuffers(1, &ebo);
	GLHandler::glf().glDeleteVertexArrays(1, &vao);
	doClean = false;
}

void GLMesh::setVertices(
    float const* vertices, size_t size, GLShaderProgram const& shaderProgram,
    std::vector<QPair<const char*, unsigned int>> const& mapping,
    std::vector<unsigned int> const& elements)
{
	GLHandler::glf().glBindVertexArray(vao);
	GLHandler::glf().glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// put data in buffer (it is now sent to graphics card)
	GLHandler::glf().glBufferData(GL_ARRAY_BUFFER, size * sizeof(vertices[0]),
	                              vertices, GL_STATIC_DRAW);

	size_t offset = 0, stride = 0;
	for(auto map : mapping)
	{
		stride += map.second;
	}
	for(auto map : mapping)
	{
		// map position
		GLint posAttrib = shaderProgram.getAttribLocationFromName(map.first);
		if(posAttrib != -1)
		{
			GLHandler::glf().glEnableVertexAttribArray(posAttrib);
			GLHandler::glf().glVertexAttribPointer(
			    posAttrib, map.second, GL_FLOAT, GL_FALSE,
			    stride * sizeof(float),
			    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
			    reinterpret_cast<void*>(offset * sizeof(float)));
		}
		offset += map.second;
	}
	if(offset != 0)
	{
		vboSize = size / offset;
	}
	if(!elements.empty())
	{
		GLHandler::glf().glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		GLHandler::glf().glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		                              elements.size() * sizeof(elements[0]),
		                              &(elements[0]), GL_STATIC_DRAW);
		eboSize = elements.size();
	}

	GLHandler::glf().glBindVertexArray(0);
}

void GLMesh::setVertices(
    std::vector<float> const& vertices, GLShaderProgram const& shaderProgram,
    std::vector<QPair<const char*, unsigned int>> const& mapping,
    std::vector<unsigned int> const& elements)
{
	setVertices(&(vertices[0]), vertices.size(), shaderProgram, mapping,
	            elements);
}

void GLMesh::setVertices(std::vector<float> const& vertices,
                         GLShaderProgram const& shaderProgram,
                         QStringList const& mappingNames,
                         std::vector<unsigned int> const& mappingSizes,
                         std::vector<unsigned int> const& elements)
{
	std::vector<QPair<const char*, unsigned int>> mapping;
	for(unsigned int i(0); i < mappingSizes.size(); ++i)
	{
		mapping.emplace_back(mappingNames[i].toLatin1().constData(),
		                     mappingSizes[i]);
	}
	setVertices(vertices, shaderProgram, mapping, elements);
}

void GLMesh::updateVertices(float const* vertices, size_t size) const
{
	GLHandler::glf().glBindVertexArray(vao);
	GLHandler::glf().glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// put data in buffer (it is now sent to graphics card)
	GLHandler::glf().glBufferData(GL_ARRAY_BUFFER, size * sizeof(vertices[0]),
	                              vertices, GL_DYNAMIC_DRAW);
	GLHandler::glf().glBindVertexArray(0);
}

void GLMesh::updateVertices(std::vector<float> const& vertices) const
{
	updateVertices(&(vertices[0]), vertices.size());
}

void GLMesh::render(PrimitiveType primitiveType) const
{
	if(primitiveType == PrimitiveType::AUTO)
	{
		primitiveType
		    = (eboSize == 0) ? PrimitiveType::POINTS : PrimitiveType::TRIANGLES;
	}

	GLHandler::glf().glBindVertexArray(vao);
	if(eboSize == 0)
	{
		GLHandler::glf().glDrawArrays(static_cast<GLenum>(primitiveType), 0,
		                              vboSize);
	}
	else
	{
		GLHandler::glf().glDrawElements(static_cast<GLenum>(primitiveType),
		                                eboSize, GL_UNSIGNED_INT, nullptr);
	}
	GLHandler::glf().glBindVertexArray(0);
}
