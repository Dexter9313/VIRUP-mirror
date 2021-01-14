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
	vbo = new GLBuffer(GL_ARRAY_BUFFER);
	ebo = new GLBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

void GLMesh::cleanUp()
{
	if(!doClean)
	{
		return;
	}
	--instancesCount();
	delete vbo;
	delete ebo;
	GLHandler::glf().glDeleteVertexArrays(1, &vao);
	doClean = false;
}

void GLMesh::setVertexShaderMapping(
    GLShaderProgram const& shaderProgram,
    std::vector<QPair<const char*, unsigned int>> const& mapping)
{
	GLHandler::glf().glBindVertexArray(vao);

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
			vbo->bind(); // binds the vbo to the vao attrib pointer
			GLHandler::glf().glVertexAttribPointer(
			    posAttrib, map.second, GL_FLOAT, GL_FALSE,
			    stride * sizeof(float),
			    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
			    reinterpret_cast<void*>(offset * sizeof(float)));
		}
		offset += map.second;
	}
	vertexSize = offset * sizeof(float);

	ebo->bind();
	GLHandler::glf().glBindVertexArray(0);
}

void GLMesh::setVertexShaderMapping(
    GLShaderProgram const& shaderProgram, QStringList const& mappingNames,
    std::vector<unsigned int> const& mappingSizes)
{
	std::vector<QPair<const char*, unsigned int>> mapping;
	for(unsigned int i(0); i < mappingSizes.size(); ++i)
	{
		mapping.emplace_back(mappingNames[i].toLatin1().constData(),
		                     mappingSizes[i]);
	}
	setVertexShaderMapping(shaderProgram, mapping);
}

void GLMesh::setVertices(float const* vertices, size_t vertSize)
{
	vbo->setData(vertices, vertSize);
}

void GLMesh::setVertices(float const* vertices, size_t vertSize,
                         unsigned int const* elements, size_t elemSize)
{
	vbo->setData(vertices, vertSize);
	ebo->setData(elements, elemSize);
}

void GLMesh::setVertices(std::vector<float> const& vertices)
{
	setVertices(&(vertices[0]), vertices.size());
}

void GLMesh::setVertices(std::vector<float> const& vertices,
                         std::vector<unsigned int> const& elements)
{
	setVertices(&(vertices[0]), vertices.size(), &(elements[0]),
	            elements.size());
}

void GLMesh::render(PrimitiveType primitiveType) const
{
	if(vertexSize == 0)
	{
		return;
	}
	if(primitiveType == PrimitiveType::AUTO)
	{
		primitiveType = (ebo->getSize() == 0) ? PrimitiveType::POINTS
		                                      : PrimitiveType::TRIANGLES;
	}

	GLHandler::glf().glBindVertexArray(vao);
	if(ebo->getSize() == 0)
	{
		GLHandler::glf().glDrawArrays(static_cast<GLenum>(primitiveType), 0,
		                              vbo->getSize() / vertexSize);
	}
	else
	{
		GLHandler::glf().glDrawElements(static_cast<GLenum>(primitiveType),
		                                ebo->getSize() / sizeof(unsigned int),
		                                GL_UNSIGNED_INT, nullptr);
	}
	GLHandler::glf().glBindVertexArray(0);
}
