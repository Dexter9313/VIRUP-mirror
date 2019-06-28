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

#ifndef DETAILEDPLANETRENDERER_H
#define DETAILEDPLANETRENDERER_H

#include <QImageReader>
#include <QtConcurrent>

#include "AssetLoader.hpp"
#include "BasicCamera.hpp"
#include "Primitives.hpp"

#include "math/constants.hpp"

#include "graphics/renderers/Rings.hpp"

class DetailedPlanetRenderer
{
  public:
	DetailedPlanetRenderer(float radius = 1.f, QVector3D const& oblateness
	                                           = QVector3D(1.0, 1.0, 1.0));

	void initTerrestrial(float seed,
	                     QColor const& color = QColor(255, 255, 255),
	                     float polarLatitude = 90.f, float atmosphere = 0.f);
	void initGazGiant(float seed, QColor const& color = QColor(255, 255, 255),
	                  float bandsIntensity = 1.f, float stormsIntensity = 1.f);
	void initFromTex(QString const& diffusePath, float atmosphere);
	void initFromTex(QString const& diffusePath, QString const& normalPath,
	                 float atmosphere = 0.f);
	void updateModel(QString const& modelName);
	void updateTextureLoading(bool cancelLoading = false);
	float updateModelLoading();

	void initRings(float seed, float innerRing, float outerRing,
	               QString const& texturePath = "");
	void render(QVector3D const& pos, QVector3D const& lightpos,
	            float lightradius, QColor const& lightcolor,
	            std::array<QVector4D, 5> const& neighborsPosRadius,
	            std::array<QVector3D, 5> const& neighborsOblateness,
	            QMatrix4x4 const& properRotation, bool flipCoords = false);
	void render(QMatrix4x4 const& model, QVector3D const& lightpos,
	            float lightradius, QColor const& lightcolor,
	            std::array<QVector4D, 5> const& neighborsPosRadius,
	            std::array<QVector3D, 5> const& neighborsOblateness,
	            QMatrix4x4 const& properRotation, bool flipCoords = false);
	bool isValid() const { return valid && !modelIsLoading; };
	bool isLoading() const { return modelIsLoading || !futures.empty(); };
	~DetailedPlanetRenderer();

  private:
	void loadParallel(QString const& path, unsigned int index);
	void loadModelParallel(QString const& path);
	static void envMap(GLHandler::ShaderProgram& shader, GLHandler::Mesh& mesh,
	                   GLHandler::RenderTarget& renderTarget);

	bool valid = false;
	float radius;
	float atmosphere = 0.f;
	QVector3D oblateness;

	bool normal = false;

	GLHandler::ShaderProgram shader = {};
	GLHandler::Mesh mesh            = {};
	GLHandler::RenderTarget cubemapDiffuse;
	GLHandler::RenderTarget cubemapNormal;

	Rings* rings = nullptr;

	static unsigned int& cubemapsSize();

	// parallel QImage loading
	std::vector<QFuture<void>> futures;

	// parallel Model loading
	bool modelIsLoading = false;
	float lastSphereVal = 0.f;
	QFuture<float> modelFuture;
	std::vector<std::vector<float>> loadedVertices;
	std::vector<std::vector<unsigned int>> loadedIndices;
	std::vector<std::string> loadedTexs;

	GLHandler::PixelBufferObject pbos[2];
};

#endif // DETAILEDPLANETRENDERER_H
