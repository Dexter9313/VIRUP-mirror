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

#include "AssetLoader.hpp"

float AssetLoader::loadFile(QString modelName,
                            std::vector<std::vector<float>>& vertices,
                            std::vector<std::vector<unsigned int>>& indices,
                            std::vector<std::string>& texturesPaths)
{
	vertices.resize(0);
	indices.resize(0);
	texturesPaths.resize(0);
	if(!modelName.contains('/'))
	{
		modelName = "models/" + modelName;
	}

	modelName = getAbsoluteDataPath(modelName);

	std::string path(modelName.toStdString());
	std::string directory = path.substr(0, path.find_last_of('/'));

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
	    path, static_cast<unsigned int>(aiProcess_Triangulate)
	              | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices
	              | aiProcess_OptimizeMeshes | aiProcess_GenSmoothNormals);

	if(scene == nullptr
	   // NOLINTNEXTLINE(readability-implicit-bool-conversion)
	   || static_cast<unsigned int>(scene->mFlags)
	          // NOLINTNEXTLINE(readability-implicit-bool-conversion)
	          & static_cast<unsigned int>(AI_SCENE_FLAGS_INCOMPLETE)
	   || scene->mRootNode == nullptr || scene->mNumMeshes <= 0)
	{
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString()
		          << std::endl;
		return 0.f;
	}

	float boundingSphereRadius = 0.f;

	for(unsigned int i(0); i < scene->mNumMeshes; ++i)
	{
		std::vector<float> v;
		std::vector<unsigned int> ind;
		aiMesh* mesh = scene->mMeshes[i];
		for(unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			QVector3D vertice(mesh->mVertices[i].x, mesh->mVertices[i].y,
			                  mesh->mVertices[i].z);
			if(boundingSphereRadius < vertice.length())
			{
				boundingSphereRadius = vertice.length();
			}
			v.push_back(vertice.x());
			v.push_back(vertice.y());
			v.push_back(vertice.z());
			v.push_back(mesh->mNormals[i].x);
			v.push_back(mesh->mNormals[i].y);
			v.push_back(mesh->mNormals[i].z);
			if(mesh->mTextureCoords[0] != nullptr)
			{
				v.push_back(mesh->mTextureCoords[0][i].x);
				v.push_back(mesh->mTextureCoords[0][i].y);
			}
			else
			{
				v.push_back(0.f);
				v.push_back(0.f);
			}
		}
		for(unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face;
			face.mNumIndices = 0;
			face             = mesh->mFaces[i];
			for(unsigned int j = 0; j < face.mNumIndices; j++)
			{
				ind.push_back(face.mIndices[j]);
			}
		}
		vertices.push_back(v);
		indices.push_back(ind);

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiString str;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
		std::string texpath(str.C_Str());
		int pos(texpath.size() - 1);
		while(pos > 0 && texpath[pos] != '\\' && texpath[pos] != '/')
		{
			pos--;
		}
		pos++;
		if(!texpath.empty())
		{
			texpath.append(directory).append("/").append(
			    texpath.substr(pos, texpath.size() - 1));
			texturesPaths.push_back(texpath);
		}
	}

	return boundingSphereRadius;
}

void AssetLoader::loadModel(
    std::vector<std::vector<float>> const& vertices,
    std::vector<std::vector<unsigned int>> const& indices,
    std::vector<std::string> const& texturesPaths,
    std::vector<GLHandler::Mesh>& meshes,
    std::vector<GLHandler::Texture>& textures,
    GLHandler::ShaderProgram const& shader)
{
	for(unsigned int i(0); i < vertices.size(); ++i)
	{
		GLHandler::Mesh result(GLHandler::newMesh());
		GLHandler::setVertices(
		    result, vertices[i], shader,
		    {{"position", 3}, {"normal", 3}, {"texcoord", 2}}, indices[i]);
		meshes.push_back(result);
	}
	for(auto const& texPath : texturesPaths)
	{
		textures.push_back(GLHandler::newTexture(texPath.c_str()));
	}
}

float AssetLoader::loadModel(QString const& modelName,
                             std::vector<GLHandler::Mesh>& meshes,
                             std::vector<GLHandler::Texture>& textures,
                             GLHandler::ShaderProgram const& shader)
{
	std::vector<std::vector<float>> v;
	std::vector<std::vector<unsigned int>> ind;
	std::vector<std::string> tPaths;

	float bsRad(loadFile(modelName, v, ind, tPaths));
	if(bsRad == 0.f)
	{
		return 0.f;
	}

	loadModel(v, ind, tPaths, meshes, textures, shader);
	return bsRad;
}
