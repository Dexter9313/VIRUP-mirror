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

#ifndef ASYNCMESH_HPP
#define ASYNCMESH_HPP

#include <QtConcurrent>

#include "AssetLoader.hpp"

class AsyncMesh
{
  public:
	// grabs ownage of defaultMesh
	AsyncMesh(QString const& path, GLHandler::Mesh const& defaultMesh,
	          GLHandler::ShaderProgram shader);
	bool isLoaded() const { return loaded; };
	float getBoundingSphereRadius() const { return boundingSphereRadius; };
	GLHandler::Mesh getDefaultMesh() const { return defaultMesh; };
	void updateMesh();
	GLHandler::Mesh getMesh();
	~AsyncMesh();

  private:
	GLHandler::Mesh defaultMesh = {};
	GLHandler::Mesh mesh        = {};

	QFuture<float> future;

	bool loaded                = false;
	bool emptyPath             = false;
	float boundingSphereRadius = 0.f;
	std::vector<std::vector<float>> loadedVertices;
	std::vector<std::vector<unsigned int>> loadedIndices;
	std::vector<std::string> loadedTexs;

	GLHandler::ShaderProgram shader;
};

#endif // ASYNCMESH_HPP
