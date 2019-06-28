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

#include "graphics/renderers/DetailedPlanetRenderer.hpp"

unsigned int& DetailedPlanetRenderer::cubemapsSize()
{
	static unsigned int cubemapsSize = 2048;
	return cubemapsSize;
}

DetailedPlanetRenderer::DetailedPlanetRenderer(float radius,
                                               QVector3D const& oblateness)
    : radius(radius)
    , oblateness(oblateness)
{
	cubemapDiffuse = GLHandler::newRenderTarget(cubemapsSize(), cubemapsSize(),
	                                            GL_RGBA16F, true);
}

void DetailedPlanetRenderer::initTerrestrial(float seed, QColor const& color,
                                             float polarLatitude,
                                             float atmosphere)
{
	valid         = true;
	shader        = GLHandler::newShader("planet/planet", {{"NORMAL", ""}});
	mesh          = Primitives::newUnitSphere(shader, 50, 50);
	normal        = true;
	cubemapNormal = GLHandler::newRenderTarget(cubemapsSize(), cubemapsSize(),
	                                           GL_RGBA16F, true);

	GLHandler::setShaderParam(shader, "diff", 0);
	GLHandler::setShaderParam(shader, "texRing", 1);
	GLHandler::setShaderParam(shader, "norm", 2);
	GLHandler::setShaderParam(shader, "atmosphere", atmosphere);

	// NOLINTNEXTLINE(cert-msc30-c, cert-msc50-c, cert-msc50-cpp)
	// float seed = 10000.f * static_cast<float>(rand()) / INT_MAX;

	GLHandler::ShaderProgram sdiff = GLHandler::newShader(
	    "planet/planet", "planet/gentex/terrestrialdiff");
	GLHandler::setShaderParam(sdiff, "color", color);
	GLHandler::setShaderParam(sdiff, "polarLatitude", polarLatitude);
	GLHandler::setShaderParam(sdiff, "seed", seed);
	GLHandler::Mesh mdiff = Primitives::newUnitSphere(sdiff, 50, 50);

	envMap(sdiff, mdiff, cubemapDiffuse);

	GLHandler::deleteMesh(mdiff);
	GLHandler::deleteShader(sdiff);

	GLHandler::ShaderProgram snorm = GLHandler::newShader(
	    "planet/planet", "planet/gentex/terrestrialnorm");
	GLHandler::setShaderParam(snorm, "color", color);
	GLHandler::setShaderParam(snorm, "polarLatitude", polarLatitude);
	GLHandler::setShaderParam(snorm, "seed", seed);
	GLHandler::Mesh mnorm = Primitives::newUnitSphere(snorm, 50, 50);

	envMap(snorm, mnorm, cubemapNormal);

	GLHandler::deleteMesh(mnorm);
	GLHandler::deleteShader(snorm);
}

void DetailedPlanetRenderer::initGazGiant(float seed, QColor const& color,
                                          float bandsIntensity,
                                          float stormsIntensity)
{
	valid  = true;
	shader = GLHandler::newShader("planet/planet");
	mesh   = Primitives::newUnitSphere(shader, 50, 50);

	GLHandler::setShaderParam(shader, "diff", 0);
	GLHandler::setShaderParam(shader, "texRing", 1);
	// NOLINTNEXTLINE(cert-msc30-c, cert-msc50-c, cert-msc50-cpp)
	// float seed = 10000.f * static_cast<float>(rand()) / INT_MAX;

	GLHandler::ShaderProgram sdiff
	    = GLHandler::newShader("planet/planet", "planet/gentex/gazgiantdiff");
	GLHandler::setShaderParam(sdiff, "color", color);
	GLHandler::setShaderParam(sdiff, "bandsIntensity", bandsIntensity);
	GLHandler::setShaderParam(sdiff, "stormsIntensity", stormsIntensity);
	GLHandler::setShaderParam(sdiff, "seed", seed);
	GLHandler::Mesh mdiff = Primitives::newUnitSphere(sdiff, 50, 50);

	envMap(sdiff, mdiff, cubemapDiffuse);

	GLHandler::deleteMesh(mdiff);
	GLHandler::deleteShader(sdiff);
}

void DetailedPlanetRenderer::initFromTex(QString const& diffusePath,
                                         float atmosphere)
{
	shader = GLHandler::newShader("planet/planet");
	mesh   = Primitives::newUnitSphere(shader, 50, 50);

	loadParallel(diffusePath, 0);
	this->atmosphere = atmosphere;
}

