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

#include "DebugCamera.hpp"

DebugCamera::DebugCamera(VRHandler const* vrHandler)
    : BasicCamera(vrHandler)
    , camMesh(GLHandler::newMesh())
    , camMeshShader(GLHandler::newShader("default"))
{
	GLHandler::setShaderParam(camMeshShader, "color", QColor::fromRgbF(1.0f, 0.0f, 0.0f));

	std::vector<float> vertices = {
	    -1.0f, -1.0f, -1.0f, // 0
	    -1.0f, -1.0f, 1.0f,  // 1
	    -1.0f, 1.0f,  -1.0f, // 2
	    -1.0f, 1.0f,  1.0f,  // 3
	    1.0f,  -1.0f, -1.0f, // 4
	    1.0f,  -1.0f, 1.0f,  // 5
	    1.0f,  1.0f,  -1.0f, // 6
	    1.0f,  1.0f,  1.0f,  // 7
	};

	std::vector<unsigned int> elements = {
	    0, 1, 0, 2, 0, 4,

	    7, 6, 7, 5, 7, 3,

	    1, 3, 1, 5,

	    2, 6, 2, 3,

	    4, 6, 4, 5,
	};

	GLHandler::setVertices(camMesh, vertices, camMeshShader, {{"position", 3}},
	                       elements);
}

void DebugCamera::update(bool force2D)
{
	// act as a 2D camera if we debug from screen only
	BasicCamera::update(
	    force2D || !QSettings().value("debugcamera/debuginheadset").toBool());
}

void DebugCamera::renderCamera(BasicCamera const* cam)
{
	bool followHMD(QSettings().value("debugcamera/followhmd").toBool());
	if(*vrHandler && followHMD)
	{
		GLHandler::setShaderParam(camMeshShader, "color", QColor::fromRgbF(1.0f, 0.0f, 0.0f));
		GLHandler::setUpRender(camMeshShader,
		                       cam->hmdScreenToWorldTransform(Side::LEFT));
		GLHandler::render(camMesh, GLHandler::PrimitiveType::LINES);
		GLHandler::setShaderParam(camMeshShader, "color", QColor::fromRgbF(0.0f, 1.0f, 0.0f));
		GLHandler::setUpRender(camMeshShader,
		                       cam->hmdScreenToWorldTransform(Side::RIGHT));
		GLHandler::render(camMesh, GLHandler::PrimitiveType::LINES);
	}
	else
	{
		GLHandler::setShaderParam(camMeshShader, "color", QColor::fromRgbF(1.0f, 0.0f, 0.0f));
		GLHandler::setUpRender(camMeshShader, cam->screenToWorldTransform());
		GLHandler::render(camMesh, GLHandler::PrimitiveType::LINES);
	}
}
