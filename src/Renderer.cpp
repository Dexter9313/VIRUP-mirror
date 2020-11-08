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

#include "Renderer.hpp"

#include "AbstractMainWin.hpp"

Renderer::Renderer(AbstractMainWin& window, VRHandler& vrHandler)
    : window(window)
    , vrHandler(vrHandler)
{
}

void Renderer::init()
{
	if(initialized)
	{
		clean();
	}

	vFOV = QSettings().value("graphics/vfov").toDouble();
	hFOV = QSettings().value("graphics/hfov").toDouble();
	if(vFOV == 0.0)
	{
		if(hFOV == 0.0)
		{
			vFOV = 70.0;
		}
		else
		{
			vFOV = hFOV / getAspectRatio();
		}
	}
	if(hFOV == 0.0)
	{
		hFOV = getAspectRatio() * vFOV;
	}

	dbgCamera = new DebugCamera(vrHandler);
	dbgCamera->lookAt({2, 0, 2}, {0, 0, 0}, {0, 0, 1});
	dbgCamera->setPerspectiveProj(vFOV, hFOV / vFOV);

	auto defaultCam = new BasicCamera(vrHandler);
	defaultCam->lookAt({1, 1, 1}, {0, 0, 0}, {0, 0, 1});
	defaultCam->setPerspectiveProj(vFOV, hFOV / vFOV);
	appendSceneRenderPath("default", RenderPath(defaultCam));

	reloadPostProcessingTargets();
	updateAngleShiftMat();

	initialized = true;
}

void Renderer::windowResized()
{
	if(!initialized)
	{
		return;
	}

	vFOV = QSettings().value("graphics/vfov").toDouble();
	hFOV = QSettings().value("graphics/hfov").toDouble();
	if(vFOV == 0.0)
	{
		if(hFOV == 0.0)
		{
			vFOV = 70.0;
		}
		else
		{
			vFOV = hFOV / getAspectRatio();
		}
	}
	if(hFOV == 0.0)
	{
		hFOV = getAspectRatio() * vFOV;
	}

	QSettings().setValue("window/width", window.size().width());
	QSettings().setValue("window/height", window.size().height());
	if(QSettings().value("window/forcerenderresolution").toBool())
	{
		return;
	}
	for(auto pair : sceneRenderPipeline_)
	{
		pair.second.camera->setPerspectiveProj(vFOV, hFOV / vFOV);
	}
	dbgCamera->setPerspectiveProj(vFOV, hFOV / vFOV);
	reloadPostProcessingTargets();
}

QSize Renderer::getSize() const
{
	QSize renderSize(window.size().width(), window.size().height());
	if(QSettings().value("window/forcerenderresolution").toBool())
	{
		renderSize.setWidth(QSettings().value("window/forcewidth").toInt());
		renderSize.setHeight(QSettings().value("window/forceheight").toInt());
	}
	return renderSize;
}

float Renderer::getAspectRatio() const
{
	QSize renderSize(getSize());
	float aspectRatio(static_cast<float>(renderSize.width())
	                  / static_cast<float>(renderSize.height()));
	return aspectRatio;
}

BasicCamera const& Renderer::getCamera(QString const& pathId) const
{
	for(auto const& pair : sceneRenderPipeline)
	{
		if(pair.first == pathId)
		{
			return *(pair.second.camera);
		}
	}
	throw(std::domain_error(std::string("Path id doesn't exist.")
	                        + pathId.toStdString()));
}

BasicCamera& Renderer::getCamera(QString const& pathId)
{
	for(auto const& pair : sceneRenderPipeline)
	{
		if(pair.first == pathId)
		{
			return *(pair.second.camera);
		}
	}
	throw(std::domain_error(std::string("Path id doesn't exist.")
	                        + pathId.toStdString()));
}

QImage Renderer::getLastFrame() const
{
	return GLHandler::generateScreenshot(
	           postProcessingTargets.at(postProcessingPipeline_.size() % 2))
	    .mirrored(false, true);
}

void Renderer::appendSceneRenderPath(QString const& id, RenderPath path)
{
	sceneRenderPipeline_.append(QPair<QString, RenderPath>(id, path));
}

