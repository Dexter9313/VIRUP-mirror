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

#include "AsyncMesh.hpp"

AsyncMesh::AsyncMesh(QString const& path, GLHandler::Mesh const& defaultMesh,
                     GLHandler::ShaderProgram shader)
    : defaultMesh(defaultMesh)
    , shader(shader)
{
	if(path.isEmpty())
	{
		emptyPath = true;
		return;
	}
	future = QtConcurrent::run([path, this]() {
		return AssetLoader::loadFile(path, this->loadedVertices,
		                             this->loadedIndices, this->loadedTexs);
	});
}

void AsyncMesh::updateMesh()
{
	if(emptyPath)
	{
		return;
	}

	if(loaded || !future.isFinished())
	{
		return;
	}

	loaded               = true;
	boundingSphereRadius = future.result();

	std::vector<GLHandler::Mesh> meshes;
	std::vector<GLHandler::Texture> textures;
	AssetLoader::loadModel(loadedVertices, loadedIndices, loadedTexs, meshes,
	                       textures, shader);

	if(meshes.size() == 1)
	{
		GLHandler::deleteMesh(mesh);
		mesh = meshes[0];
	}
	else
	{
		for(auto vMesh : meshes)
		{
			GLHandler::deleteMesh(vMesh);
		}
	}

	for(auto vTex : textures)
	{
		GLHandler::deleteTexture(vTex);
	}

	loadedVertices.resize(0);
	loadedVertices.shrink_to_fit();
	loadedIndices.resize(0);
	loadedIndices.shrink_to_fit();
	loadedTexs.resize(0);
	loadedTexs.shrink_to_fit();
}

GLHandler::Mesh AsyncMesh::getMesh()
{
	if(emptyPath)
	{
		return defaultMesh;
	}

	updateMesh();
	if(loaded)
	{
		return mesh;
	}
	return defaultMesh;
}

AsyncMesh::~AsyncMesh()
{
	GLHandler::deleteMesh(defaultMesh);
	if(!emptyPath && loaded)
	{
		GLHandler::deleteMesh(mesh);
	}
}
