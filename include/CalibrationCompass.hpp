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

#ifndef CALIBRATIONCOMPASS_HPP
#define CALIBRATIONCOMPASS_HPP

#include "BasicCamera.hpp"
#include "Primitives.hpp"
#include "Text3D.hpp"
#include "gl/GLMesh.hpp"

class CalibrationCompass
{
  public:
	CalibrationCompass();
	void render(QMatrix4x4 const& angleShiftMat);

	static double& forcedTickResolution();
	static bool& forceProtractorMode();
	static float& serverHorizontalFOV();
	static unsigned int& serverRenderTargetWidth();
	static double
	    getDoubleFarRightPixelSubtendedAngle(float horizontalFOV,
	                                         unsigned int renderTargetWidth);
	static double getCurrentTickResolution();
	~CalibrationCompass();

	float exposure     = 1.f;
	float dynamicrange = 1.f;

  private:
	GLShaderProgram shader;
	GLMesh circle;
	std::vector<Text3D*> billboards;

	void renderCircle(QMatrix4x4 const& angleShiftMat, float latitude);
	void renderCompassTicks(QMatrix4x4 const& angleShiftMat,
	                        float heightMultiplier, double deltaDeg,
	                        bool labels = false);
};

#endif // CALIBRATIONCOMPASS_HPP
