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
    : shader("model", setUpShaderDefines())
{
	boundingSphereRadius = AssetLoader::loadModel(modelName, meshes, shader,
	                                              defaultDiffuseColor);

	shader.setUniform("diffuse", 0);
	shader.setUniform("specular", 1);
	shader.setUniform("ambient", 2);
	shader.setUniform("emissive", 3);
	shader.setUniform("normals", 4);
	shader.setUniform("shininess", 5);
	shader.setUniform("opacity", 6);
	shader.setUniform("lightmap", 7);
	shader.setUniform("shadowmap", 8);
}

Model::Model(QString const& modelName, GLShaderProgram&& shader,
             QColor const& defaultDiffuseColor)
    : shader(std::move(shader))
{
	boundingSphereRadius = AssetLoader::loadModel(
	    modelName, meshes, this->shader, defaultDiffuseColor);

	this->shader.setUniform("diffuse", 0);
	this->shader.setUniform("specular", 1);
	this->shader.setUniform("ambient", 2);
	this->shader.setUniform("emissive", 3);
	this->shader.setUniform("normals", 4);
	this->shader.setUniform("shininess", 5);
	this->shader.setUniform("opacity", 6);
	this->shader.setUniform("lightmap", 7);
	this->shader.setUniform("shadowmap", 8);
}

void Model::generateShadowMap(QMatrix4x4 const& model, Light& light)
{
	std::vector<GLMesh const*> glMeshes;
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
	shader.setUniform("cameraPosition", model.inverted() * cameraPosition);

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
		shader.setUniform("localTransform", mesh.transform);
		GLHandler::setUpRender(shader, model * mesh.transform, geometricSpace);
		mesh.mesh->render();
	}
}

Model::~Model()
{
	for(auto const& mesh : meshes)
	{
		delete mesh.mesh;
		for(auto pair : mesh.textures)
		{
			GLHandler::deleteTexture(pair.second);
		}
	}
}

QMap<QString, QString> Model::setUpShaderDefines()
{
	QMap<QString, QString> defines;
	unsigned int smoothshadows(
	    QSettings().value("graphics/smoothshadows").toUInt());
	if(smoothshadows > 0)
	{
		defines["SMOOTHSHADOWS"] = QString::number(smoothshadows * 2 + 1);
	}
	return defines;
}
