/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@hotmail.fr>

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

#ifndef GRAPHICS_UTILS_HPP
#define GRAPHICS_UTILS_HPP

#include <QColor>
#include <QVector3D>

#include "math/Vector3.hpp"
#include "physics/Color.hpp"

namespace Utils
{
QVector3D toQt(Vector3 const& vector);
Vector3 fromQt(QVector3D const& vector);
QColor toQt(Color const& color);
Color fromQt(QColor const& color);
} // namespace Utils

#endif // GRAPHICS_UTILS_HPP
