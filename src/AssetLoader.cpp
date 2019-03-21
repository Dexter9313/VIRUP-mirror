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

void AssetLoader::loadModel(QString modelName,
                            std::vector<GLHandler::Mesh>& meshes,
                            std::vector<GLHandler::Texture>& textures,
                            GLHandler::ShaderProgram const& shader)
{
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
	              | aiProcess_OptimizeMeshes);

	if(scene == nullptr
	   // NOLINTNEXTLINE(readability-implicit-bool-conversion)
	   || static_cast<unsigned int>(scene->mFlags)
	          // NOLINTNEXTLINE(readability-implicit-bool-conversion)
	          & static_cast<unsigned int>(AI_SCENE_FLAGS_INCOMPLETE)
	   || scene->mRootNode == nullptr || scene->mNumMeshes <= 0)
	{
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString()
		          << std::endl;
		return;
	}
	for(unsigned int i(0); i < scene->mNumMeshes; ++i)
	{
		std::vector<float> vertices;
		std::vector<unsigned int> indices;
		aiMesh* mesh = scene->mMeshes[i];
		for(unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			vertices.push_back(mesh->mVertices[i].x);
			vertices.push_back(mesh->mVertices[i].y);
			vertices.push_back(mesh->mVertices[i].z);
			vertices.push_back(mesh->mNormals[i].x);
			vertices.push_back(mesh->mNormals[i].y);
			vertices.push_back(mesh->mNormals[i].z);
			if(mesh->mTextureCoords[0] != nullptr)
			{
				vertices.push_back(mesh->mTextureCoords[0][i].x);
				vertices.push_back(mesh->mTextureCoords[0][i].y);
			}
			else
			{
				vertices.push_back(0.f);
				vertices.push_back(0.f);
			}
		}
		for(unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for(unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}
		GLHandler::Mesh result(GLHandler::newMesh());
		GLHandler::setVertices(
		    result, vertices, shader,
		    {{"position", 3}, {"normal", 3}, {"texcoord", 2}}, indices);
		meshes.push_back(result);

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
		texpath.append(directory).append("/").append(
		    texpath.substr(pos, texpath.size() - 1));
		textures.push_back(GLHandler::newTexture(texpath.c_str()));
	}
}
