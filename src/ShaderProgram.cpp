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

#include "ShaderProgram.hpp"

std::unordered_set<ShaderProgram*>& ShaderProgram::allShaderPrograms()
{
	static std::unordered_set<ShaderProgram*> allShaderPrograms = {};
	return allShaderPrograms;
}

ShaderProgram::ShaderProgram()
{
	allShaderPrograms().insert(this);
}

ShaderProgram::ShaderProgram(QString const& shadersCommonName,
                             QMap<QString, QString> const& defines)
    : ShaderProgram(shadersCommonName, shadersCommonName, defines)
{
}

ShaderProgram::ShaderProgram(QString const& vertexName,
                             QString const& fragmentName,
                             QMap<QString, QString> const& defines)
{
	load(vertexName, fragmentName, defines);

	allShaderPrograms().insert(this);
}

GLHandler::ShaderProgram ShaderProgram::toGLHandler() const
{
	return glShader;
}

void ShaderProgram::load(QString const& shadersCommonName,
                         QMap<QString, QString> const& defines)
{
	load(shadersCommonName, shadersCommonName, defines);
}

void ShaderProgram::load(QString const& vertexName, QString const& fragmentName,
                         QMap<QString, QString> const& defines)
{
	if(valid)
	{
		GLHandler::deleteShader(glShader);
	}
	glShader      = GLHandler::newShader(vertexName, fragmentName, defines);
	valid         = true;
	vert          = vertexName;
	frag          = fragmentName;
	this->defines = defines;
	/*
	GLint count;
	GLint size;  // size of the variable
	GLenum type; // type of the variable (float, vec3 or mat4, etc)

	const GLsizei bufSize = 16; // maximum name length
	GLchar name[bufSize];       // variable name in GLSL
	GLsizei length;             // name length
	GLHandler::glf().glGetProgramiv(glShader, GL_ACTIVE_UNIFORMS, &count);
	printf("Active Uniforms: %d\n", count);

	for(GLint i(0); i < count; i++)
	{
	    GLHandler::glf().glGetActiveUniform(glShader, (GLuint) i, bufSize,
	                                        &length, &size, &type, name);

	    printf("\tUniform #%d Type id: %u Name: %s\n", i, type, name);
	    std::string typeStr;
	    auto decoded(decodeUniformType(type));
	    switch(decoded.first)
	    {
	        case UNKNOWN:
	            typeStr = "UNKNOWN";
	            break;
	        case FLOAT:
	            typeStr = "float";
	            break;
	        case DOUBLE:
	            typeStr = "double";
	            break;
	        case UINT:
	            typeStr = "unsigned int";
	            break;
	        case INT:
	            typeStr = "int";
	            break;
	    }
	    printf("\t\tType: %s[%d]\n", typeStr.c_str(), decoded.second);
	    std::cout << "\t\tValue : ";

	    GLint loc(GLHandler::glf().glGetUniformLocation(glShader, name));
	    std::vector<float> valf(decoded.second);
	    std::vector<double> vald(decoded.second);
	    std::vector<int> vali(decoded.second);
	    std::vector<unsigned int> valui(decoded.second);
	    switch(decoded.first)
	    {
	        case UNKNOWN:
	            break;
	        case FLOAT:
	            GLHandler::glf().glGetUniformfv(glShader, loc, &(valf[0]));
	            for(auto v : valf)
	            {
	                std::cout << v << ", ";
	            }
	            break;
	        case DOUBLE:
	            GLHandler::glf().glGetUniformdv(glShader, loc, &(vald[0]));
	            for(auto v : vald)
	            {
	                std::cout << v << ", ";
	            }
	            break;
	        case UINT:
	            GLHandler::glf().glGetUniformiv(glShader, loc, &(vali[0]));
	            for(auto v : vali)
	            {
	                std::cout << v << ", ";
	            }
	            break;
	        case INT:
	            GLHandler::glf().glGetUniformuiv(glShader, loc, &(valui[0]));
	            for(auto v : valui)
	            {
	                std::cout << v << ", ";
	            }
	            break;
	    }
	    std::cout << "\n";
	}
	*/
}

