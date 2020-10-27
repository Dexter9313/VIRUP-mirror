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

#ifndef TONEMAPPINGMODEL_HPP
#define TONEMAPPINGMODEL_HPP

#include "vr/VRHandler.hpp"

/** @ingroup pycall
 *
 * @brief The model that transforms Hight Dynamic Range rendering values into
 * RGB displayable colors.
 *
 * Callable in Python as the "ToneMappingModel" object.
 */
class ToneMappingModel : public QObject
{
	Q_OBJECT
	/**
	 * @brief Multiplier on all color components before post-processing.
	 */
	Q_PROPERTY(float exposure MEMBER exposure)
	/**
	 * @brief Factor between the smallest color component value and highest
	 * color component value.
	 *
	 * A dynamic range of 1 means that no mapping will be performed (use this
	 * value if your shaders output from 0 to 1 already). In this case,
	 * exposure is ignored.
	 *
	 * For example, a dynamicrange of 10000 means that color components values
	 * from [1 ; 10000] will be mapped to [0.5/255 ; 1]. (If the screen dynamic
	 * range is 255, which is always the case in non-HDR hardware.)
	 */
	Q_PROPERTY(float dynamicrange MEMBER dynamicrange)
	/**
	 * @brief Apply Purkinje effect or not for dark scenes.
	 */
	Q_PROPERTY(bool purkinje MEMBER purkinje)
	/**
	 * @brief Is automatic exposure active or not.
	 */
	Q_PROPERTY(bool autoexposure MEMBER autoexposure)
	/**
	 * @brief Modifier for the autoexposure target.
	 *
	 * Use this if the value autoexposure targets is too dark or too bright to
	 * your taste. This target depends on the environment luminance, so be sure
	 * to check both highly lit and dark scenes.
	 */
	Q_PROPERTY(float autoexposurecoeff MEMBER autoexposurecoeff)
	/**
	 * @brief Modifier for the eye adaptation algorithm speed.
	 *
	 * By default (value 1.f), eye adaptation speed is realistic : it will take
	 * a few dozen minutes to fully adapt to dark conditions and a few dozen
	 * seconds to fully adapt to bright conditions.
	 */
	Q_PROPERTY(float autoexposuretimecoeff MEMBER autoexposuretimecoeff)

  public:
	ToneMappingModel(VRHandler const& vrHandler);
	void autoUpdateExposure(float averageLuminance, float frameTiming);

	float exposure     = 1.f;
	float dynamicrange = 1.f;
	bool purkinje      = false;

	bool autoexposure           = false;
	float autoexposurecoeff     = 1.f;
	float autoexposuretimecoeff = 1.f;

  private:
	VRHandler const& vrHandler;

	/*
	 * BEG AUTOEXPOSURE MODEL
	 */
	enum AutoExposureState
	{
		MATCHING,
		INCREASING,
		DECREASING,
		FASTDECREASING,
	};

	// absolute minimum visible log of luminance (log10 cd/m^2)
	const double minVisibleLogLuminance     = -5.0;
	const double minConeVisibleLogLuminance = -2.5;

	double time                         = 0.0;
	double startLogLuminance            = -2.0;
	AutoExposureState autoExposureState = AutoExposureState::MATCHING;
	double coneDominationEnd            = 10.0 * 60.0;
	double rodDominationBegin           = 14.0 * 60.0;
	// duration for Hermit interpolator between cones and rods being used
	double mixDuration = 2.0 * 60.0; // s

	double coneThreshold(double t);
	double dConeThreshold(double t); // derivative
	// double invConeThreshold(double lum); // inverse
	double rodThreshold(double t);
	double dRodThreshold(double t); // derivative
	// double invRodThreshold(double lum); // inverse
	double mixThreshold(double t);
	double dMixThreshold(double t); // derivative
	// double invMixThreshold(double lum); // inverse

	static double getConeRodIntersectionTime(double startLogLuminance);

	double threshold(double t);
	double dThreshold(double t); // derivative
	// double invThreshold(double lum); // inverse

	// LA=light adaptation
	double LAthreshold(double t);
	double dLAthreshold(double t);
	double invLAthreshold(double lum);

	// temporal variation of minimum visible luminance given the current minimum
	// visible luminance (cd/m^2) computed using :
	// https://www.researchgate.net/figure/Dark-adaptation-curves-for-a-normal-subject-40-years-of-age-obtained-after-a-96_fig2_5677883
	// double dLumOverdTAtGivenLum(double minVisibleLum);

	double dLALumOverdTAtGivenLum(double minVisibleLum);
	/*
	 * END AUTOEXPOSURE MODEL
	 */
};

#endif // TONEMAPPINGMODEL_HPP
