/*
    Copyright (C) 2020 Florian Cabot <florian.cabot@hotmail.fr>

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

#ifndef VOLUMETRICMODEL_HPP
#define VOLUMETRICMODEL_HPP

#include <fstream>

#include "Camera.hpp"
#include "Primitives.hpp"
#include "gl/GLHandler.hpp"
#include "graphics/Utils.hpp"
#include "math/Vector3.hpp"

class VolumetricModel
{
  public:
	VolumetricModel(QString const& datFile);
	void initMesh();
	void initOcclusionModel(QString const& datFile);
	Vector3 getMinPos() const { return minPos; };
	Vector3 getMaxPos() const { return maxPos; };
	QMatrix4x4 getPosToTexCoord() const { return posToTexCoord; };
	GLTexture const& getTexture() const { return *tex; };
	void render(Camera const& camera, QMatrix4x4 const& model, QVector3D const& campos);
	~VolumetricModel();

	float brightnessMultiplier = 1.f;

  private:
	Vector3 minPos;
	Vector3 maxPos;
	QMatrix4x4 posToTexCoord;
	GLTexture* tex = nullptr;

	bool renderable = false;

	QMatrix4x4 dataModel;
	GLShaderProgram shader;
	GLMesh mesh;

	VolumetricModel* occlusionModel = nullptr;
};

#endif // VOLUMETRICMODEL_HPP