void DetailedPlanetRenderer::initFromTex(QString const& diffusePath,
                                         QString const& normalPath,
                                         float atmosphere)
{
	shader = GLHandler::newShader("planet/planet", {{"NORMAL", ""}});
	mesh   = Primitives::newUnitSphere(shader, 50, 50);

	loadParallel(diffusePath, 0);
	loadParallel(normalPath, 1);
	this->atmosphere = atmosphere;
}

void DetailedPlanetRenderer::updateModel(QString const& modelName)
{
	loadModelParallel(modelName);
}

void DetailedPlanetRenderer::updateTextureLoading(bool cancelLoading)
{
	if(futures.empty())
	{
		return;
	}

	for(auto& f : futures)
	{
		if(!f.isFinished())
		{
			return;
		}
	}

	if(cancelLoading)
	{
		valid = true;
		GLHandler::deletePixelBufferObject(pbos[0]);
		if(futures.size() > 1)
		{
			GLHandler::deletePixelBufferObject(pbos[1]);
		}
		futures.resize(0);
		futures.shrink_to_fit();
		return;
	}

	valid = true;
	GLHandler::setShaderParam(shader, "diff", 0);
	GLHandler::setShaderParam(shader, "texRing", 1);
	GLHandler::setShaderParam(shader, "atmosphere", atmosphere);

	GLHandler::ShaderProgram sdiff
	    = GLHandler::newShader("planet/planet", "planet/gentex/difftocube");
	GLHandler::Mesh mdiff = Primitives::newUnitSphere(sdiff, 50, 50);

	GLHandler::Texture tdiff = GLHandler::copyPBOToTex(pbos[0]);
	GLHandler::deletePixelBufferObject(pbos[0]);

	GLHandler::useTextures({tdiff});
	envMap(sdiff, mdiff, cubemapDiffuse);

	GLHandler::deleteTexture(tdiff);
	GLHandler::deleteMesh(mdiff);
	GLHandler::deleteShader(sdiff);

	if(futures.size() > 1)
	{
		normal        = true;
		cubemapNormal = GLHandler::newRenderTarget(
		    cubemapsSize(), cubemapsSize(), GL_RGBA16F, true);
		GLHandler::setShaderParam(shader, "norm", 2);

		GLHandler::ShaderProgram snorm
		    = GLHandler::newShader("planet/planet", "planet/gentex/normtocube");
		GLHandler::Mesh mnorm = Primitives::newUnitSphere(snorm, 50, 50);

		GLHandler::Texture tnorm = GLHandler::copyPBOToTex(pbos[1], false);
		GLHandler::deletePixelBufferObject(pbos[1]);

		GLHandler::useTextures({tnorm});
		envMap(snorm, mnorm, cubemapNormal);

		GLHandler::deleteTexture(tnorm);
		GLHandler::deleteMesh(mnorm);
		GLHandler::deleteShader(snorm);
	}
	else
	{
		GLHandler::setShaderParam(shader, "texRing", 1);
	}
	futures.resize(0);
	futures.shrink_to_fit();
}

float DetailedPlanetRenderer::updateModelLoading()
{
	if(!modelIsLoading)
	{
		return lastSphereVal;
	}

	if(!modelFuture.isFinished())
	{
		return lastSphereVal;
	}

	modelIsLoading = false;

	lastSphereVal = modelFuture.result();

	std::vector<GLHandler::Mesh> meshes;
	std::vector<GLHandler::Texture> textures;
	AssetLoader::loadModel(loadedVertices, loadedIndices, loadedTexs, meshes,
	                       textures, shader);

	if(meshes.size() == 1)
	{
		GLHandler::deleteMesh(mesh);
		mesh = meshes[0];
	}
	else
	{
		for(auto vMesh : meshes)
		{
			GLHandler::deleteMesh(vMesh);
		}
	}

	for(auto vTex : textures)
	{
		GLHandler::deleteTexture(vTex);
	}

	loadedVertices.resize(0);
	loadedVertices.shrink_to_fit();
	loadedIndices.resize(0);
	loadedIndices.shrink_to_fit();
	loadedTexs.resize(0);
	loadedTexs.shrink_to_fit();

	return lastSphereVal;
}

void DetailedPlanetRenderer::initRings(float seed, float innerRing,
                                       float outerRing,
                                       QString const& texturePath)
{
	rings = new Rings(seed, innerRing, outerRing, radius, oblateness,
	                  texturePath);
	GLHandler::setShaderParam(shader, "innerRing", innerRing);
	GLHandler::setShaderParam(shader, "outerRing", outerRing);
}

