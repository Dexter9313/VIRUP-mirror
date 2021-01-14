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

#ifndef GLSHADERPROGRAM_HPP
#define GLSHADERPROGRAM_HPP

#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_2_Core>

#include "PythonQtHandler.hpp"

class GLHandler;
/** @ingroup pywrap
 * @brief Represents an OpenGL Shader Program.
 *
 * Available in python scripts within package PythonQt.GL .
 *
 * A valid shader program is at least composed out of a vertex shader and a
 * fragment shader.
 */
class GLShaderProgram
{
  public:
	enum class Stage
	{
		VERTEX,
		TESS_CONTROL,
		TESS_EVALUATION,
		GEOMETRY,
		FRAGMENT,
		COMPUTE
	};

	// implement those in protected if and only if they're needed for the Python
	// API
	GLShaderProgram()                             = delete;
	GLShaderProgram(GLShaderProgram const& other) = delete;
	GLShaderProgram& operator=(GLShaderProgram const& other) = delete;
	/**
	 * @brief Returns the number of allocated OpenGL shader programs (not the
	 * actual @ref GLShaderProgram number of instances).
	 */
	static unsigned int getInstancesCount() { return instancesCount(); };
	/**
	 * @brief Returns a string description of the shader.
	 *
	 * The integer shown is the actual GLuint id of the OpenGL shader program.
	 */
	QString toStr() const { return QString::number(glShaderProgram); };

