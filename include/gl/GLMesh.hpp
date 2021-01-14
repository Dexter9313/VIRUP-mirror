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

#ifndef GLMESH_HPP
#define GLMESH_HPP

#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_2_Core>

#include "PythonQtHandler.hpp"

#include "GLShaderProgram.hpp"

class GLBuffer;
class GLHandler;

/**
 * @brief Represents a type of primitive for rasterization.
 *
 * AUTO is set to POINTS if no elements are given when setting the mesh's
 * vertices, and TRIANGLES otherwise.
 *
 * See <a href="https://www.khronos.org/opengl/wiki/Primitive">OpenGL
 * Primitive</a>.
 */
enum class PrimitiveType
{
	POINTS         = GL_POINTS,
	LINES          = GL_LINES,
	LINE_STRIP     = GL_LINE_STRIP,
	LINE_LOOP      = GL_LINE_LOOP,
	TRIANGLES      = GL_TRIANGLES,
	TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
	AUTO // if no ebo, POINTS, else TRIANGLES
};

/** @ingroup pywrap
 * @brief Represents a triplet of an OpenGL's Vertex Array Object (VAO), Vertex
 * Buffer Object (VBO) and Element Array Buffer (or Element Buffer Object EBO).
 *
 * A mesh is a set of vertices that can be rendered by the GPU. These
 * vertices can be described as positions or by more than that (colors,
 * normals, etc...).
 *
 * It doesn't handle model matrix or shaders, it is a very basic
 * vertex data storage class.
 */
class GLMesh
{
  public:
	// implement those in protected if and only if they're needed for the Python
	// API
	GLMesh(GLMesh const& other) = delete;
	GLMesh& operator=(GLMesh const& other) = delete;
	/**
	 * @brief Returns the number of allocated OpenGL meshes .
	 */
	static unsigned int getInstancesCount() { return instancesCount(); };

	GLMesh(GLMesh&& other)
	    : vao(other.vao)
	    , vbo(other.vbo)
	    , ebo(other.ebo)
	    , vertexSize(other.vertexSize)
	    , doClean(other.doClean)
	{
		// prevent other from cleaning shader if it destroys itself
		other.doClean = false;
	};
	/**
	 * @brief Allocates a new @ref Mesh.
	 */
	GLMesh();
	GLBuffer& getVBO() { return *vbo; };
	GLBuffer& getEBO() { return *ebo; };
	void setVertexShaderMapping(
	    GLShaderProgram const& shaderProgram,
	    std::vector<QPair<const char*, unsigned int>> const& mapping);
	/**
	 * @brief Convenient version of the @ref setVertices(GLHandler::Mesh&,
	 * std::vector<float>const&, GLShaderProgram const&, std::vector<QPair<const
	 * char*, unsigned int>>const&, std::vector<unsigned int>const&) method to
	 * be used in Python.
	 *
	 * Behaves the same way as its other version, but the attribute mapping is
	 * specified differently, as it is harder to construct a QVector<QPair>
	 * object in Python. Instead of an array of pairs (name, size), the
	 * mapping is specified by all the ordered names in the @p mappingNames
	 * parameter and all their corresponding sizes in the same order in the @p
	 * mappingSizes parameter.
	 */
	void setVertexShaderMapping(GLShaderProgram const& shaderProgram,
	                            QStringList const& mappingNames,
	                            std::vector<unsigned int> const& mappingSizes);
	// doesn't work in PythonQt
	void setVertices(float const* vertices, size_t vertSize);
	void setVertices(float const* vertices, size_t vertSize,
	                 unsigned int const* elements, size_t elemSize);
	// doesn't work in PythonQt
	/**
	 * @brief Sets vertices data for a mesh.
	 *
	 * The vertices are specified in the "array of structures" order, "structure
	 * of arrays" isn't supported as of this version.
	 *
	 * @param mesh @ref Mesh for which the vertices are set.
	 *
	 * @param vertices Vertices data. Can be any set of floating point numbers.
	 * Usually, it will at least contain tridimentional positional information.
	 * The data must be linearized. For example, if a vertex must have two
	 * attributes, a 3D position p and a 2D texture coordinate tc, one vertex
	 * will be represented by the 5 values { p0, p1, p2, tc0, tc1 }. Vertices
	 * are stored in a contiguous way : { vertex 0, vertex 1, ... } or more
	 * precisely {p0,0, p0,1, p0,2, tc0,0, tc0,1, p1,0, p1,1, p1,2, tc1,0,
	 * tc1,1} to follow the previous example (where pi,j is j-th component of
	 * the ith vertex position and tci,j is the j-th component of the ith vertex
	 * texture coordinates). Therefore @p vertices is a 1D array of size (number
	 * of vertices) * (sum of the dimensions of one vertex attributes), in our
	 * example (number of vertices) * 5.
	 *
	 * @param shaderProgram Which shader program will be used to render the
	 * mesh. This is used to get the attributes locations to set in the Vertex
	 * Array Object.
	 *
	 * @param mapping An array of ordered pairs which informs on how to map
	 * attributes from the @p vertices parameter to the @p shaderProgram inputs.
	 * Each pair's first element is the name of the attribute as stated in the
	 * @p shaderProgram and its second element is the dimension of this
	 * attribute. For the earlier example given, @p mapping should be set as
	 * {{"position", 3}, {"texcoord", 2}}, if @p shaderProgram's vertex array
	 * contains the two inputs <code>in vec3 position;</code> and <code>in vec2
	 * texcoord;</code> and positions are specified in @p vertices before
	 * texture coordiantes are (which is the case - {p0, tc0, p1, tc1, ...}).
	 *
	 * @param elements Optional. Content of the element buffer object that will
	 * specify vertices rendering order. If none is provided, the order of @p
	 * vertices will be followed. More technically, glDrawArrays will be called
	 * if this parameter is empty, and glDrawElements will be called otherwise.
	 * Also, if this parameter is empty, the automatic @ref PrimitiveType for
	 * the mesh will be POINTS.
	 */
	void setVertices(std::vector<float> const& vertices);
	void setVertices(std::vector<float> const& vertices,
	                 std::vector<unsigned int> const& elements);
	/**
	 * @brief Draws a mesh on the current render target.
	 *
	 * @attention Make sure you called @ref setUpRender accordingly before
	 * calling this method.
	 *
	 * @attention This rendering will use the last @ref GLShaderProgram passed
	 * to
	 * @ref setUpRender to draw the mesh. You can override the used shader
	 * program by usinga @ref useShader. Just make sure the shader you want to
	 * use has already been passed to @ref setUpRender with the correct
	 * parameters before.
	 *
	 * @param mesh @ref Mesh to be drawn.
	 * @param primitiveType @ref PrimitiveType of the mesh. If AUTO and the mesh
	 * doesn't have elements, POINTS will be assumed, but if the mesh has
	 * elements, TRIANGLES will be assumed.
	 */
	void render(PrimitiveType primitiveType = PrimitiveType::AUTO) const;
	virtual ~GLMesh() { cleanUp(); };

