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
	static void loadModel(QString modelName,
	                      std::vector<GLHandler::Mesh>& meshes,
	                      std::vector<GLHandler::Texture>& textures,
	                      GLHandler::ShaderProgram const& shader);
};

#endif // ASSETLOADER_H
