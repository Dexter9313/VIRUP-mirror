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

DebugCamera::DebugCamera(VRHandler const& vrHandler)
    : BasicCamera(vrHandler)
    , camMeshShader("default")
{
	camMeshShader.setUniform("color", QColor::fromRgbF(1.0f, 0.0f, 0.0f));

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

	camMesh.setVertices(vertices, camMeshShader, {{"position", 3}}, elements);
}

void DebugCamera::update(QMatrix4x4 const& angleShiftMat)
{
	// act as a 2D camera if we debug from screen only
	if(!QSettings().value("debugcamera/debuginheadset").toBool())
	{
		BasicCamera::update2D(angleShiftMat);
	}
	else
	{
		BasicCamera::update(angleShiftMat);
	}
}

void DebugCamera::renderCamera(BasicCamera const* cam)
{
	if(vrHandler.isEnabled() && followHMD())
	{
		camMeshShader.setUniform("color", QColor::fromRgbF(1.0f, 0.0f, 0.0f));
		GLHandler::setUpRender(camMeshShader,
		                       cam->hmdScreenToWorldTransform(Side::LEFT));
		camMesh.render(PrimitiveType::LINES);
		camMeshShader.setUniform("color", QColor::fromRgbF(0.0f, 1.0f, 0.0f));
		GLHandler::setUpRender(camMeshShader,
		                       cam->hmdScreenToWorldTransform(Side::RIGHT));
		camMesh.render(PrimitiveType::LINES);
	}
	else
	{
		camMeshShader.setUniform("color", QColor::fromRgbF(1.0f, 0.0f, 0.0f));
		GLHandler::setUpRender(camMeshShader, cam->screenToWorldTransform());
		camMesh.render(PrimitiveType::LINES);
	}
}

bool DebugCamera::isEnabled() const
{
	return QSettings().value("debugcamera/enabled").toBool();
}

void DebugCamera::setEnabled(bool enabled)
{
	QSettings().setValue("debugcamera/enabled", enabled);
}

void DebugCamera::toggle()
{
	setEnabled(!isEnabled());
}

bool DebugCamera::debugInHeadset() const
{
	return QSettings().value("debugcamera/debuginheadset").toBool();
}

void DebugCamera::setDebugInHeadset(bool debuginheadset)
{
	QSettings().setValue("debugcamera/debuginheadset", debuginheadset);
}

void DebugCamera::toggleDebugInHeadset()
{
	setDebugInHeadset(!debugInHeadset());
}

bool DebugCamera::followHMD() const
{
	return QSettings().value("debugcamera/followhmd").toBool();
}

void DebugCamera::setFollowHMD(bool followhmd)
{
	QSettings().setValue("debugcamera/followhmd", followhmd);
}

void DebugCamera::toggleFollowHMD()
{
	setFollowHMD(!followHMD());
}