  protected:
	/**
	 * @brief Frees the underlying OpenGL buffers.
	 */
	void cleanUp();

  private:
	GLuint vao              = 0;
	GLBuffer* vbo           = 0;
	GLBuffer* ebo           = 0;
	unsigned int vertexSize = 0; // in bytes

	bool doClean = true;
	static unsigned int& instancesCount();
};

// <PythonQt>
class GLMeshWrapper : public PythonQtWrapper
{
	Q_OBJECT
  public:
	virtual const char* wrappedClassName() const override { return "GLMesh"; }
	virtual const char* wrappedClassPackage() const override { return "GL"; }

  public Q_SLOTS:
	GLMesh* new_GLMesh() { return new GLMesh; }
	unsigned int static_GLMesh_getInstancesCount()
	{
		return GLMesh::getInstancesCount();
	};
	void setVertexShaderMapping(GLMesh* m, GLShaderProgram const& shaderProgram,
	                            QStringList const& mappingNames,
	                            std::vector<unsigned int> const& mappingSizes)
	{
		m->setVertexShaderMapping(shaderProgram, mappingNames, mappingSizes);
	}
	void setVertices(GLMesh* m, std::vector<float> const& vertices,
	                 std::vector<unsigned int> const& elements = {})
	{
		m->setVertices(vertices, elements);
	}
	void render(GLMesh* m, PrimitiveType t = PrimitiveType::AUTO)
	{
		m->render(t);
	}
	void delete_GLMesh(GLMesh* v) { delete v; }

	// access methods
};

// To use PrimitiveType with <PythonQt>
class PyPrimitiveType : public QObject
{
	Q_OBJECT
  public:
	enum PrimitiveType
	{
		POINTS         = GL_POINTS,
		LINES          = GL_LINES,
		LINE_STRIP     = GL_LINE_STRIP,
		LINE_LOOP      = GL_LINE_LOOP,
		TRIANGLES      = GL_TRIANGLES,
		TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
		AUTO // if no ebo, POINTS, else TRIANGLES
	};
	Q_ENUM(PrimitiveType);
};
// </PythonQt>

#endif // GLMESH_HPP
