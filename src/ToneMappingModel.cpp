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

#include "ToneMappingModel.hpp"

ToneMappingModel::ToneMappingModel(VRHandler const* vrHandler)
    : vrHandler(vrHandler)
{
	double inter(getConeRodIntersectionTime(0.0));
	coneDominationEnd  = inter - 0.5 * mixDuration;
	rodDominationBegin = inter + 0.5 * mixDuration;
}

void ToneMappingModel::autoUpdateExposure(float averageLuminance,
                                          float frameTiming)
{
	if(!autoexposure)
	{
		return;
	}

	frameTiming *= autoexposuretimecoeff;

	float aec
	    = (averageLuminance > 1000.0
	           ? 2.0
	           : (averageLuminance < 1.0 ? 0.002 : averageLuminance / 500.0));
	aec *= autoexposurecoeff;

	const float limit(*vrHandler ? 350.0 * aec : 1000.0 * aec);
	averageLuminance *= exposure;

	// simple model if all goes wrong
	/*if(averageLuminance > limit)
	{
	    exposure /= pow(100.f, frameTiming);
	}
	else if(averageLuminance < limit)
	{
	    exposure *= pow(10.f, frameTiming);
	}*/

	// Curve from :
	// https://www.researchgate.net/figure/Dark-adaptation-curves-for-a-normal-subject-40-years-of-age-obtained-after-a-96_fig2_5677883

	// light adaptation
	if(averageLuminance > limit)
	{
		if(frameTiming >= 0.1)
		{
			exposure *= limit / averageLuminance;
			autoExposureState = AutoExposureState::MATCHING;
		}
		else
		{
			if(autoExposureState != AutoExposureState::INCREASING)
			{
				time              = invLAthreshold(1.0 / exposure);
				autoExposureState = AutoExposureState::INCREASING;
			}
			double minVisibleLum(1.0 / exposure);
			minVisibleLum += frameTiming * dLAthreshold(time);
			exposure = 1.0 / minVisibleLum;
		}
	}
	// dark adaptation
	else if(averageLuminance < limit)
	{
		// if log threshold > 0 and begin DA, immediate adaptation
		if(frameTiming >= 1.0 || (exposure < 1.0 && frameTiming >= 0.1))
		{
			exposure *= limit / averageLuminance;
			if(exposure > 1.0)
			{
				exposure = 1.0;
			}
			autoExposureState = AutoExposureState::DECREASING;
		}
		else if(exposure < 1.0)
		{
			// use light adaptation backwards... TODO find better
			if(autoExposureState != AutoExposureState::FASTDECREASING)
			{
				time              = invLAthreshold(1.0 / exposure);
				autoExposureState = AutoExposureState::FASTDECREASING;
			}
			double minVisibleLum(1.0 / exposure);
			minVisibleLum -= frameTiming * dLAthreshold(time);
			exposure = 1.0 / minVisibleLum;
		}
		else
		{
			if(autoExposureState != AutoExposureState::DECREASING)
			{
				time              = 0.0;
				startLogLuminance = log10(1.0 / exposure);
				autoExposureState = AutoExposureState::DECREASING;
				double tInter(getConeRodIntersectionTime(startLogLuminance));
				mixDuration        = tInter / 6.0;
				coneDominationEnd  = tInter - 0.5 * mixDuration;
				rodDominationBegin = tInter + 0.5 * mixDuration;
			}
			exposure = 1.0 / threshold(time);
		}
	}
	time += frameTiming;

	// 1 / (min scotopic vision)
	if(!*vrHandler && exposure > 2e4f)
	{
		exposure = 2e4f;
	}
	else if(*vrHandler && exposure > 5e3f)
	{
		exposure = 5e3f;
	}
	// 1 / (max photopic vision)
	if(exposure < 1e-4f)
	{
		exposure = 1e-4f;
	}
}

double ToneMappingModel::coneThreshold(double t)
{
	const double a = startLogLuminance;
	const double b = minConeVisibleLogLuminance;
	return pow(10, (a - b) * exp(-t / 60.0) + b);
}

double ToneMappingModel::dConeThreshold(double t)
{
	const double a = startLogLuminance;
	const double b = minConeVisibleLogLuminance;
	return (a - b) * (-1.0 / 60.0) * exp(-t / 60.0) * log(10)
	       * coneThreshold(t);
}

/*double ToneMappingModel::invConeThreshold(double lum)
{
    return -60.0 * log((log10(lum) + 2.5) / 2.5);
}*/

double ToneMappingModel::rodThreshold(double t)
{
	const double tInterMinutes(getConeRodIntersectionTime(startLogLuminance)
	                           / 60.0);
	const double a = minConeVisibleLogLuminance;
	const double b = minVisibleLogLuminance;
	const double c = tInterMinutes;
	const double d = -0.14;
	return pow(10, (a - b) * exp(d * (t / 60.0 - c)) + b);
}

double ToneMappingModel::dRodThreshold(double t)
{
	const double tInterMinutes(getConeRodIntersectionTime(startLogLuminance)
	                           / 60.0);
	const double a = minConeVisibleLogLuminance;
	const double b = minVisibleLogLuminance;
	const double c = tInterMinutes;
	const double d = -0.14;
	return (a - b) * (d / 60.0) * exp(d * (t / 60.0 - c)) * log(10)
	       * rodThreshold(t);
}

