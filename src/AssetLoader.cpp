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

std::vector<AssetLoader::TextureType> const& AssetLoader::textureTypes()
{
	static std::vector<TextureType> textureTypes
	    = {TextureType::DIFFUSE,  TextureType::SPECULAR, TextureType::AMBIENT,
	       TextureType::EMISSIVE, TextureType::NORMALS,  TextureType::SHININESS,
	       TextureType::OPACITY,  TextureType::LIGHTMAP};
	return textureTypes;
}

std::vector<aiTextureType> const& AssetLoader::assimpTextureTypes()
{
	static std::vector<aiTextureType> assimpTextureTypes = {
	    aiTextureType_DIFFUSE,  aiTextureType_SPECULAR, aiTextureType_AMBIENT,
	    aiTextureType_EMISSIVE, aiTextureType_NORMALS,  aiTextureType_SHININESS,
	    aiTextureType_OPACITY,  aiTextureType_LIGHTMAP};
	return assimpTextureTypes;
}

float AssetLoader::loadFile(QString modelName,
                            std::vector<MeshDescriptor>& meshDescriptors)
{
	meshDescriptors.resize(0);
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
	              | aiProcess_OptimizeMeshes | aiProcess_GenSmoothNormals
	              | aiProcess_CalcTangentSpace);

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

	return parseNode(scene->mRootNode, scene, directory, QMatrix4x4(),
	                 meshDescriptors);
}

void AssetLoader::loadModel(std::vector<MeshDescriptor> const& meshDescriptors,
                            std::vector<TexturedMesh>& meshes,
                            GLShaderProgram const& shader,
                            QColor const& defaultDiffuseColor)
{
	for(auto const& descriptor : meshDescriptors)
	{
		TexturedMesh tMesh;

		tMesh.mesh = new GLMesh;
		tMesh.mesh->setVertices(
		    descriptor.vertices, shader,
		    {{"position", 3}, {"tangent", 3}, {"normal", 3}, {"texcoord", 2}},
		    descriptor.indices);

		for(auto const& tex : descriptor.texturesPathsTypes)
		{
			// discard additional textures, keep only one per type
			if(tMesh.textures.count(tex.first) == 0 && !tex.second.empty())
			{
				tMesh.textures[tex.first] = new GLTexture(
				    tex.second.c_str(), tex.first == TextureType::DIFFUSE);
			}
		}
		// complete with default textures
		for(auto const& ttype : textureTypes())
		{
			if(tMesh.textures.count(ttype) == 0)
			{
				QColor color(getDefaultColor(ttype, defaultDiffuseColor));
				char data[4];
				data[0] = color.red();
				data[1] = color.green();
				data[2] = color.blue();
				data[3] = color.alpha();
				tMesh.textures[ttype]
				    = new GLTexture(GLTexture::Tex2DProperties(
				                        1, 1, ttype == TextureType::DIFFUSE),
				                    {}, {&data[0]});
			}
		}
		tMesh.transform = descriptor.transform;
		meshes.push_back(tMesh);
	}
}

float AssetLoader::loadModel(QString const& modelName,
                             std::vector<TexturedMesh>& meshes,
                             GLShaderProgram const& shader,
                             QColor const& defaultDiffuseColor)
{
	std::vector<MeshDescriptor> descriptors;

	float bsRad(loadFile(modelName, descriptors));
	if(bsRad == 0.f)
	{
		return 0.f;
	}

	loadModel(descriptors, meshes, shader, defaultDiffuseColor);
	return bsRad;
}

std::string AssetLoader::findFilePath(std::string const& directory,
                                      std::string const& fileName)
{
	QDir dir(directory.c_str());
	QFileInfoList results = dir.entryInfoList();
	for(auto const& entry : results)
	{
		if(entry.fileName() == "." || entry.fileName() == "..")
		{
			continue;
		}
		if(!entry.isDir())
		{
			if(entry.fileName() == fileName.c_str())
			{
				return entry.filePath().toLatin1().data();
			}
			continue;
		}
		std::string retrieved(
		    findFilePath(entry.filePath().toLatin1().data(), fileName));
		if(!retrieved.empty())
		{
			return retrieved;
		}
	}
	return "";
}

QColor AssetLoader::getDefaultColor(TextureType ttype, QColor diffuseColor)
{
	switch(ttype)
	{
		case TextureType::DIFFUSE:
			return diffuseColor;
		case TextureType::SPECULAR:
			return {0, 0, 0};
		case TextureType::AMBIENT:
			return {255, 255, 255};
		case TextureType::EMISSIVE:
			return {0, 0, 0};
		case TextureType::NORMALS:
			return {128, 128, 255};
		case TextureType::SHININESS:
			return {0, 0, 0};
		case TextureType::OPACITY:
			return {255, 255, 255};
		case TextureType::LIGHTMAP:
			return {255, 255, 255};
		default:
			return {};
	}
	return {};
}