void Renderer::insertSceneRenderPath(QString const& id, RenderPath path,
                                     unsigned int pos)
{
	sceneRenderPipeline_.insert(pos, QPair<QString, RenderPath>(id, path));
}

void Renderer::removeSceneRenderPath(QString const& id)
{
	for(int i(0); i < sceneRenderPipeline_.size(); ++i)
	{
		if(sceneRenderPipeline_[i].first == id)
		{
			delete sceneRenderPipeline_[i].second.camera;
			sceneRenderPipeline_.removeAt(i);
			break;
		}
	}
}

void Renderer::appendPostProcessingShader(QString const& id,
                                          QString const& fragment,
                                          QMap<QString, QString> const& defines)
{
	postProcessingPipeline_.emplace_back(
	    std::make_pair(id, GLShaderProgram("postprocess", fragment, defines)));
}

void Renderer::insertPostProcessingShader(QString const& id,
                                          QString const& fragment,
                                          unsigned int pos)
{
	postProcessingPipeline_.emplace(
	    std::next(postProcessingPipeline_.begin(), pos),
	    std::make_pair(id, GLShaderProgram("postprocess", fragment)));
}

void Renderer::removePostProcessingShader(QString const& id)
{
	for(auto it(postProcessingPipeline_.begin());
	    it != postProcessingPipeline_.end(); ++it)
	{
		if(it->first == id)
		{
			postProcessingPipeline_.erase(it);
			break;
		}
	}
}

void Renderer::reloadPostProcessingTargets()
{
	QSize newSize(getSize());

	GLHandler::defaultRenderTargetFormat() = GL_RGBA32F;

	GLHandler::deleteRenderTarget(postProcessingTargets[0]);
	GLHandler::deleteRenderTarget(postProcessingTargets[1]);
	postProcessingTargets[0]
	    = GLHandler::newRenderTarget(newSize.width(), newSize.height());
	postProcessingTargets[1]
	    = GLHandler::newRenderTarget(newSize.width(), newSize.height());

	if(QSettings().value("graphics/antialiasing").toUInt() > 0)
	{
		GLHandler::deleteRenderTarget(multisampledTarget);
		multisampledTarget = GLHandler::newRenderTargetMultisample(
		    newSize.width(), newSize.height(),
		    static_cast<unsigned int>(1)
		        << QSettings().value("graphics/antialiasing").toUInt(),
		    GL_RGBA32F);
	}

	if(vrHandler.isEnabled())
	{
		vrHandler.reloadPostProcessingTargets();
	}
}

void Renderer::updateAngleShiftMat()
{
	angleShiftMat = QMatrix4x4();
	if(!QSettings().value("network/server").toBool())
	{
		angleShiftMat.rotate(QSettings().value("network/angleshift").toDouble(),
		                     QVector3D(0.f, 1.f, 0.f));
	}
}

void Renderer::renderVRControls() const
{
	if(vrHandler.isEnabled())
	{
		vrHandler.renderControllers();
		vrHandler.renderHands();
	}
}

void Renderer::vrRenderSinglePath(RenderPath& renderPath, QString const& pathId,
                                  bool debug, bool debugInHeadset)
{
	GLHandler::glf().glClear(renderPath.clearMask);
	renderPath.camera->update(angleShiftMat);
	dbgCamera->update(angleShiftMat);

	if(debug && debugInHeadset)
	{
		dbgCamera->uploadMatrices();
	}
	else
	{
		renderPath.camera->uploadMatrices();
	}
	if(pathIdRenderingControllers == pathId && renderControllersBeforeScene)
	{
		renderVRControls();
	}
	// render scene
	if(wireframe)
	{
		GLHandler::beginWireframe();
	}
	window.renderScene(*renderPath.camera, pathId);
	if(pathIdRenderingControllers == pathId && !renderControllersBeforeScene)
	{
		renderVRControls();
	}
	PythonQtHandler::evalScript(
	    "if \"renderScene\" in dir():\n\trenderScene()");
	if(debug && debugInHeadset)
	{
		dbgCamera->renderCamera(renderPath.camera);
	}
	if(wireframe)
	{
		GLHandler::endWireframe();
	}
}

