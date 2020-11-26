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

#include "AbstractState.hpp"
#include "BasicCamera.hpp"
#include "Primitives.hpp"
#include "Text3D.hpp"
#include "gl/GLMesh.hpp"

class CalibrationCompass
{
  public:
	class State : public AbstractState
	{
	  public:
		State()                   = default;
		State(State const& other) = default;
		State(State&& other)      = default;
		virtual void readFromDataStream(QDataStream& stream) override
		{
			stream >> compassForceTickRes;
			stream >> compassProtractor;
			stream >> servHFOV;
			stream >> servRTWidth;
		};
		virtual void writeInDataStream(QDataStream& stream) override
		{
			stream << compassForceTickRes;
			stream << compassProtractor;
			stream << servHFOV;
			stream << servRTWidth;
		};

		double compassForceTickRes = 0.0;
		bool compassProtractor     = false;
		float servHFOV             = 70.f;
		unsigned int servRTWidth   = 1920;
	};

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

	static void readState(AbstractState const& s)
	{
		auto const& state         = dynamic_cast<State const&>(s);
		forcedTickResolution()    = state.compassForceTickRes;
		forceProtractorMode()     = state.compassProtractor;
		serverHorizontalFOV()     = state.servHFOV;
		serverRenderTargetWidth() = state.servRTWidth;
	}
	static void writeState(AbstractState& s)
	{
		auto& state               = dynamic_cast<State&>(s);
		state.compassForceTickRes = forcedTickResolution();
		state.compassProtractor   = forceProtractorMode();
		state.servHFOV            = serverHorizontalFOV();
		state.servRTWidth         = serverRenderTargetWidth();
	}

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
