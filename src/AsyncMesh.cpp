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

QList<QPair<QFuture<void>, AsyncMesh::Data*>>& AsyncMesh::waitingForDeletion()
{
	static QList<QPair<QFuture<void>, AsyncMesh::Data*>> waitingForDeletion
	    = {};
	return waitingForDeletion;
}

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

	data = new Data;

	Data* thisData = this->data;
	future         = QtConcurrent::run([path, thisData]() {
        return AssetLoader::loadFile(path, thisData->loadedVertices,
                                     thisData->loadedIndices,
                                     thisData->loadedTexs);
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
	AssetLoader::loadModel(data->loadedVertices, data->loadedIndices,
	                       data->loadedTexs, meshes, textures, shader);

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

	delete data;
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
	// future.waitForFinished();
	GLHandler::deleteMesh(defaultMesh);
	if(!emptyPath)
	{
		if(loaded)
		{
			GLHandler::deleteMesh(mesh);
		}
		else if(future.isFinished())
		{
			delete data;
		}
		else
		{
			waitingForDeletion().push_back({future, data});
		}
	}
}

void AsyncMesh::garbageCollect(bool force)
{
	// go in reverse because of possible deletions
	for(int i(waitingForDeletion().size() - 1); i >= 0; --i)
	{
		if(force)
		{
			waitingForDeletion()[i].first.waitForFinished();
		}
		if(waitingForDeletion()[i].first.isFinished())
		{
			delete waitingForDeletion()[i].second;
			waitingForDeletion().removeAt(i);
		}
	}
}
