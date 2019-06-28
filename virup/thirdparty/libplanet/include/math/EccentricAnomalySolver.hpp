/*
    Copyright (C) 2018 Florian Cabot <florian.cabot@hotmail.fr>

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

#ifndef ECCENTRICANOMALYSOLVER_H
#define ECCENTRICANOMALYSOLVER_H

#include <array>
#include <boost/math/tools/roots.hpp>
#include <limits>
#include <tuple>

#include "constants.hpp"
#include "math/MathUtils.hpp"

/*! \ingroup math
 * Solves [Kepler's Equation](https://en.wikipedia.org/wiki/Kepler%27s_equation)
 * and its variants.
 *
 * See \ref math group description for conventions and notations.
 *
 * From mean anomaly and eccentricity, finds the eccentric anomaly (or
 * equivalent notion if it's not defined, see #solveForParabolicOrbit). Three
 * orbit cases are considered : elliptic, parabolic and hyperbolic.
 * This class is not meant to be instanciated.
 */
class EccentricAnomalySolver
{
  public:
	/*! Solves the original
	 * [Kepler's Equation](https://en.wikipedia.org/wiki/Kepler%27s_equation)
	 * to find the eccentric anomaly of an elliptic orbit from mean anomaly and
	 * eccentricity.
	 *
	 * \param meanAnomaly known mean anomaly
	 * \param eccentricity known eccentricity
	 */
	static double solveForEllipticOrbit(double meanAnomaly,
	                                    double eccentricity);

	/*! Solves
	 * [Barker's
	 * Equation](https://en.wikipedia.org/wiki/Parabolic_trajectory#Barker%27s_equation)
	 *
	 * Returned "eccentric anomaly" is actually D=tan(nu/2) as defined in
	 * source, nu being the
	 * true anomaly.
	 *
	 * Equation has been a bit arranged so that we have 6*meanAnomaly = 3*D +
	 * D^3,
	 * which is a cubic equation that can be solved by the
	 * [Cardano's
	 * method](https://en.wikipedia.org/wiki/Cubic_function#Cardano%27s_method).
	 *
	 * \param meanAnomaly is actually sqrt(mu / p^3) * (t-T)  as defined in
	 * source, see \ref Orbit::getMeanAnomalyAtUT for more details
	 */
	static double solveForParabolicOrbit(double meanAnomaly);

	/*! Solves the
	 * [hyperbolic
	 * variant](https://en.wikipedia.org/wiki/Kepler%27s_equation#Hyperbolic_Kepler_equation)
	 * of Kepler's Equation
	 * to find the eccentric anomaly of an hyperbolic orbit from mean anomaly
	 * and eccentricity.
	 *
	 * \param meanAnomaly known mean anomaly
	 * \param eccentricity known eccentricity
	 */
	static double solveForHyperbolicOrbit(double meanAnomaly,
	                                      double eccentricity);

  private:
	EccentricAnomalySolver() = delete;
};

#endif // ECCENTRICANOMALYSOLVER_H
