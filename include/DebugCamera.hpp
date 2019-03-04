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

#ifndef DEBUGCAMERA_H
#define DEBUGCAMERA_H

#include "BasicCamera.hpp"

class DebugCamera : public BasicCamera
{
	Q_OBJECT
  public:
	DebugCamera(VRHandler const* vrHandler);
	virtual void update() override;
	void renderCamera(BasicCamera const* cam);

  public slots:
	bool isEnabled() const;
	void setEnabled(bool enabled);
	void toggle();
	bool debugInHeadset() const;
	void setDebugInHeadset(bool debuginheadset);
	void toggleDebugInHeadset();
	bool followHMD() const;
	void setFollowHMD(bool followhmd);
	void toggleFollowHMD();

  private:
	GLHandler::Mesh camMesh;
	GLHandler::ShaderProgram camMeshShader;
};

#endif // DEBUGCAMERA_H
