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

#include "VolumetricModel.hpp"

VolumetricModel::VolumetricModel(QString const& datFile)
    : shader("volume")
{
	std::ifstream file(datFile.toStdString(), std::ios::binary);
	int64_t Nx, Ny, Nz;
	double minx, maxx, miny, maxy, minz, maxz;

	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
	file.read(reinterpret_cast<char*>(&Nx), sizeof(int64_t));
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
	file.read(reinterpret_cast<char*>(&minx), sizeof(double));
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
	file.read(reinterpret_cast<char*>(&maxx), sizeof(double));
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
	file.read(reinterpret_cast<char*>(&Ny), sizeof(int64_t));
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
	file.read(reinterpret_cast<char*>(&miny), sizeof(double));
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
	file.read(reinterpret_cast<char*>(&maxy), sizeof(double));
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
	file.read(reinterpret_cast<char*>(&Nz), sizeof(int64_t));
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
	file.read(reinterpret_cast<char*>(&minz), sizeof(double));
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
	file.read(reinterpret_cast<char*>(&maxz), sizeof(double));

	minPos = Vector3(minx, miny, minz);
	maxPos = Vector3(maxx, maxy, maxz);

	std::cout << datFile.toStdString() << std::endl;
	std::cout << minPos << " " << maxPos << std::endl;

	posToTexCoord.scale(1.0 / (maxPos[0] - minPos[0]),
	                    1.0 / (maxPos[1] - minPos[1]),
	                    1.0 / (maxPos[2] - minPos[2]));
	posToTexCoord.translate(-1.0 * Utils::toQt(minPos));

	std::vector<float> data(Nx * Ny * Nz);
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
	file.read(reinterpret_cast<char*>(&data[0]), Nx * Ny * Nz * sizeof(float));

	float max(0.0);
	for(auto v : data)
	{
		if(v > max)
		{
			max = v;
		}
	}

	tex = new GLTexture(GLTexture::Tex3DProperties(Nz, Ny, Nx, GL_R32F),
	                    {GL_LINEAR, GL_CLAMP_TO_BORDER},
	                    {&data[0], GL_FLOAT, GL_RED});
	tex->generateMipmap();
}

void VolumetricModel::initMesh()
{
	renderable = true;

	Primitives::setAsUnitCube(mesh, shader);

	dataModel.translate(Utils::toQt(minPos));
	dataModel.scale(maxPos[0] - minPos[0], maxPos[1] - minPos[1],
	                maxPos[2] - minPos[2]);
	dataModel.translate(QVector3D(0.5f, 0.5f, 0.5f));
}

void VolumetricModel::initOcclusionModel(QString const& datFile)
{
	occlusionModel = new VolumetricModel(datFile);
}

void VolumetricModel::render(Camera const& camera, QMatrix4x4 const& model,
                             QVector3D const& campos)
{
	GLHandler::beginTransparent(GL_ONE, GL_ONE);
	GLHandler::setBackfaceCulling(true, GL_FRONT);
	shader.setUniform(
	    "color", QSettings().value("data/darkmattercolor").value<QColor>());
	shader.setUniform("campos", dataModel.inverted() * campos);

	std::vector<GLTexture const*> texs({tex});

	if(occlusionModel != nullptr)
	{
		shader.setUniform("useOccl", 1.f);
		shader.setUniform("occltransform",
		                  dataModel.inverted() * model.inverted()
		                      * occlusionModel->getPosToTexCoord());
		shader.setUniform("occltex", 1);
		texs.push_back(&occlusionModel->getTexture());
	}

	GLHandler::useTextures(texs);
	GLHandler::setUpRender(shader, model * dataModel);
	mesh.render(PrimitiveType::TRIANGLE_STRIP);
	GLHandler::setBackfaceCulling(true);
	GLHandler::endTransparent();
}

VolumetricModel::~VolumetricModel()
{
	delete tex;
	delete occlusionModel;
}
