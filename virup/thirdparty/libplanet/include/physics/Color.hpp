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

#ifndef COLOR_HPP
#define COLOR_HPP

#include <QJsonObject>

/*! \ingroup phys
 * An
 * [ARGB-represented](https://en.wikipedia.org/wiki/RGBA_color_space#ARGB_(word-order))
 * color.
 *
 * See \ref phys group description for conventions and notations.
 */
class Color
{
  public:
	/*! Default constructor
	 *
	 * Constructs opaque black.
	 */
	Color();
	/*! RGB constructor
	 *
	 * Constructs an opaque color from three RGB values.
	 *
	 * \param r Red channel with value from 0 (no contribution) to 255 (max
	 * contribution).
	 * \param g Green channel with value from 0 (no contribution) to 255 (max
	 * contribution).
	 * \param b Blue channel with value from 0 (no contribution) to 255 (max
	 * contribution).
	 */
	Color(unsigned int r, unsigned int g, unsigned int b);

	/*! RGB constructor
	 *
	 * Constructs a color from four ARGB values.
	 *
	 * \param alpha Alpha channel with value from 0 (no contribution) to 255
	 * (max contribution).
	 *
	 * \param r Red channel with value from 0 (no contribution) to 255 (max
	 * contribution).
	 * \param g Green channel with value from 0 (no contribution) to 255 (max
	 * contribution).
	 * \param b Blue channel with value from 0 (no contribution) to 255 (max
	 * contribution).
	 */
	Color(unsigned int alpha, unsigned int r, unsigned int g, unsigned int b);

	Color(QJsonObject const& json,
	      Color const& defaultValue = Color(255, 0, 0, 0));

	QJsonObject getJSONRepresentation() const;

	/*!
	 * Alpha channel with value from 0 (no contribution) to 255
	 * (max contribution).
	 */
	unsigned int alpha;
	/*! Red channel with value from 0 (no contribution) to 255 (max
	 * contribution).
	 */
	unsigned int r;
	/*! Green channel with value from 0 (no contribution) to 255 (max
	 * contribution).
	 */
	unsigned int g;
	/*! Blue channel with value from 0 (no contribution) to 255 (max
	 * contribution).
	 */
	unsigned int b;
};

#endif // COLOR_HPP
