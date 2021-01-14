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

#include "movingcube/MovingCube.hpp"

MovingCube::MovingCube()
    : cubeShader("default")
{
	cubeShader.setUniform("alpha", 0.5f);
	cubeShader.setUniform("color", QColor::fromRgbF(1.0f, 1.0f, 1.0f));

	std::vector<float> vertices = cubeVertices(0);

	std::vector<unsigned int> elements = {
	    0, 1, 0, 2, 0, 4,

	    7, 6, 7, 5, 7, 3,

	    1, 3, 1, 5,

	    2, 6, 2, 3,

	    4, 6, 4, 5,
	};
	cube.setVertexShaderMapping(cubeShader, {{"position", 3}});
	cube.setVertices(vertices, elements);

	cubeTimer.start();
}

void MovingCube::update()
{
	cube.setVertices(cubeVertices(cubeTimer.elapsed()));
}

void MovingCube::render()
{
	GLHandler::setUpRender(cubeShader);
	cube.render(PrimitiveType::LINES);
}

std::vector<float> MovingCube::cubeVertices(uint64_t dt)
{
	float dtf = dt / 2000.f;

	std::vector<float> result = {
	    -1.0f, -1.0f, -1.0f, // 0
	    -1.0f, -1.0f, 1.0f,  // 1
	    -1.0f, 1.0f,  -1.0f, // 2
	    -1.0f, 1.0f,  1.0f,  // 3
	    1.0f,  -1.0f, -1.0f, // 4
	    1.0f,  -1.0f, 1.0f,  // 5
	    1.0f,  1.0f,  -1.0f, // 6
	    1.0f,  1.0f,  1.0f,  // 7
	};

	for(unsigned int i(0); i < 24; i += 3)
	{
		result[i] *= cosf(dtf);
	}
	for(unsigned int i(1); i < 24; i += 3)
	{
		result[i] *= sinf(dtf);
	}

	return result;
}

