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

#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>

#include "GLHandler.hpp"
#include "utils.hpp"

class AssetLoader
{
  public:
	enum class TextureType
	{
		NONE      = 0,
		DIFFUSE   = 1,
		SPECULAR  = 2,
		AMBIENT   = 3,
		EMISSIVE  = 4,
		NORMALS   = 6,
		SHININESS = 7,
		OPACITY   = 9,
		LIGHTMAP  = 10
	};

	// for iteration on types
	static std::vector<TextureType> const& textureTypes();

	struct MeshDescriptor
	{
		std::vector<float> vertices;
		std::vector<unsigned int> indices;
		std::vector<std::pair<TextureType, std::string>> texturesPathsTypes;
	};

	struct TexturedMesh
	{
		GLHandler::Mesh mesh;
		std::map<TextureType, GLHandler::Texture> textures; // a.k.a. material
	};

	// returns model bounding sphere radius
	static float loadFile(QString modelName,
	                      std::vector<MeshDescriptor>& meshDescriptors);

	// GLHandler resources in textured meshes are yours to free !
	static void loadModel(std::vector<MeshDescriptor> const& meshDescriptors,
	                      std::vector<TexturedMesh>& meshes,
	                      GLHandler::ShaderProgram const& shader,
	                      QColor const& defaultDiffuseColor
	                      = {0xff, 0x09, 0xf7});

	// GLHandler resources in textured meshes are yours to free !
	static float
	    loadModel(QString const& modelName, std::vector<TexturedMesh>& meshes,
	              GLHandler::ShaderProgram const& shader,
	              QColor const& defaultDiffuseColor = {0xff, 0x09, 0xf7});

  private:
	static std::string findFilePath(std::string const& directory,
	                                std::string const& fileName);

	static QColor getDefaultColor(TextureType ttype, QColor diffuseColor);

	static std::vector<aiTextureType> const& assimpTextureTypes();
};

#endif // ASSETLOADER_H
