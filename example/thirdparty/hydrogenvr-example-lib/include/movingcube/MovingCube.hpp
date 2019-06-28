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

#ifndef MOVINGCUBE_HPP
#define MOVINGCUBE_HPP

#include <QElapsedTimer>

#include "GLHandler.hpp"

class MovingCube
{
  public:
	MovingCube();
	void update();
	void render();
	~MovingCube();

  private:
	GLHandler::Mesh cube = {};
	GLHandler::ShaderProgram cubeShader;
	QElapsedTimer cubeTimer;

	static std::vector<float> cubeVertices(uint64_t dt);
	GLHandler::Mesh createCube(GLHandler::ShaderProgram const& shader);
};

#endif // MOVINGCUBE_HPP
