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

#include "../../include/physics/Color.hpp"

Color::Color()
    : Color(0, 0, 0)
{
}

Color::Color(unsigned int r, unsigned int g, unsigned int b)
    : Color(255, r, g, b)
{
}

Color::Color(unsigned int alpha, unsigned int r, unsigned int g, unsigned int b)
    : alpha(alpha)
    , r(r)
    , g(g)
    , b(b)
{
}

Color::Color(QJsonObject const& json, Color const& defaultValue)

    : Color(json["alpha"].toInt(defaultValue.alpha),
            json["r"].toInt(defaultValue.r), json["g"].toInt(defaultValue.g),
            json["b"].toInt(defaultValue.b))
{
}

QJsonObject Color::getJSONRepresentation() const
{
	return QJsonObject({{"r", static_cast<int>(r)},
	                    {"g", static_cast<int>(g)},
	                    {"b", static_cast<int>(b)},
	                    {"alpha", static_cast<int>(alpha)}});
}