QMatrix4x4 AssetLoader::assimpToQt(aiMatrix4x4 m)
{
	return {m.a1, m.a2, m.a3, m.a4, m.b1, m.b2, m.b3, m.b4,
	        m.c1, m.c2, m.c3, m.c4, m.d1, m.d2, m.d3, m.d4};
}

float AssetLoader::parseNode(aiNode const* node, aiScene const* scene,
                             std::string const& directory,
                             QMatrix4x4 const& transform,
                             std::vector<MeshDescriptor>& meshDescriptors)
{
	float boundingSphereRadius(0.f);
	QMatrix4x4 nodeTransform(transform * assimpToQt(node->mTransformation));
	for(unsigned int i(0); i < node->mNumMeshes; ++i)
	{
		meshDescriptors.emplace_back(MeshDescriptor());
		MeshDescriptor& descriptor = meshDescriptors.back();
		aiMesh const* mesh         = scene->mMeshes[node->mMeshes[i]];
		float bsr(parseMesh(mesh, scene, directory, nodeTransform, descriptor));
		if(bsr > boundingSphereRadius)
		{
			boundingSphereRadius = bsr;
		}
	}
	// recurse
	for(unsigned int i(0); i < node->mNumChildren; ++i)
	{
		float bsr(parseNode(node->mChildren[i], scene, directory, nodeTransform,
		                    meshDescriptors));
		if(bsr > boundingSphereRadius)
		{
			boundingSphereRadius = bsr;
		}
	}
	return boundingSphereRadius;
}

float AssetLoader::parseMesh(aiMesh const* mesh, aiScene const* scene,
                             std::string const& directory,
                             QMatrix4x4 const& transform,
                             MeshDescriptor& result)
{
	float boundingSphereRadius(0.f);
	std::vector<float>& v          = result.vertices;
	std::vector<unsigned int>& ind = result.indices;
	for(unsigned int j(0); j < mesh->mNumVertices; j++)
	{
		QVector3D vertice(mesh->mVertices[j].x, mesh->mVertices[j].y,
		                  mesh->mVertices[j].z);
		if(boundingSphereRadius
		   < (transform * QVector4D(vertice, 1.f)).length())
		{
			boundingSphereRadius
			    = (transform * QVector4D(vertice, 1.f)).length();
		}
		v.push_back(vertice.x());
		v.push_back(vertice.y());
		v.push_back(vertice.z());
		if(mesh->HasTangentsAndBitangents())
		{
			v.push_back(mesh->mTangents[j].x);
			v.push_back(mesh->mTangents[j].y);
			v.push_back(mesh->mTangents[j].z);
		}
		else
		{
			v.push_back(0.f);
			v.push_back(0.f);
			v.push_back(0.f);
		}
		v.push_back(mesh->mNormals[j].x);
		v.push_back(mesh->mNormals[j].y);
		v.push_back(mesh->mNormals[j].z);
		if(mesh->mTextureCoords[0] != nullptr)
		{
			v.push_back(mesh->mTextureCoords[0][j].x);
			v.push_back(mesh->mTextureCoords[0][j].y);
		}
		else
		{
			v.push_back(0.f);
			v.push_back(0.f);
		}
	}
	for(unsigned int j(0); j < mesh->mNumFaces; j++)
	{
		aiFace face;
		face.mNumIndices = 0;
		face             = mesh->mFaces[j];
		for(unsigned int k = 0; k < face.mNumIndices; k++)
		{
			ind.push_back(face.mIndices[k]);
		}
	}

	std::vector<std::pair<TextureType, std::string>>& texturesPathsTypes
	    = result.texturesPathsTypes;
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	aiString str;
	for(unsigned int j(0); j < textureTypes().size(); ++j)
	{
		for(unsigned int k(0);
		    k < material->GetTextureCount(assimpTextureTypes()[j]); ++k)
		{
			material->GetTexture(assimpTextureTypes()[j], k, &str);
			std::string texpath(str.C_Str());
			int pos(texpath.size() - 1);
			while(pos > 0 && texpath[pos] != '\\' && texpath[pos] != '/')
			{
				pos--;
			}
			if(texpath[pos] == '\\' || texpath[pos] == '/')
			{
				pos++;
			}
			if(!texpath.empty())
			{
				texpath = findFilePath(directory,
				                       texpath.substr(pos, texpath.size()));
				texturesPathsTypes.emplace_back(
				    std::pair<TextureType, std::string>{textureTypes()[j],
				                                        texpath});
			}
		}
	}
	result.transform = transform;
	return boundingSphereRadius;
}
