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

#include "gl/GLShaderProgram.hpp"

unsigned int& GLShaderProgram::instancesCount()
{
	static unsigned int instancesCount = 0;
	return instancesCount;
}

GLShaderProgram::GLShaderProgram(QString const& shadersCommonName,
                                 QMap<QString, QString> const& defines)
    : GLShaderProgram(shadersCommonName, shadersCommonName, defines)
{
}

GLShaderProgram::GLShaderProgram(QString const& vertexName,
                                 QString const& fragmentName,
                                 QMap<QString, QString> const& defines)
    : GLShaderProgram(
          {{vertexName, Stage::VERTEX}, {fragmentName, Stage::FRAGMENT}},
          defines)
{
}

GLShaderProgram::GLShaderProgram(
    std::vector<std::pair<QString, Stage>> const& pipeline,
    QMap<QString, QString> const& defines)
    : glShaderProgram(GLHandler::glf().glCreateProgram())
{
	++instancesCount();

	for(auto const& stage : pipeline)
	{
		QString name(stage.first);
		// (extension, GLenum stage)
		auto pair(decodeStage(stage.second));
		if(!name.contains('.'))
		{
			name = "shaders/" + name + pair.first;
		}
		// vertex shader
		GLuint shader(loadShader(name, pair.second, defines));
		GLHandler::glf().glAttachShader(glShaderProgram, shader);
		GLHandler::glf().glDeleteShader(shader);
	}

	GLHandler::glf().glBindFragDataLocation(
	    glShaderProgram, 0,
	    "outColor"); // optional for one buffer
	GLHandler::glf().glLinkProgram(glShaderProgram);
	GLHandler::glf().glValidateProgram(glShaderProgram);
}

void GLShaderProgram::cleanUp()
{
	if(!doClean)
	{
		return;
	}
	--instancesCount();
	GLHandler::glf().glUseProgram(0);
	GLHandler::glf().glDeleteProgram(glShaderProgram);
	doClean = false;
}

int GLShaderProgram::getAttribLocationFromName(const char* attributeName) const
{
	return GLHandler::glf().glGetAttribLocation(glShaderProgram, attributeName);
}

void GLShaderProgram::setUnusedAttributesValues(
    std::vector<QPair<const char*, std::vector<float>>> const& defaultValues)
    const
{
	for(auto attribute : defaultValues)
	{
		GLint posAttrib = GLHandler::glf().glGetAttribLocation(glShaderProgram,
		                                                       attribute.first);
		if(posAttrib != -1)
		{
			GLHandler::glf().glDisableVertexAttribArray(posAttrib);
			// special case where we have to do it, see :
			// https://bugreports.qt.io/browse/QTBUG-40090?jql=text%20~%20%22glvertexattrib%22
			QOpenGLFunctions glf_base;
			glf_base.initializeOpenGLFunctions();
			switch(attribute.second.size())
			{
				case 1:
					glf_base.glVertexAttrib1fv(posAttrib, &attribute.second[0]);
					break;
				case 2:
					glf_base.glVertexAttrib2fv(posAttrib, &attribute.second[0]);
					break;
				case 3:
					glf_base.glVertexAttrib3fv(posAttrib, &attribute.second[0]);
					break;
				case 4:
					glf_base.glVertexAttrib4fv(posAttrib, &attribute.second[0]);
					break;
				default:
					break;
			}
		}
	}
}

void GLShaderProgram::setUnusedAttributesValues(
    QStringList const& names,
    std::vector<std::vector<float>> const& values) const
{
	std::vector<QPair<const char*, std::vector<float>>> defaultValues;
	for(unsigned int i(0); i < values.size(); ++i)
	{
		defaultValues.emplace_back(names[i].toLatin1().constData(), values[i]);
	}
	setUnusedAttributesValues(defaultValues);
}

void GLShaderProgram::setUniform(const char* paramName, int value) const
{
	use();
	GLHandler::glf().glUniform1i(
	    GLHandler::glf().glGetUniformLocation(glShaderProgram, paramName),
	    value);
}

void GLShaderProgram::setUniform(const char* paramName, float value) const
{
	use();
	GLHandler::glf().glUniform1f(
	    GLHandler::glf().glGetUniformLocation(glShaderProgram, paramName),
	    value);
}

void GLShaderProgram::setUniform(const char* paramName,
                                 QVector2D const& value) const
{
	use();
	GLHandler::glf().glUniform2f(
	    GLHandler::glf().glGetUniformLocation(glShaderProgram, paramName),
	    value.x(), value.y());
}

void GLShaderProgram::setUniform(const char* paramName,
                                 QVector3D const& value) const
{
	use();
	GLHandler::glf().glUniform3f(
	    GLHandler::glf().glGetUniformLocation(glShaderProgram, paramName),
	    value.x(), value.y(), value.z());
}

void GLShaderProgram::setUniform(const char* paramName, unsigned int size,
                                 QVector3D const* values) const
{
	use();
	auto data = new GLfloat[3 * size];
	for(unsigned int i(0); i < size; ++i)
	{
		for(unsigned int j(0); j < 3; ++j)
		{
			data[i * 3 + j] = values[i][j];
		}
	}
	GLHandler::glf().glUniform3fv(
	    GLHandler::glf().glGetUniformLocation(glShaderProgram, paramName), size,
	    &(data[0]));
	delete[] data;
}

