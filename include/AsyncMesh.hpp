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
	AsyncMesh(QString const& path, GLMesh&& defaultMesh);
	bool isLoaded() const { return loaded; };
	float getBoundingSphereRadius() const { return boundingSphereRadius; };
	GLMesh const& getDefaultMesh() const { return defaultMesh; };
	// call regularly (especially at least before using getMesh)
	void updateMesh(GLShaderProgram const& shader);
	GLMesh const& getMesh();
	~AsyncMesh();

	static void garbageCollect(bool force = false);

  private:
	GLMesh defaultMesh;
	GLMesh* mesh = nullptr;

	QFuture<float> future;

	bool loaded                = false;
	bool emptyPath             = false;
	float boundingSphereRadius = 0.f;

	std::vector<AssetLoader::MeshDescriptor>* meshDescriptors;

	// never wait for futures to finish within destructor ! if you need to
	// release resources and the future didn't finish, push it here and other
	// AsyncTextures will take care of it later
	static QList<
	    QPair<QFuture<void>, std::vector<AssetLoader::MeshDescriptor>*>>&
	    waitingForDeletion();
};

#endif // ASYNCMESH_HPP