/*double ToneMappingModel::invRodThreshold(double lum)
{
    return 60.0
           * (-log((log10(lum) - minVisibleLogLuminance)
                   / (-minVisibleLogLuminance - 2.5))
                  / 0.18
              + 12);
}*/

double ToneMappingModel::mixThreshold(double t)
{
	// Hermit polynomial
	const double tHerm((t - coneDominationEnd)
	                   / (rodDominationBegin - coneDominationEnd));
	const double p0(coneThreshold(coneDominationEnd)),
	    m0 = dConeThreshold(coneDominationEnd);
	const double p1(rodThreshold(rodDominationBegin)),
	    m1 = dRodThreshold(rodDominationBegin);

	const double ttHerm(tHerm * tHerm), tttHerm(tHerm * ttHerm),
	    h00(2 * tttHerm - 3 * ttHerm + 1), h10(tttHerm - 2 * ttHerm + tHerm),
	    h01(-2 * tttHerm + 3 * ttHerm), h11(tttHerm - ttHerm);

	return h00 * p0 + h10 * (rodDominationBegin - coneDominationEnd) * m0
	       + h01 * p1 + h11 * (rodDominationBegin - coneDominationEnd) * m1;
}

double ToneMappingModel::dMixThreshold(double t)
{
	// Hermit polynomial
	const double tHerm((t - coneDominationEnd)
	                   / (rodDominationBegin - coneDominationEnd));
	const double p0(coneThreshold(coneDominationEnd)),
	    m0 = dConeThreshold(coneDominationEnd);
	const double p1(rodThreshold(rodDominationBegin)),
	    m1 = dRodThreshold(rodDominationBegin);

	const double ttHerm(tHerm * tHerm), h00(6 * ttHerm - 6 * tHerm),
	    h10(3 * ttHerm - 4 * tHerm + 1), h01(-6 * ttHerm + 6 * tHerm),
	    h11(3 * ttHerm - 2 * tHerm);

	return (h00 * p0 + h10 * (rodDominationBegin - coneDominationEnd) * m0
	        + h01 * p1 + h11 * (rodDominationBegin - coneDominationEnd) * m1)
	       / 60.0;
}

/*double ToneMappingModel::invMixThreshold(double lum)
{
    // Newton's method
    double t0 = 12.0 * 60.0;
    for(unsigned int i(0); i < 10; ++i)
    {
        t0 -= (mixThreshold(t0) - lum) / dMixThreshold(t0);
    }
    return t0;
}*/

double ToneMappingModel::getConeRodIntersectionTime(double startLogLuminance)
{
	return 60.0 * 5 * (2.4 + startLogLuminance);
}

double ToneMappingModel::threshold(double t)
{
	// if cones dominant
	if(t < coneDominationEnd)
	{
		return coneThreshold(t);
	}
	// if mixed cones/rods
	if(t < rodDominationBegin)
	{
		return mixThreshold(t);
	}
	// if rods dominant
	return rodThreshold(t);
}

double ToneMappingModel::dThreshold(double t)
{
	// if cones dominant
	if(t < coneDominationEnd)
	{
		return dConeThreshold(t);
	}
	// if mixed cones/rods
	if(t < rodDominationBegin)
	{
		return dMixThreshold(t);
	}
	// if rods dominant
	return dRodThreshold(t);
}

/*double ToneMappingModel::invThreshold(double lum)
{
    // if rods dominant
    if(lum < rodThreshold(rodDominationBegin))
    {
        return invRodThreshold(lum);
    }
    // else if mixed cones/rods
    else if(lum < coneThreshold(coneDominationEnd))
    {
        return invMixThreshold(lum);
    }
    // else if cones dominant
    else
    {
        return invConeThreshold(lum);
    }
}*/

double ToneMappingModel::LAthreshold(double t)
{
	// Neural mechanism of visual information degradation from retina to V1 area
	// Haixin Zhong & Rubin Wang
	// 2020

	return pow(10,
	           2.65 * (2.77 * exp(0.02 * t / 60.0) - 2.76 * exp(-5.42 * t / 60))
	               - 5);
}

double ToneMappingModel::dLAthreshold(double t)
{
	return (39.6419 + 0.14681 * exp(5.44 * t / 60.0)) / exp(5.42 * t / 60.0)
	       * LAthreshold(t) * log(10.0) / 60.0;
}

double ToneMappingModel::invLAthreshold(double lum)
{
	// Newton's method
	double t0 = 1.0 * 60.0;
	for(unsigned int i(0); i < 100; ++i)
	{
		double dt((LAthreshold(t0) - lum) / dLAthreshold(t0));
		if(dt > 10.0)
		{
			dt = 10.0;
		}
		else if(dt < -10.0)
		{
			dt = -10.0;
		}
		t0 -= dt;
		if(abs(dt) < 0.1)
		{
			break;
		}
		if(t0 < 1.0)
		{
			t0 = 1.0;
		}
		else if(t0 > 600.0)
		{
			t0 = 600.0;
		}
	}
	return t0;
}

/*double ToneMappingModel::dLumOverdTAtGivenLum(double minVisibleLum)
{
    return dThreshold(invThreshold(minVisibleLum));
}*/

double ToneMappingModel::dLALumOverdTAtGivenLum(double minVisibleLum)
{
	return dLAthreshold(invLAthreshold(minVisibleLum));
}
