/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@epfl.ch>

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

#include "Primitives.hpp"

GLHandler::Mesh Primitives::newUnitCube(GLHandler::ShaderProgram shader,
                                        GLHandler::PrimitiveType primitiveType)
{
	std::vector<float> vertices = {
	    -0.5f, -0.5f, -0.5f, // 0
	    -0.5f, -0.5f, 0.5f,  // 1
	    -0.5f, 0.5f,  -0.5f, // 2
	    -0.5f, 0.5f,  0.5f,  // 3
	    0.5f,  -0.5f, -0.5f, // 4
	    0.5f,  -0.5f, 0.5f,  // 5
	    0.5f,  0.5f,  -0.5f, // 6
	    0.5f,  0.5f,  0.5f,  // 7
	};

	GLHandler::Mesh result(GLHandler::newMesh());

	if(primitiveType == GLHandler::PrimitiveType::POINTS)
	{
		GLHandler::setVertices(result, vertices, shader, {{"position", 3}});
		return result;
	}

	std::vector<unsigned int> elements;

	if(primitiveType == GLHandler::PrimitiveType::LINES)
	{
		elements = {
		    0, 1, 0, 2, 0, 4,

		    7, 6, 7, 5, 7, 3,

		    1, 3, 1, 5,

		    2, 6, 2, 3,

		    4, 6, 4, 5,
		};
	}
	else if(primitiveType == GLHandler::PrimitiveType::TRIANGLES)
	{
		// as seen from (1, 0, 0) with up being (0, 0, 1)
		elements = {
		    0, 1, 2, // back0
		    3, 2, 1, // back1
		    6, 5, 4, // front0
		    7, 4, 5, // front1
		    0, 1, 4, // left0
		    5, 4, 1, // left1
		    2, 6, 3, // right0
		    7, 3, 6, // right1
		    0, 2, 4, // down0
		    6, 4, 2, // down1
		    1, 3, 5, // up0
		    7, 5, 3, // up1
		};
	}
	else if(primitiveType == GLHandler::PrimitiveType::TRIANGLE_STRIP
	        || primitiveType == GLHandler::PrimitiveType::AUTO)
	{
		// see : Optimizing Triangle Strips for Fast Rendering, Francine Evans,
		// Steven Skiena, Amitabh Varshney,
		// http://www.cs.umd.edu/gvil/papers/av_ts.pdf
		//
		// NOTATION :
		// their index : +/-(0.5), +/-(0.5), +/-(0.5) = our index
		//
		// if we define
		// 1 : -, -, - = 0
		// and their 1,3,8,5 to be the bottom face then :
		// 1 : -, -, - = 0
		// 2 : -, -, + = 1
		// 3 : +, -, - = 4
		// 4 : +, -, + = 5
		// 5 : -, +, - = 2
		// 6 : -, +, + = 3
		// 7 : +, +, + = 7
		// 8 : +, +, - = 6
		//
		// Their strip order is : 4 3 7 8 5 3 1 4 2 7 6 5 2 1,
		// thus :
		elements = {5, 4, 7, 6, 2, 4, 0, 5, 1, 7, 3, 2, 1, 0};
	}
	GLHandler::setVertices(result, vertices, shader, {{"position", 3}},
	                       elements);
	return result;
}

GLHandler::Mesh Primitives::newUnitSphere(
    GLHandler::ShaderProgram shader, unsigned int latDivisions,
    unsigned int lonDivisions, GLHandler::PrimitiveType primitiveType)
{
	std::vector<float> vertices;
	std::vector<unsigned int> elements;

	// "north pole"
	vertices.push_back(0.f);
	vertices.push_back(0.f);
	vertices.push_back(1.f);

	// link to first latitude
	if(primitiveType == GLHandler::PrimitiveType::TRIANGLES)
	{
		// all except last point
		for(unsigned int i(0); i < lonDivisions - 1; ++i)
		{
			elements.push_back(0);
			elements.push_back(i + 1);
			elements.push_back(i + 2);
		}
		elements.push_back(0);
		elements.push_back(lonDivisions);
		elements.push_back(1);
	}
	else if(primitiveType == GLHandler::PrimitiveType::LINES)
	{
		for(unsigned int i(0); i < lonDivisions; ++i)
		{
			elements.push_back(0);
			elements.push_back(i + 1);
		}
	}

	for(unsigned int i(0); i < latDivisions; ++i)
	{
		float lat = (static_cast<float>(i + 1) / (latDivisions + 1)) * M_PI;

		float cosLat(std::cos(lat)), sinLat(std::sin(lat));

		for(unsigned int j(0); j < lonDivisions; ++j)
		{
			float lon = 2 * M_PI * static_cast<float>(j) / lonDivisions;
			vertices.push_back(sinLat * std::cos(lon));
			vertices.push_back(sinLat * std::sin(lon));
			vertices.push_back(cosLat);

			// elements
			if(primitiveType == GLHandler::PrimitiveType::TRIANGLES)
			{
				// don't do anything on first latitude
				if(i == 0)
				{
					continue;
				}

				if(j != lonDivisions - 1) // if not last point
				{
					elements.push_back(latDivisions * i + j + 1);
					elements.push_back(latDivisions * i + j + 2);
					elements.push_back(latDivisions * (i - 1) + j + 1);

					elements.push_back(latDivisions * (i - 1) + j + 2);
					elements.push_back(latDivisions * (i - 1) + j + 1);
					elements.push_back(latDivisions * i + j + 2);
				}
				else
				{
					elements.push_back(latDivisions * i + j + 1);
					elements.push_back(latDivisions * i + 1);
					elements.push_back(latDivisions * (i - 1) + j + 1);

					elements.push_back(latDivisions * (i - 1) + 1);
					elements.push_back(latDivisions * (i - 1) + j + 1);
					elements.push_back(latDivisions * i + 1);
				}
			}
			else if(primitiveType == GLHandler::PrimitiveType::LINES)
			{
				// draw latitude line
				if(j != lonDivisions - 1) // if not last point
				{
					elements.push_back(latDivisions * i + j + 1);
					elements.push_back(latDivisions * i + j + 2);
				}
				else
				{
					elements.push_back(latDivisions * i + j + 1);
					elements.push_back(latDivisions * i + 1);
				}
				// draw longitude line
				if(i != 0) // if not first latitude
				{
					elements.push_back(latDivisions * i + j + 1);
					elements.push_back(latDivisions * (i - 1) + j + 1);
				}
			}
		}
	}

	// link last latitude to south pole
	unsigned int southPole(latDivisions * lonDivisions + 1);
	if(primitiveType == GLHandler::PrimitiveType::TRIANGLES)
	{
		for(unsigned int i(0); i < lonDivisions - 1; ++i)
		{
			elements.push_back(southPole);
			elements.push_back(southPole - i - 1);
			elements.push_back(southPole - i - 2);
		}
		elements.push_back(southPole);
		elements.push_back(southPole - lonDivisions);
		elements.push_back(southPole - 1);
	}
	else if(primitiveType == GLHandler::PrimitiveType::LINES)
	{
		for(unsigned int i(0); i < lonDivisions; ++i)
		{
			elements.push_back(southPole);
			elements.push_back(southPole - i - 1);
		}
	}

	// "south pole"
	vertices.push_back(0.f);
	vertices.push_back(0.f);
	vertices.push_back(-1.f);

	GLHandler::Mesh result(GLHandler::newMesh());
	GLHandler::setVertices(result, vertices, shader, {{"position", 3}},
	                       elements);

	return result;
}