	GLShaderProgram(GLShaderProgram&& other)
	    : glShaderProgram(other.glShaderProgram)
	    , doClean(other.doClean)
	{
		// prevent other from cleaning shader if it destroys itself
		other.doClean = false;
	};
	/**
	 * @brief Convenient shortcut for GLShaderProgram(@p shadersCommonName, @p
	 * shadersCommonName, @p shadersCommonName).
	 */
	GLShaderProgram(QString const& shadersCommonName,
	                QMap<QString, QString> const& defines = {});
	/**
	 * @brief Allocates an OpenGL shader program consisting of a vertex shader
	 * and a fragment shader.
	 *
	 * @warning Geometry shaders aren't supported as of now.
	 *
	 * @warning If something is wrong with the shader code, you won't be able to
	 * know in-code as of this version. A qWarning() will be issued, so watch
	 * the standard output if something doesn't render.
	 *
	 * @attention The fragment shader output color must be called "outColor".
	 *
	 * The returned shader program isn't shared anywhere so it is safe to set
	 * its constant uniforms once in its entire lifetime, they won't be
	 * overriden by any side effect.
	 *
	 * You can specify shader names as any path, any data path or only provide
	 * the file name without its extension if it is stored in the data folder
	 * data/ * /shaders and has .vert or .frag extension for vertex shader or
	 * fragment shader respectively. For example, if you want to refer to the
	 * vertex shader located in data/core/shaders/default.vert, you can pass
	 * "default" as the @p vertexName parameter or "shaders/default.vertex" or
	 * its complete path. This allows usage of the convenient function @ref
	 * GLShaderProgram(QString const&) when both vertex and fragment shader
	 * share the same name : GLShaderProgram("default") will search for both
	 * "shaders/default.vert" and "shader/default.frag".
	 *
	 * As of this version, the vertex and fragment shaders linked in the shader
	 * program are freed and will have to be compiled again if a @ref
	 * GLShaderProgram is constructed another time with the same parameters.
	 */
	GLShaderProgram(QString const& vertexName, QString const& fragmentName,
	                QMap<QString, QString> const& defines = {});
	// pair := (name/path, stage)
	GLShaderProgram(std::vector<std::pair<QString, Stage>> const& pipeline,
	                QMap<QString, QString> const& defines = {});
	/** @brief Returns attribute location in shader program.
	 */
	int getAttribLocationFromName(const char* attributeName) const;
	// doesn't work in PythonQt
	/** @brief Sets values for vertex attributes that aren't provided by a
	 * vertex array.
	 *
	 * Useful if one shader is used for different rendering methods that
	 * don't provide the same level of information.
	 *
	 * For example, if your shader takes "in float luminosity;" as input but
	 * your vertex buffer only contains vertex positions, you have to use this
	 * method to set luminosity to 1.f for example. It will disable the vertex
	 * attribute array for "luminosity" and use 1.f instead.
	 *
	 * @param shader Shader for which to set values.
	 * @param defaultValues List of pairs of attribute name + default value (ex:
	 * ("luminosity", {1.f}) or ("normal", {1.f, 0.f, 0.f})). First element of
	 * the pair is the name of the attribute, and second element is the
	 * multidimensional value. The size of the given vector will determine the
	 * type (1 = float, 2 = vec2, 3 = vec3 and 4 = vec4).
	 */
	void setUnusedAttributesValues(
	    std::vector<QPair<const char*, std::vector<float>>> const&
	        defaultValues) const;
	/**
	 * @brief Convenient version of the @ref
	 * setShaderUnusedAttributesValues(GLShaderProgram, std::vector<QPair<const
	 * char*, std::vector<float>>>const&) method to be used in Python.
	 *
	 * Behaves the same way as its other version, but the attribute mapping is
	 * specified differently, as it is harder to construct a QVector<QPair>
	 * object in Python. Instead of an array of pairs (name, values), the
	 * mapping is specified by all the ordered names in the @p names
	 * parameter and all their corresponding values in the same order in the @p
	 * values parameter.
	 */
	void setUnusedAttributesValues(
	    QStringList const& names,
	    std::vector<std::vector<float>> const& values) const;
	/**
	 * @brief Sets the @p shader program's uniform @p paramName to a certain @p
	 * value.
	 *
	 * The uniform must be of type int.
	 */
	void setUniform(const char* paramName, int value) const;
	/**
	 * @brief Sets the @p shader program's uniform @p paramName to a certain @p
	 * value.
	 *
	 * The uniform must be of type float.
	 */
	void setUniform(const char* paramName, float value) const;
	/**
	 * @brief Sets the @p shader program's uniform @p paramName to a certain @p
	 * value.
	 *
	 * The uniform must be of type vec2.
	 */
	void setUniform(const char* paramName, QVector2D const& value) const;
	/**
	 * @brief Sets the @p shader program's uniform @p paramName to a certain @p
	 * value.
	 *
	 * The uniform must be of type vec3.
	 */
	void setUniform(const char* paramName, QVector3D const& value) const;
	/**
	 * @brief Sets the @p shader program's uniform @p paramName to a certain @p
	 * array of values of size @size.
	 *
	 * The uniform must be an array of type vec3.
	 */
	void setUniform(const char* paramName, unsigned int size,
	                QVector3D const* values) const;
	/**
	 * @brief Sets the @p shader program's uniform @p paramName to a certain @p
	 * value.
	 *
	 * The uniform must be of type vec4.
	 */
	void setUniform(const char* paramName, QVector4D const& value) const;
	/**
	 * @brief Sets the @p shader program's uniform @p paramName to a certain @p
	 * array of values of size @size.
	 *
	 * The uniform must be an array of type vec4.
	 */
	void setUniform(const char* paramName, unsigned int size,
	                QVector4D const* values) const;
	/**
	 * @brief Sets the @p shader program's uniform @p paramName to a certain @p
	 * value.
	 *
	 * The uniform must be of type mat4.
	 */
	void setUniform(const char* paramName, QMatrix4x4 const& value) const;
	/**
	 * @brief Sets the @p shader program's uniform @p paramName to a certain @p
	 * value.
	 *
	 * The uniform must be of type vec3. If the color is from @p sRGB space, it
	 * will be converted to linear space before being set.
	 */
	void setUniform(const char* paramName, QColor const& value,
	                bool sRGB = true) const;
	/**
	 * @brief Tells OpenGL to use this @p shader program for rendering.
	 *
	 * If you exclusively use @ref GLHandler methods, this method will be called
	 * automatically when necessary and you shouldn't use it.
	 */
	void use() const;
	virtual ~GLShaderProgram() { cleanUp(); };

  protected:
	/**
	 * @brief Frees the underlying OpenGL shader program.
	 */
	void cleanUp();

