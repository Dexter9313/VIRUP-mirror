/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@epfl.ch>

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

#include "Billboard.hpp"

Billboard::Billboard(const char* texPath)
    : Billboard(texPath, GLShaderProgram("billboard"))
{
}
Billboard::Billboard(QImage const& image)
    : Billboard(image, GLShaderProgram("billboard"))
{
}

Billboard::Billboard(const char* texPath, GLShaderProgram&& shader)
    : tex(texPath)
    , shader(std::move(shader))
{
	Primitives::setAsQuad(quad, this->shader, PrimitiveType::TRIANGLE_STRIP);
}

Billboard::Billboard(QImage const& image, GLShaderProgram&& shader)
    : tex(image)
    , shader(std::move(shader))
{
	Primitives::setAsQuad(quad, this->shader, PrimitiveType::TRIANGLE_STRIP);
}

void Billboard::render(BasicCamera const& camera)
{
	QVector3D hmdPos = camera.hmdSpaceToWorldTransform().inverted() * position;
	QMatrix4x4 model;
	model.translate(hmdPos);
	model.scale(width / camera.getEyeDistanceFactor());
	GLHandler::beginTransparent();
	GLHandler::useTextures({&tex});
	GLHandler::setUpRender(shader, model, GLHandler::GeometricSpace::HMD);
	quad.render(PrimitiveType::TRIANGLE_STRIP);
	GLHandler::endTransparent();
}