void ShaderProgram::reload()
{
	if(valid)
	{
		GLHandler::deleteShader(glShader);
		glShader = GLHandler::newShader(vert, frag, defines);
		for(auto const& pair : uniformsBackup)
		{
			auto type(static_cast<QMetaType::Type>(pair.second.type()));
			switch(type)
			{
				case QMetaType::Int:
					GLHandler::setShaderParam(glShader, pair.first,
					                          pair.second.toInt());
					break;
				case QMetaType::Float:
					GLHandler::setShaderParam(glShader, pair.first,
					                          pair.second.toFloat());
					break;
				case QMetaType::QVector2D:
					GLHandler::setShaderParam(glShader, pair.first,
					                          pair.second.value<QVector2D>());
					break;
				case QMetaType::QVector3D:
					GLHandler::setShaderParam(glShader, pair.first,
					                          pair.second.value<QVector3D>());
					break;
				case QMetaType::QVector4D:
					GLHandler::setShaderParam(glShader, pair.first,
					                          pair.second.value<QVector4D>());
					break;
				case QMetaType::QMatrix4x4:
					GLHandler::setShaderParam(glShader, pair.first,
					                          pair.second.value<QMatrix4x4>());
					break;
				case QMetaType::QColor:
					GLHandler::setShaderParam(glShader, pair.first,
					                          pair.second.value<QColor>());
					break;
				default:
					break;
			}
		}
	}
}

void ShaderProgram::reloadAllShaderPrograms()
{
	for(auto shaderProgram : allShaderPrograms())
	{
		shaderProgram->reload();
	}
}

ShaderProgram::~ShaderProgram()
{
	if(valid)
	{
		GLHandler::deleteShader(glShader);
	}

	allShaderPrograms().erase(this);
}

