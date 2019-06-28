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

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <boost/math/constants/constants.hpp>

/*! \ingroup math
 * Contains all mathematical and physical constants
 *
 * See \ref math group description for conventions and notations.
 */
namespace constant
{
/*! [Pi](https://en.wikipedia.org/wiki/Pi)
 */
static const double pi = boost::math::constants::pi<double>();

/*! [Gravitationnal
 * constant](https://en.wikipedia.org/wiki/Gravitational_constant)
 */
static const double G = 6.67408e-11;

/*! [Not a number](https://en.wikipedia.org/wiki/NaN), result of invalid
 * mathematical operations
 */
static const double NaN = nan("");
} // namespace constant

#endif // CONSTANTS_HPP
