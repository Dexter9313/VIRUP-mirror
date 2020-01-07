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

#include "Model.hpp"

Model::Model(QString const& modelName, QColor const& defaultDiffuseColor)
{
	QMap<QString, QString> defines;
	unsigned int smoothshadows(
	    QSettings().value("graphics/smoothshadows").toUInt());
	if(smoothshadows > 0)
	{
		defines["SMOOTHSHADOWS"] = QString::number(smoothshadows * 2 + 1);
	}
	shader = GLHandler::newShader("model", defines);

	boundingSphereRadius = AssetLoader::loadModel(modelName, meshes, shader,
	                                              defaultDiffuseColor);

	GLHandler::setShaderParam(shader, "diffuse", 0);
	GLHandler::setShaderParam(shader, "specular", 1);
	GLHandler::setShaderParam(shader, "ambient", 2);
	GLHandler::setShaderParam(shader, "emissive", 3);
	GLHandler::setShaderParam(shader, "normals", 4);
	GLHandler::setShaderParam(shader, "shininess", 5);
	GLHandler::setShaderParam(shader, "opacity", 6);
	GLHandler::setShaderParam(shader, "lightmap", 7);
	GLHandler::setShaderParam(shader, "shadowmap", 8);
}

Model::Model(QString const& modelName, GLHandler::ShaderProgram shader,
             QColor const& defaultDiffuseColor)
    : shader(shader)
{
	boundingSphereRadius = AssetLoader::loadModel(modelName, meshes, shader,
	                                              defaultDiffuseColor);

	GLHandler::setShaderParam(shader, "diffuse", 0);
	GLHandler::setShaderParam(shader, "specular", 1);
	GLHandler::setShaderParam(shader, "ambient", 2);
	GLHandler::setShaderParam(shader, "emissive", 3);
	GLHandler::setShaderParam(shader, "normals", 4);
	GLHandler::setShaderParam(shader, "shininess", 5);
	GLHandler::setShaderParam(shader, "opacity", 6);
	GLHandler::setShaderParam(shader, "lightmap", 7);
	GLHandler::setShaderParam(shader, "shadowmap", 8);
}

void Model::generateShadowMap(QMatrix4x4 const& model, Light& light)
{
	std::vector<GLHandler::Mesh> glMeshes;
	std::vector<QMatrix4x4> models;
	for(auto const& mesh : meshes)
	{
		glMeshes.push_back(mesh.mesh);
		models.push_back(mesh.transform);
	}
	light.generateShadowMap(glMeshes, boundingSphereRadius, models, model);
}

void Model::render(QVector3D const& cameraPosition, QMatrix4x4 const& model,
                   Light const& light, GLHandler::GeometricSpace geometricSpace)
{
	light.setUpShader(shader, boundingSphereRadius, model);
	GLHandler::setShaderParam(shader, "cameraPosition",
	                          model.inverted() * cameraPosition);

	for(auto& mesh : meshes)
	{
		GLHandler::useTextures(
		    {mesh.textures[AssetLoader::TextureType::DIFFUSE],
		     mesh.textures[AssetLoader::TextureType::SPECULAR],
		     mesh.textures[AssetLoader::TextureType::AMBIENT],
		     mesh.textures[AssetLoader::TextureType::EMISSIVE],
		     mesh.textures[AssetLoader::TextureType::NORMALS],
		     mesh.textures[AssetLoader::TextureType::SHININESS],
		     mesh.textures[AssetLoader::TextureType::OPACITY],
		     mesh.textures[AssetLoader::TextureType::LIGHTMAP],
		     light.getShadowMap()});
		GLHandler::setShaderParam(shader, "localTransform", mesh.transform);
		GLHandler::setUpRender(shader, model * mesh.transform, geometricSpace);
		GLHandler::render(mesh.mesh);
	}
}

Model::~Model()
{
	for(auto const& mesh : meshes)
	{
		GLHandler::deleteMesh(mesh.mesh);
		for(auto pair : mesh.textures)
		{
			GLHandler::deleteTexture(pair.second);
		}
	}
	GLHandler::deleteShader(shader);
}
