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
#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

#include <QString>
#include <map>
#include <unordered_set>

#include "GLHandler.hpp"

class ShaderProgram
{
  public:
	enum UniformBaseType
	{
		UNKNOWN,
		FLOAT,
		DOUBLE,
		UINT,
		INT,
	};

	ShaderProgram();
	ShaderProgram(QString const& shadersCommonName,
	              QMap<QString, QString> const& defines = {});
	ShaderProgram(QString const& vertexName, QString const& fragmentName,
	              QMap<QString, QString> const& defines = {});
	GLHandler::ShaderProgram toGLHandler() const;
	void load(QString const& shadersCommonName,
	          QMap<QString, QString> const& defines = {});
	void load(QString const& vertexName, QString const& fragmentName,
	          QMap<QString, QString> const& defines = {});
	void reload();
	bool isValid() { return valid; };
	static void reloadAllShaderPrograms();
	template <typename T>
	void setUniform(char const* name, T const& value);
	~ShaderProgram();

  private:
	GLHandler::ShaderProgram glShader = 0;

	bool valid = false;
	QString vert;
	QString frag;
	QMap<QString, QString> defines;

	std::map<char const*, QVariant> uniformsBackup;

	static std::unordered_set<ShaderProgram*>& allShaderPrograms();

	// static std::pair<UniformBaseType, unsigned int> decodeUniformType(GLenum
	// type);
};

template <typename T>
void ShaderProgram::setUniform(char const* name, T const& value)
{
	uniformsBackup[name] = value;
	GLHandler::setShaderParam(glShader, name, value);
}
#endif // SHADERPROGRAM_HPP