void GLShaderProgram::setUniform(const char* paramName,
                                 QVector4D const& value) const
{
	use();
	GLHandler::glf().glUniform4f(
	    GLHandler::glf().glGetUniformLocation(glShaderProgram, paramName),
	    value.x(), value.y(), value.z(), value.w());
}

void GLShaderProgram::setUniform(const char* paramName, unsigned int size,
                                 QVector4D const* values) const
{
	use();
	auto data = new GLfloat[4 * size];
	for(unsigned int i(0); i < size; ++i)
	{
		for(unsigned int j(0); j < 4; ++j)
		{
			data[i * 4 + j] = values[i][j];
		}
	}
	GLHandler::glf().glUniform4fv(
	    GLHandler::glf().glGetUniformLocation(glShaderProgram, paramName), size,
	    &(data[0]));
	delete[] data;
}

void GLShaderProgram::setUniform(const char* paramName,
                                 QMatrix4x4 const& value) const
{
	use();
	GLHandler::glf().glUniformMatrix4fv(
	    GLHandler::glf().glGetUniformLocation(glShaderProgram, paramName), 1,
	    GL_FALSE, value.data());
}

void GLShaderProgram::setUniform(const char* paramName, QColor const& value,
                                 bool sRGB) const
{
	QColor linVal(sRGB ? GLHandler::sRGBToLinear(value) : value);
	setUniform(paramName,
	           QVector3D(linVal.redF(), linVal.greenF(), linVal.blueF()));
}
void GLShaderProgram::use() const
{
	GLHandler::glf().glUseProgram(glShaderProgram);
}

void GLShaderProgram::get(GLenum pname, GLint* params) const
{
	GLHandler::glf().glGetProgramiv(glShaderProgram, pname, params);
}

std::pair<QString, GLenum> GLShaderProgram::decodeStage(Stage s)
{
	switch(s)
	{
		case Stage::VERTEX:
			return {".vert", GL_VERTEX_SHADER};
		case Stage::TESS_CONTROL:
			return {".tesc", GL_TESS_CONTROL_SHADER};
		case Stage::TESS_EVALUATION:
			return {".tese", GL_TESS_EVALUATION_SHADER};
		case Stage::GEOMETRY:
			return {".geom", GL_GEOMETRY_SHADER};
		case Stage::FRAGMENT:
			return {".frag", GL_FRAGMENT_SHADER};
		case Stage::COMPUTE:
			return {".comp", GL_COMPUTE_SHADER};
		default:
			return {};
	}
}

QString GLShaderProgram::getFullPreprocessedSource(
    QString const& path, QMap<QString, QString> const& defines)
{
	// Read source
	QFile f(getAbsoluteDataPath(path));
	if(!f.exists())
	{
		f.setFileName(getAbsoluteDataPath("shaders/" + path));
	}
	f.open(QFile::ReadOnly | QFile::Text);
	QTextStream in(&f);
	QString source(in.readAll().toLocal8Bit());

	// Strip comments
	// One-liners
	int commentPos(source.indexOf("//"));
	while(commentPos != -1)
	{
		int endOfComment(source.indexOf('\n', commentPos));
		source.replace(commentPos, endOfComment - commentPos, "");
		commentPos = source.indexOf("//", commentPos);
	}
	// Blocks
	commentPos = source.indexOf("/*");
	while(commentPos != -1)
	{
		int endOfComment(source.indexOf("*/", commentPos));
		source.replace(commentPos, endOfComment - commentPos + 2, "");
		commentPos = source.indexOf("/*", commentPos);
	}

	// include other preprocessed sources within source
	int includePos(source.indexOf("#include"));
	while(includePos != -1)
	{
		int beginPath(source.indexOf('<', includePos));
		int endPath(source.indexOf('>', includePos));
		int endOfLine(source.indexOf('\n', includePos));

		QString includedSrc(getFullPreprocessedSource(
		    source.mid(beginPath + 1, endPath - beginPath - 1), defines));
		source.replace(includePos, endOfLine - includePos, includedSrc);

		includePos = source.indexOf("#include", includePos);
	}

	// add defines after #version
	int definesInsertPoint(source.indexOf("#version"));
	definesInsertPoint = source.indexOf('\n', definesInsertPoint) + 1;
	for(auto const& key : defines.keys())
	{
		source.insert(definesInsertPoint, QString("#define ") + key + " "
		                                      + defines.value(key) + "\n");
	}

	return source;
}

GLuint GLShaderProgram::loadShader(QString const& path, GLenum shaderType,
                                   QMap<QString, QString> const& defines)
{
	QString source(getFullPreprocessedSource(path, defines));
	QByteArray ba     = source.toLatin1();
	const char* bytes = ba.data();

	GLuint shader = GLHandler::glf().glCreateShader(shaderType);
	GLHandler::glf().glShaderSource(shader, 1, &bytes, nullptr);
	GLHandler::glf().glCompileShader(shader);
	// checks
	GLint status;
	GLHandler::glf().glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	std::array<char, 512> buffer = {};
	GLHandler::glf().glGetShaderInfoLog(shader, 512, nullptr, &buffer[0]);
	if(status != GL_TRUE)
	{
		qWarning() << "SHADER ERROR (" << path << "-" << shader
		           << ") :" << &buffer[0] << '\n';
	}

	return shader;
}
