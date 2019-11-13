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

#include "Light.hpp"

Light::Light()
    : direction(-1.f, 0.f, 0.f)
    , color(255, 255, 255)
    , ambiantFactor(0.05f)
{
	unsigned int resolution(
	    1u << (9 + QSettings().value("graphics/shadowsquality").toUInt()));
	shadowMap    = GLHandler::newDepthMap(resolution, resolution);
	shadowShader = GLHandler::newShader("shadow");
}

QMatrix4x4 Light::getTransformation(float boundingSphereRadius,
                                    QMatrix4x4 const& model, bool biased) const
{
	QMatrix4x4 lightSpace;
	if(biased)
	{
		lightSpace.translate(0.5f, 0.5f, 0.5f);
		lightSpace.scale(0.5);
	}
	lightSpace.ortho(-1.f * boundingSphereRadius, boundingSphereRadius,
	                 -1.f * boundingSphereRadius, boundingSphereRadius,
	                 -1.f * boundingSphereRadius, boundingSphereRadius);
	lightSpace.lookAt(
	    QVector3D(0.f, 0.f, 0.f),
	    QVector3D(model.inverted() * QVector4D(direction, 0.f)).normalized(),
	    QVector3D(0.f, 0.f, 1.f));

	return lightSpace;
}

void Light::setUpShader(GLHandler::ShaderProgram const& shader,
                        float boundingSphereRadius,
                        QMatrix4x4 const& model) const
{
	QVector3D relDir = QVector3D(model.inverted() * QVector4D(direction, 0.f));
	GLHandler::setShaderParam(shader, "lightDirection", relDir.normalized());
	GLHandler::setShaderParam(shader, "lightColor", color);
	GLHandler::setShaderParam(shader, "lightAmbiantFactor", ambiantFactor);
	GLHandler::setShaderParam(
	    shader, "lightspace",
	    getTransformation(boundingSphereRadius, model, true));
	GLHandler::setShaderParam(shader, "boundingSphereRadius",
	                          boundingSphereRadius);
}

GLHandler::Texture Light::getShadowMap() const
{
	return GLHandler::getColorAttachmentTexture(shadowMap);
}

void Light::generateShadowMap(std::vector<GLHandler::Mesh> meshes,
                              float boundingSphereRadius,
                              QMatrix4x4 const& model)
{
	QMatrix4x4 lightSpace(getTransformation(boundingSphereRadius, model));

	GLHandler::beginRendering(GL_DEPTH_BUFFER_BIT, shadowMap);
	GLHandler::setShaderParam(shadowShader, "camera", lightSpace);

	// see third comment :
	// https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
	GLHandler::setBackfaceCulling(false /*, GL_FRONT*/);
	for(auto& mesh : meshes)
	{
		GLHandler::render(mesh);
	}
	GLHandler::setBackfaceCulling(true);
}

Light::~Light()
{
	GLHandler::deleteShader(shadowShader);
	GLHandler::deleteRenderTarget(shadowMap);
}
