/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@hotmail.fr>

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

#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "GLHandler.hpp"
#include "Primitives.hpp"

// for now directional only
class Light
{
  public:
	Light();
	QMatrix4x4 getTransformation(float boundingSphereRadius,
	                             QMatrix4x4 const& model,
	                             bool biased = false) const;
	void setUpShader(GLHandler::ShaderProgram const& shader,
	                 float boundingSphereRadius, QMatrix4x4 const& model) const;
	GLHandler::Texture getShadowMap() const;
	void generateShadowMap(std::vector<GLHandler::Mesh> meshes,
	                       float boundingSphereRadius, QMatrix4x4 const& model);
	~Light();

	QVector3D direction;
	QColor color;
	float ambiantFactor;

  private:
	GLHandler::RenderTarget shadowMap;
	GLHandler::ShaderProgram shadowShader;
};

#endif // LIGHT_HPP