	/**
	 * @brief Returns a parameter from this OpenGL shader program object.
	 *
	 * See
	 * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetProgram.xhtml
	 * .
	 */
	void get(GLenum pname, GLint* params) const;

  private:
	const GLuint glShaderProgram;

	bool doClean = true;
	static unsigned int& instancesCount();

	static std::pair<QString, GLenum> decodeStage(Stage s);
	static QString
	    getFullPreprocessedSource(QString const& path,
	                              QMap<QString, QString> const& defines);
	static GLuint loadShader(QString const& path, GLenum shaderType,
	                         QMap<QString, QString> const& defines);
};

#define SETUNIFORM(type)                                            \
	void setUniform(GLShaderProgram* s, QString const& str, type v) \
	{                                                               \
		s->setUniform(str.toStdString().c_str(), v);                \
	}

class GLShaderProgramWrapper : public PythonQtWrapper
{
	Q_OBJECT
  public:
	virtual const char* wrappedClassName() const override
	{
		return "GLShaderProgram";
	}
	virtual const char* wrappedClassPackage() const override { return "GL"; }

  public Q_SLOTS:
	GLShaderProgram* new_GLShaderProgram(QString const& shadersCommonName)
	{
		return new GLShaderProgram(shadersCommonName);
	}
	GLShaderProgram* new_GLShaderProgram(QString const& vertexName,
	                                     QString const& fragmentName)
	{
		return new GLShaderProgram(vertexName, fragmentName);
	}
	unsigned int static_GLShaderProgram_getInstancesCount()
	{
		return GLShaderProgram::getInstancesCount();
	};
	SETUNIFORM(int);
	SETUNIFORM(float);
	SETUNIFORM(QVector2D const&);
	SETUNIFORM(QVector3D const&);
	SETUNIFORM(QVector4D const&);
	SETUNIFORM(QMatrix4x4 const&);
	SETUNIFORM(QColor const&);
	QString str(GLShaderProgram* s) { return s->toStr(); }
	void delete_GLShaderProgram(GLShaderProgram* v) { delete v; }

	// access methods
};

// if we ever need to Q_DECLARE_METATYPE
/*class PYGLShaderProgram : public GLShaderProgram
{
  public:
    PYGLShaderProgram()                         = default;
    PYGLShaderProgram(PYGLShaderProgram const&) = default;
    PYGLShaderProgram& operator=(PYGLShaderProgram const&) = default;
    PYGLShaderProgram(QString const& shadersCommonName,
                      QMap<QString, QString> const& defines = {})
        : GLShaderProgram(shadersCommonName, defines){};
    PYGLShaderProgram(QString const& vertexName, QString const& fragmentName,
                      QMap<QString, QString> const& defines = {},
                      QString geometryName                  = "")
        : GLShaderProgram(vertexName, fragmentName, defines, geometryName){};
    void cleanUp() { GLShaderProgram::cleanUp(); };
};

Q_DECLARE_METATYPE(PYGLShaderProgram)

class GLShaderProgramWrapper : public PythonQtWrapper
{
    Q_OBJECT
  public:
    virtual const char* wrappedClassName() const override
    {
        return "GLShaderProgram";
    }
    virtual const char* wrappedClassPackage() const override
    {
        return "HydrogenVR";
    }

  public Q_SLOTS:
    PYGLShaderProgram* new_GLShaderProgram(QString const& shadersCommonName)
    {
        return new PYGLShaderProgram(shadersCommonName);
    }
    PYGLShaderProgram* new_GLShaderProgram(QString const& vertexName,
                                           QString const& fragmentName)
    {
        return new PYGLShaderProgram(vertexName, fragmentName);
    }
    unsigned int static_GLShaderProgram_getInstancesCount()
    {
        return PYGLShaderProgram::getInstancesCount();
    };
    void cleanUp(PYGLShaderProgram* s) { s->cleanUp(); }

    void delete_GLShaderProgram(PYGLShaderProgram* v) { delete v; }

    // access methods
};*/

#endif // GLSHADERPROGRAM_HPP