/*
std::pair<ShaderProgram::UniformBaseType, unsigned int>
    ShaderProgram::decodeUniformType(GLenum type)
{
    std::unordered_set<GLenum> floatTypes({
        GL_FLOAT,
        GL_FLOAT_VEC2,
        GL_FLOAT_VEC3,
        GL_FLOAT_VEC4,
        GL_FLOAT_MAT2,
        GL_FLOAT_MAT3,
        GL_FLOAT_MAT4,
        GL_FLOAT_MAT2x3,
        GL_FLOAT_MAT2x4,
        GL_FLOAT_MAT3x2,
        GL_FLOAT_MAT3x4,
        GL_FLOAT_MAT4x2,
        GL_FLOAT_MAT4x3,
    });

    std::unordered_set<GLenum> doubleTypes({
        GL_DOUBLE,
        GL_DOUBLE_VEC2,
        GL_DOUBLE_VEC3,
        GL_DOUBLE_VEC4,
        GL_DOUBLE_MAT2,
        GL_DOUBLE_MAT3,
        GL_DOUBLE_MAT4,
        GL_DOUBLE_MAT2x3,
        GL_DOUBLE_MAT2x4,
        GL_DOUBLE_MAT3x2,
        GL_DOUBLE_MAT3x4,
        GL_DOUBLE_MAT4x2,
        GL_DOUBLE_MAT4x3,
    });

    std::unordered_set<GLenum> uintTypes({
        GL_UNSIGNED_INT,
        GL_UNSIGNED_INT_VEC2,
        GL_UNSIGNED_INT_VEC3,
        GL_UNSIGNED_INT_VEC4,
    });

    std::unordered_set<GLenum> intTypes({
        GL_INT,
        GL_INT_VEC2,
        GL_INT_VEC3,
        GL_INT_VEC4,
        GL_SAMPLER_1D,
        GL_SAMPLER_2D,
        GL_SAMPLER_3D,
        GL_SAMPLER_CUBE,
        GL_SAMPLER_1D_SHADOW,
        GL_SAMPLER_2D_SHADOW,
        GL_SAMPLER_1D_ARRAY,
        GL_SAMPLER_2D_ARRAY,
        GL_SAMPLER_1D_ARRAY_SHADOW,
        GL_SAMPLER_2D_ARRAY_SHADOW,
        GL_SAMPLER_2D_MULTISAMPLE,
        GL_SAMPLER_2D_MULTISAMPLE_ARRAY,
        GL_SAMPLER_CUBE_SHADOW,
        GL_SAMPLER_BUFFER,
        GL_SAMPLER_2D_RECT,
        GL_SAMPLER_2D_RECT_SHADOW,
        GL_INT_SAMPLER_1D,
        GL_INT_SAMPLER_2D,
        GL_INT_SAMPLER_3D,
        GL_INT_SAMPLER_CUBE,
        GL_INT_SAMPLER_1D_ARRAY,
        GL_INT_SAMPLER_2D_ARRAY,
        GL_INT_SAMPLER_2D_MULTISAMPLE,
        GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
        GL_INT_SAMPLER_BUFFER,
        GL_INT_SAMPLER_2D_RECT,
        GL_UNSIGNED_INT_SAMPLER_1D,
        GL_UNSIGNED_INT_SAMPLER_2D,
        GL_UNSIGNED_INT_SAMPLER_3D,
        GL_UNSIGNED_INT_SAMPLER_CUBE,
        GL_UNSIGNED_INT_SAMPLER_1D_ARRAY,
        GL_UNSIGNED_INT_SAMPLER_2D_ARRAY,
        GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE,
        GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
        GL_UNSIGNED_INT_SAMPLER_BUFFER,
        GL_UNSIGNED_INT_SAMPLER_2D_RECT,
        GL_IMAGE_1D,
        GL_IMAGE_2D,
        GL_IMAGE_3D,
        GL_IMAGE_2D_RECT,
        GL_IMAGE_CUBE,
        GL_IMAGE_BUFFER,
        GL_IMAGE_1D_ARRAY,
        GL_IMAGE_2D_ARRAY,
        GL_IMAGE_2D_MULTISAMPLE,
        GL_IMAGE_2D_MULTISAMPLE_ARRAY,
        GL_INT_IMAGE_1D,
        GL_INT_IMAGE_2D,
        GL_INT_IMAGE_3D,
        GL_INT_IMAGE_2D_RECT,
        GL_INT_IMAGE_CUBE,
        GL_INT_IMAGE_BUFFER,
        GL_INT_IMAGE_1D_ARRAY,
        GL_INT_IMAGE_2D_ARRAY,
        GL_INT_IMAGE_2D_MULTISAMPLE,
        GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY,
        GL_UNSIGNED_INT_IMAGE_1D,
        GL_UNSIGNED_INT_IMAGE_2D,
        GL_UNSIGNED_INT_IMAGE_3D,
        GL_UNSIGNED_INT_IMAGE_2D_RECT,
        GL_UNSIGNED_INT_IMAGE_CUBE,
        GL_UNSIGNED_INT_IMAGE_BUFFER,
        GL_UNSIGNED_INT_IMAGE_1D_ARRAY,
        GL_UNSIGNED_INT_IMAGE_2D_ARRAY,
        GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE,
        GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY,
        GL_UNSIGNED_INT_ATOMIC_COUNTER,
    });

    UniformBaseType resultBaseType(UniformBaseType::UNKNOWN);
    if(floatTypes.count(type) > 0)
    {
        resultBaseType = UniformBaseType::FLOAT;
    }
    else if(doubleTypes.count(type) > 0)
    {
        resultBaseType = UniformBaseType::DOUBLE;
    }
    else if(uintTypes.count(type) > 0)
    {
        resultBaseType = UniformBaseType::UINT;
    }
    else if(intTypes.count(type) > 0)
    {
        resultBaseType = UniformBaseType::INT;
    }

    if(resultBaseType == UniformBaseType::UNKNOWN)
    {
        return {resultBaseType, 0};
    }

    unsigned int resultSize(1);
    switch(type)
    {
        case GL_FLOAT_VEC2:
        case GL_DOUBLE_VEC2:
        case GL_UNSIGNED_INT_VEC2:
        case GL_INT_VEC2:
            resultSize = 2;
            break;
        case GL_FLOAT_VEC3:
        case GL_DOUBLE_VEC3:
        case GL_UNSIGNED_INT_VEC3:
        case GL_INT_VEC3:
            resultSize = 3;
            break;
        case GL_FLOAT_VEC4:
        case GL_FLOAT_MAT2:
        case GL_DOUBLE_VEC4:
        case GL_DOUBLE_MAT2:
        case GL_UNSIGNED_INT_VEC4:
        case GL_INT_VEC4:
            resultSize = 4;
            break;
        case GL_FLOAT_MAT2x3:
        case GL_FLOAT_MAT3x2:
        case GL_DOUBLE_MAT2x3:
        case GL_DOUBLE_MAT3x2:
            resultSize = 6;
            break;
        case GL_FLOAT_MAT2x4:
        case GL_FLOAT_MAT4x2:
        case GL_DOUBLE_MAT2x4:
        case GL_DOUBLE_MAT4x2:
            resultSize = 8;
            break;
        case GL_FLOAT_MAT3:
        case GL_DOUBLE_MAT3:
            resultSize = 9;
            break;
        case GL_FLOAT_MAT3x4:
        case GL_FLOAT_MAT4x3:
        case GL_DOUBLE_MAT3x4:
        case GL_DOUBLE_MAT4x3:
            resultSize = 12;
            break;
        case GL_FLOAT_MAT4:
        case GL_DOUBLE_MAT4:
            resultSize = 16;
            break;
        default:
            resultSize = 1;
    }

    return {resultBaseType, resultSize};
}
*/
