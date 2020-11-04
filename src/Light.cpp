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
    , shadowShader("shadow")
{
	unsigned int resolution(
	    1u << (9 + QSettings().value("graphics/shadowsquality").toUInt()));
	shadowMap = GLHandler::newDepthMap(resolution, resolution);
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

void Light::setUpShader(GLShaderProgram const& shader,
                        float boundingSphereRadius,
                        QMatrix4x4 const& model) const
{
	QVector3D relDir = QVector3D(model.inverted() * QVector4D(direction, 0.f));
	shader.setUniform("lightDirection", relDir.normalized());
	shader.setUniform("lightColor", color);
	shader.setUniform("lightAmbiantFactor", ambiantFactor);
	shader.setUniform("lightspace",
	                  getTransformation(boundingSphereRadius, model, true));
	shader.setUniform("boundingSphereRadius", boundingSphereRadius);
}

GLHandler::Texture Light::getShadowMap() const
{
	return GLHandler::getColorAttachmentTexture(shadowMap);
}

void Light::generateShadowMap(std::vector<GLMesh const*> const& meshes,
                              float boundingSphereRadius,
                              std::vector<QMatrix4x4> const& models,
                              QMatrix4x4 const& model)
{
	// see third comment :
	// https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
	GLHandler::setBackfaceCulling(false /*, GL_FRONT*/);
	GLHandler::beginRendering(GL_DEPTH_BUFFER_BIT, shadowMap);
	QMatrix4x4 lightSpace(getTransformation(boundingSphereRadius, model));
	for(unsigned int i(0); i < meshes.size(); ++i)
	{
		shadowShader.setUniform("camera", lightSpace * models[i]);
		meshes[i]->render();
	}
	GLHandler::setBackfaceCulling(true);
}

Light::~Light()
{
	GLHandler::deleteRenderTarget(shadowMap);
}
