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

#ifndef GRID_HPP
#define GRID_HPP

#include "Camera.hpp"
#include "GLHandler.hpp"
#include "Text3D.hpp"
#include "graphics/OrbitalSystemCamera.hpp"
#include "graphics/renderers/CelestialBodyRenderer.hpp"

/** @ingroup pycall
 *
 * Callable in Python as the "Grid" object.
 */
class Grid : public QObject
{
	Q_OBJECT
	/**
	 * @brief The grid color.
	 *
	 * @accessors getColor(), setColor()
	 */
	Q_PROPERTY(QColor color READ getColor WRITE setColor)
  public:
	Grid();
	QColor getColor() const
	{
		return QSettings().value("misc/gridcolor").value<QColor>();
	}
	void setColor(QColor const& color);
	// void renderCosmo(Camera const& cam);
	// void renderPlanet(OrbitalSystemCamera const& cam);
	void render(double scale, double height);
	~Grid();

  private:
	GLHandler::ShaderProgram shader;
	GLHandler::Mesh mesh = {};

	QString scaleText  = QString();
	Text3D scaleText3D = Text3D(1024, 256);

	static void generateGridVertices(std::vector<float>& vertices,
	                                 std::vector<unsigned int>& elements);
	static double customRound(double x, double scale = 1.0);
	/*static QString generateScaleTextCosmo(double scale);
	static QString generateScaleTextPlanet(double scale);*/
	static QString generateScaleText(double scale);
};

#endif // GRID_HPP