void Renderer::vrRender(Side side, bool debug, bool debugInHeadset)
{
	vrHandler.beginRendering(side);

	for(auto pair : sceneRenderPipeline_)
	{
		pair.second.camera->setWindowSize(getSize());
		vrRenderSinglePath(pair.second, pair.first, debug, debugInHeadset);
	}

	lastFrameAverageLuminance
	    += vrHandler.getRenderTargetAverageLuminance(side);

	// do all postprocesses including last one
	int i(0);
	for(auto it(postProcessingPipeline_.begin());
	    it != postProcessingPipeline_.end(); ++it, ++i)
	{
		window.applyPostProcShaderParams(
		    it->first, it->second,
		    vrHandler.getPostProcessingTarget(i % 2, side));
		auto texs = window.getPostProcessingUniformTextures(
		    it->first, it->second,
		    vrHandler.getPostProcessingTarget(i % 2, side));
		GLHandler::postProcess(
		    it->second, vrHandler.getPostProcessingTarget(i % 2, side),
		    vrHandler.getPostProcessingTarget((i + 1) % 2, side), texs);
	}

	vrHandler.submitRendering(side, i % 2);
}

void Renderer::renderFrame()
{
	bool debug(dbgCamera->isEnabled());
	bool debugInHeadset(dbgCamera->debugInHeadset());
	bool renderingCamIsDebug(debug
	                         && ((debugInHeadset && vrHandler.isEnabled())
	                             || !vrHandler.isEnabled()));
	bool thirdRender(QSettings().value("vr/thirdrender").toBool());

	// main render logic
	if(vrHandler.isEnabled())
	{
		vrHandler.prepareRendering();

		lastFrameAverageLuminance = 0.f;
		vrRender(Side::LEFT, debug, debugInHeadset);
		vrRender(Side::RIGHT, debug, debugInHeadset);
		lastFrameAverageLuminance *= 0.5f;

		if(!thirdRender && (!debug || debugInHeadset))
		{
			vrHandler.displayOnCompanion(window.size().width(),
			                             window.size().height());
		}
		else if(debug && !debugInHeadset)
		{
			renderingCamIsDebug = true;
		}
	}
	// if no VR or debug not in headset, render 2D
	if((!vrHandler.isEnabled() || thirdRender) || (debug && !debugInHeadset))
	{
		auto renderFunc = [=](bool overrideCamera, QMatrix4x4 overrView,
		                      QMatrix4x4 overrProj) {
			for(auto pair : sceneRenderPipeline_)
			{
				pair.second.camera->setWindowSize(getSize());
				GLHandler::glf().glClear(pair.second.clearMask);
				QMatrix4x4 viewBack(pair.second.camera->getView()),
				    projBack(pair.second.camera->getProj());
				if(overrideCamera)
				{
					pair.second.camera->setProj(overrProj);
					pair.second.camera->setView(overrView * viewBack);
				}
				pair.second.camera->update2D(angleShiftMat);
				dbgCamera->update(angleShiftMat);
				if(renderingCamIsDebug)
				{
					dbgCamera->uploadMatrices();
				}
				else
				{
					pair.second.camera->uploadMatrices();
				}
				// render scene
				if(wireframe)
				{
					GLHandler::beginWireframe();
				}

				window.renderScene(*pair.second.camera, pair.first);
				PythonQtHandler::evalScript(
				    "if \"renderScene\" in dir():\n\trenderScene()");
				if(debug)
				{
					dbgCamera->renderCamera(pair.second.camera);
				}
				if(wireframe)
				{
					GLHandler::endWireframe();
				}
				if(overrideCamera)
				{
					pair.second.camera->setProj(projBack);
					pair.second.camera->setView(viewBack);
				}
			}
		};

		if(projection == Projection::DEFAULT)
		{
			if(cubemapTargetInit)
			{
				GLHandler::deleteRenderTarget(cubemapTarget);
				cubemapTargetInit = false;
			}
			if(QSettings().value("graphics/antialiasing").toUInt() == 0)
			{
				GLHandler::beginRendering(postProcessingTargets[0]);
				renderFunc(false, QMatrix4x4(), QMatrix4x4());
			}
			else
			{
				GLHandler::beginRendering(multisampledTarget);
				renderFunc(false, QMatrix4x4(), QMatrix4x4());
				GLHandler::blitColorBuffer(multisampledTarget,
				                           postProcessingTargets[0]);
			}
		}
		else if(projection == Projection::PANORAMA360)
		{
			if(!cubemapTargetInit)
			{
				unsigned int side(getSize().width() / 3);
				cubemapTarget = GLHandler::newRenderTarget(side, side, true);
				cubemapTargetInit = true;
			}
			GLHandler::generateEnvironmentMap(cubemapTarget, renderFunc);

			GLShaderProgram shader("postprocess", "panorama360");
			GLHandler::postProcess(shader, cubemapTarget,
			                       postProcessingTargets[0]);
		}
		else if(projection == Projection::VR360)
		{
			if(!cubemapTargetInit)
			{
				unsigned int side(getSize().width() / 3);
				cubemapTarget = GLHandler::newRenderTarget(side, side, true);
				cubemapTargetInit = true;
			}
			int tgtWidth(postProcessingTargets[0].getSize().width()),
			    tgtHeight(postProcessingTargets[0].getSize().height());
			QVector3D shift(0.065, 0.0, 0.0);

			GLHandler::generateEnvironmentMap(cubemapTarget, renderFunc, shift);
			GLShaderProgram shader("postprocess", "panorama360");
			GLHandler::postProcess(shader, cubemapTarget,
			                       postProcessingTargets[0]);
			GLHandler::blitColorBuffer(
			    postProcessingTargets[0], postProcessingTargets[1], 0, 0,
			    tgtWidth, tgtHeight, 0, 0, tgtWidth, tgtHeight / 2);

			GLHandler::generateEnvironmentMap(cubemapTarget, renderFunc,
			                                  -shift);
			GLHandler::postProcess(shader, cubemapTarget,
			                       postProcessingTargets[0]);
			GLHandler::blitColorBuffer(
			    postProcessingTargets[0], postProcessingTargets[1], 0, 0,
			    tgtWidth, tgtHeight, 0, tgtHeight / 2, tgtWidth, tgtHeight);
			GLHandler::blitColorBuffer(postProcessingTargets[1],
			                           postProcessingTargets[0]);
		}
		else
		{
			qDebug() << "Invalid RenderTarget::Projection";
		}

		// compute average luminance
		auto tex
		    = GLHandler::getColorAttachmentTexture(postProcessingTargets[0]);
		lastFrameAverageLuminance = GLHandler::getTextureAverageLuminance(tex);

		// postprocess
		int i(0);
		for(auto it(postProcessingPipeline_.begin());
		    it != postProcessingPipeline_.end(); ++i, ++it)
		{
			window.applyPostProcShaderParams(it->first, it->second,
			                                 postProcessingTargets.at(i % 2));
			auto texs = window.getPostProcessingUniformTextures(
			    it->first, it->second, postProcessingTargets.at(i % 2));
			GLHandler::postProcess(it->second, postProcessingTargets.at(i % 2),
			                       postProcessingTargets.at((i + 1) % 2), texs);
		}
		// blit result on screen
		GLHandler::blitColorBuffer(
		    postProcessingTargets.at(postProcessingPipeline_.size() % 2),
		    GLHandler::getScreenRenderTarget());
	}
}

void Renderer::clean()
{
	if(!initialized)
	{
		return;
	}

	for(auto const& pair : sceneRenderPipeline_)
	{
		delete pair.second.camera;
	}
	delete dbgCamera;

	if(cubemapTargetInit)
	{
		GLHandler::deleteRenderTarget(cubemapTarget);
	}
	GLHandler::deleteRenderTarget(multisampledTarget);
	GLHandler::deleteRenderTarget(postProcessingTargets[0]);
	GLHandler::deleteRenderTarget(postProcessingTargets[1]);

	initialized = false;
}

Renderer::~Renderer()
{
	clean();
}
