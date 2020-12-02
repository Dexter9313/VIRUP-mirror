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

QList<QPair<QFuture<void>, std::vector<AssetLoader::MeshDescriptor>*>>&
    AsyncMesh::waitingForDeletion()
{
	static QList<
	    QPair<QFuture<void>, std::vector<AssetLoader::MeshDescriptor>*>>
	    waitingForDeletion = {};
	return waitingForDeletion;
}

AsyncMesh::AsyncMesh(QString const& path, GLMesh&& defaultMesh)
    : defaultMesh(std::move(defaultMesh))
{
	if(path.isEmpty())
	{
		emptyPath = true;
		return;
	}

	this->meshDescriptors    = new std::vector<AssetLoader::MeshDescriptor>;
	auto thisMeshDescriptors = this->meshDescriptors;
	future                   = QtConcurrent::run([path, thisMeshDescriptors]() {
        return AssetLoader::loadFile(path, *thisMeshDescriptors);
    });
}

void AsyncMesh::updateMesh(GLShaderProgram const& shader)
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

	std::vector<AssetLoader::TexturedMesh> meshes;
	AssetLoader::loadModel(*meshDescriptors, meshes, shader);

	if(meshes.size() == 1)
	{
		mesh = meshes[0].mesh;
		for(auto pair : meshes[0].textures)
		{
			delete pair.second;
		}
	}
	else
	{
		for(auto const& vMesh : meshes)
		{
			delete vMesh.mesh;
			for(auto pair : vMesh.textures)
			{
				delete pair.second;
			}
		}
	}

	delete meshDescriptors;
}

GLMesh const& AsyncMesh::getMesh()
{
	if(emptyPath)
	{
		return defaultMesh;
	}

	if(loaded)
	{
		return *mesh;
	}
	return defaultMesh;
}

AsyncMesh::~AsyncMesh()
{
	// future.waitForFinished();
	if(!emptyPath)
	{
		if(loaded)
		{
			delete mesh;
		}
		else
		{
			waitingForDeletion().push_back({future, meshDescriptors});
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