void DetailedPlanetRenderer::render(
    QVector3D const& pos, QVector3D const& lightpos, float lightradius,
    QColor const& lightcolor,
    std::array<QVector4D, 5> const& neighborsPosRadius,
    std::array<QVector3D, 5> const& neighborsOblateness,
    QMatrix4x4 const& properRotation, bool flipCoords)
{
	QMatrix4x4 model;

	model.translate(pos);
	model.scale(radius);

	render(model, lightpos, lightradius, lightcolor, neighborsPosRadius,
	       neighborsOblateness, properRotation, flipCoords);
}

void DetailedPlanetRenderer::render(
    QMatrix4x4 const& model, QVector3D const& lightpos, float lightradius,
    QColor const& lightcolor,
    std::array<QVector4D, 5> const& neighborsPosRadius,
    std::array<QVector3D, 5> const& neighborsOblateness,
    QMatrix4x4 const& properRotation, bool flipCoords)
{
	GLHandler::setShaderParam(shader, "lightpos", lightpos);
	GLHandler::setShaderParam(shader, "lightradius", lightradius);
	GLHandler::setShaderParam(shader, "lightcolor", lightcolor);
	GLHandler::setShaderParam(shader, "neighborsPosRadius", 5,
	                          &(neighborsPosRadius[0]));
	GLHandler::setShaderParam(shader, "neighborsOblateness", 5,
	                          &(neighborsOblateness[0]));
	GLHandler::setShaderParam(shader, "properRotation", properRotation);
	GLHandler::setShaderParam(shader, "oblateness", oblateness);
	if(flipCoords)
	{
		GLHandler::setShaderParam(shader, "flipCoords", QVector2D(-1.f, -1.f));
	}

	std::vector<GLHandler::Texture> textures;
	textures.push_back(GLHandler::getColorAttachmentTexture(cubemapDiffuse));
	if(rings != nullptr)
	{
		textures.push_back(rings->getTexture());
	}
	else
	{
		textures.push_back({});
	}

	if(normal)
	{
		textures.push_back(GLHandler::getColorAttachmentTexture(cubemapNormal));
	}

	GLHandler::useTextures(textures);
	GLHandler::setUpRender(shader, model);
	GLHandler::render(mesh);

	if(rings != nullptr)
	{
		rings->render(model, lightpos, lightradius, lightcolor,
		              neighborsPosRadius, neighborsOblateness, properRotation);
	}
}

void DetailedPlanetRenderer::loadParallel(QString const& path,
                                          unsigned int index)
{
	unsigned int texmaxsize(QSettings().value("quality/texmaxsize").toUInt());
	pbos[index]
	    = GLHandler::newPixelBufferObject(texmaxsize * 512, texmaxsize * 256);
	unsigned char* data(pbos[index].mappedData);

	futures.push_back(QtConcurrent::run([path, data, texmaxsize]() {
		QImageReader imgReader(path);
		imgReader.setScaledSize(QSize(texmaxsize * 512, texmaxsize * 256));
		QImage img(imgReader.read());
		if(img.isNull())
		{
			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
			qWarning() << "Could not load Texture '" + path
			                  + "' : " + imgReader.errorString();
			return;
		}
		img = img.convertToFormat(QImage::Format_RGBA8888);
		std::memcpy(data, img.bits(), std::size_t(img.byteCount()));
	}));
}

void DetailedPlanetRenderer::loadModelParallel(QString const& path)
{
	modelIsLoading = true;
	modelFuture    = QtConcurrent::run([path, this]() {
        return AssetLoader::loadFile(path, this->loadedVertices,
                                     this->loadedIndices, this->loadedTexs);
    });
}

void DetailedPlanetRenderer::envMap(GLHandler::ShaderProgram& shader,
                                    GLHandler::Mesh& mesh,
                                    GLHandler::RenderTarget& renderTarget)
{
	GLHandler::generateEnvironmentMap(renderTarget, [shader, mesh]() {
		GLHandler::setBackfaceCulling(false);
		GLHandler::setUpRender(shader);
		GLHandler::render(mesh);
		GLHandler::setBackfaceCulling(true);
	});
}

DetailedPlanetRenderer::~DetailedPlanetRenderer()
{
	for(auto& future : futures)
	{
		future.waitForFinished();
	}
	updateTextureLoading(true);
	modelFuture.waitForFinished();
	updateModelLoading();

	GLHandler::deleteRenderTarget(cubemapDiffuse);
	if(normal)
	{
		GLHandler::deleteRenderTarget(cubemapNormal);
	}

	GLHandler::deleteMesh(mesh);
	GLHandler::deleteShader(shader);

	if(rings != nullptr)
	{
		delete rings;
	}
}
