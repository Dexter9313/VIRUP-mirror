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

#ifndef UNIVERSALTIME_HPP
#define UNIVERSALTIME_HPP

#include <boost/predef.h>
#include <boost/preprocessor/stringize.hpp>
#pragma message(BOOST_PP_STRINGIZE(__GNUC__))
#pragma message(BOOST_PP_STRINGIZE(__clang__))

#if defined(__GNUC__) && !defined(__clang__)
#pragma message("Using float128")
#include <boost/multiprecision/float128.hpp>

// TODO see if enforce strong typing

/*! Defines a type wide enough to hold big time lengths with
 * enough precision.
 */
typedef boost::multiprecision::float128 UniversalTime;
#else
#pragma message("Using cpp_dec_float_50")
#include <boost/multiprecision/cpp_dec_float.hpp>
typedef boost::multiprecision::cpp_dec_float_50
    UniversalTime; // log10(2^113) ~= 34, so 35 dec digits
#endif
#endif // UNIVERSALTIME_HPP
