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

#include "graphics/renderers/Rings.hpp"

Rings::Rings(float seed, float innerRing, float outerRing, float planetradius,
             QVector3D const& planetoblateness, QString const& texturePath)
{
	shader = GLHandler::newShader("planet/ring");
	mesh   = GLHandler::newMesh();

	GLHandler::setShaderParam(shader, "inner", innerRing);
	GLHandler::setShaderParam(shader, "outer", outerRing);
	GLHandler::setShaderParam(shader, "planetradius", planetradius);
	GLHandler::setShaderParam(shader, "planetoblateness", planetoblateness);

	float coeff(1.f / cos(3.1415 / 20.0));

	std::vector<float> ringVertices;
	std::vector<unsigned int> ringElements;
	for(unsigned int i(0); i < 20; ++i)
	{
		// 2 * i
		ringVertices.push_back(innerRing * cos(i * 3.1415f / 10.f));
		ringVertices.push_back(innerRing * sin(i * 3.1415f / 10.f));

		// 2 * i + 1
		ringVertices.push_back(outerRing * coeff * cos(i * 3.1415f / 10.f));
		ringVertices.push_back(outerRing * coeff * sin(i * 3.1415f / 10.f));

		if(i != 0)
		{
			ringElements.push_back(2 * (i - 1));
			ringElements.push_back(2 * (i - 1) + 1);
			ringElements.push_back(2 * i + 1);

			ringElements.push_back(2 * (i - 1));
			ringElements.push_back(2 * i + 1);
			ringElements.push_back(2 * i);
		}
	}
	ringElements.push_back(2 * (20 - 1));
	ringElements.push_back(2 * (20 - 1) + 1);
	ringElements.push_back(1);

	ringElements.push_back(2 * (20 - 1));
	ringElements.push_back(1);
	ringElements.push_back(0);

	GLHandler::setVertices(mesh, ringVertices, shader, {{"position", 2}},
	                       ringElements);

	if(texturePath == "")
	{
		texTarget = GLHandler::newRenderTarget(16000, 1);
		update(seed);
	}
	else
	{
		textured = true;
		tex      = GLHandler::newTexture(texturePath.toLatin1().data());
	}
}

GLHandler::Texture Rings::getTexture()
{
	return textured ? tex : GLHandler::getColorAttachmentTexture(texTarget);
}

void Rings::update(float seed)
{
	if(textured)
	{
		return;
	}

	GLHandler::ShaderProgram s = GLHandler::newShader("planet/gentex/ringtex");
	GLHandler::Mesh tmpMesh    = GLHandler::newMesh();
	GLHandler::setVertices(tmpMesh,
	                       {-1.f, -1.f, 1.f, -1.f, -1.f, 1.f, 1.f, 1.f}, s,
	                       {{"position", 2}});

	GLHandler::setShaderParam(s, "color", QColor(210, 180, 140));
	GLHandler::setShaderParam(
	    s, "seed",
	    // NOLINTNEXTLINE(cert-msc30-c, cert-msc50-c, cert-msc50-cpp)
	    /*10000.f * static_cast<float>(rand()) / INT_MAX*/ seed);

	GLHandler::beginRendering(texTarget);
	GLHandler::useShader(s);
	GLHandler::render(tmpMesh, GLHandler::PrimitiveType::TRIANGLE_STRIP);

	GLHandler::deleteMesh(tmpMesh);
	GLHandler::deleteShader(s);
}

void Rings::render(QMatrix4x4 const& model, QVector3D const& lightpos,
                   float lightradius, QColor const& lightcolor,
                   std::array<QVector4D, 5> const& neighborsPosRadius,
                   std::array<QVector3D, 5> const& neighborsOblateness,
                   QMatrix4x4 const& properRotation)
{
	GLHandler::setShaderParam(shader, "lightpos", lightpos);
	GLHandler::setShaderParam(shader, "lightradius", lightradius);
	GLHandler::setShaderParam(shader, "lightcolor", lightcolor);
	GLHandler::setShaderParam(shader, "neighborsPosRadius", 5,
	                          &(neighborsPosRadius[0]));
	GLHandler::setShaderParam(shader, "neighborsOblateness", 5,
	                          &(neighborsOblateness[0]));
	GLHandler::setShaderParam(shader, "properRotation", properRotation);
	GLHandler::beginTransparent();
	GLHandler::setUpRender(shader, model);
	GLHandler::useTextures({getTexture()});
	GLHandler::setBackfaceCulling(false);
	GLHandler::render(mesh);
	GLHandler::endTransparent();
	GLHandler::setBackfaceCulling(true);
}

Rings::~Rings()
{
	GLHandler::deleteMesh(mesh);
	GLHandler::deleteShader(shader);
	if(!textured)
	{
		GLHandler::deleteRenderTarget(texTarget);
	}
	else
	{
		GLHandler::deleteTexture(tex);
	}
}
