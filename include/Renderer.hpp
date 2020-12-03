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

#ifndef RENDERER_H
#define RENDERER_H

#include <list>
#include <utility>

#include "BasicCamera.hpp"
#include "CalibrationCompass.hpp"
#include "DebugCamera.hpp"
#include "MainRenderTarget.hpp"
#include "vr/VRHandler.hpp"

class AbstractMainWin;

class Renderer
{
  public:
	struct RenderPath
	{
		explicit RenderPath(BasicCamera* camera)
		    : camera(camera){};
		RenderPath(GLbitfield clearMask, BasicCamera* camera)
		    : clearMask(clearMask)
		    , camera(camera){};

		GLbitfield clearMask = 0x0; // don't clear anything by default
		BasicCamera* camera;
	};

	Renderer(AbstractMainWin& window, VRHandler& vrHandler);
	void init();
	void windowResized();
	QSize getSize() const;
	float getRenderTargetAspectRatio() const;
	float getAspectRatioFromFOV() const;
	float getVerticalFOV() const { return vFOV; };
	float getHorizontalFOV() const { return hFOV; };
	/**
	 * @brief Returns a constant reference to the @ref BasicCamera used for
	 * rendering during specific scene rendering path.
	 *
	 * Can be of polymorphic type if you have your own cameras registered.
	 */
	BasicCamera const& getCamera(QString const& pathId) const;
	/**
	 * @brief Returns a constant reference to the @ref BasicCamera used for
	 * rendering during specific scene rendering path.
	 *
	 * Can be of polymorphic type if you have your own cameras registered.
	 */
	BasicCamera& getCamera(QString const& pathId);
	/**
	 * @brief Similar to @e getCamera() but dynamically casts the BasicCamera
	 * into the specified class.
	 *
	 * T must derive from @e BasicCamera.
	 */
	template <class T>
	T const& getCamera(QString const& pathId) const;
	/**
	 * @brief Similar to @e getCamera() but dynamically casts the BasicCamera
	 * into the specified class.
	 *
	 * T must derive from @e BasicCamera.
	 */
	template <class T>
	T& getCamera(QString const& pathId);
	/**
	 * @brief Returns a reference to the @ref DebugCamera of the engine.
	 */
	DebugCamera& getDebugCamera() { return *dbgCamera; };
	QImage getLastFrame() const;
	// /!\ ownership of path.camera
	void appendSceneRenderPath(QString const& id, RenderPath path);
	// /!\ ownership of path.camera
	void insertSceneRenderPath(QString const& id, RenderPath path,
	                           unsigned int pos);
	void removeSceneRenderPath(QString const& id);

	// in frame units
	float getLastFrameAverageLuminance() const
	{
		return lastFrameAverageLuminance;
	};
	/**
	 * @brief Appends a post-processing shader to the post-processing pipeline.
	 *
	 * The engine will take care of the vertex shader for you as long as you
	 * follow the post-processing instructions given in the class description.
	 *
	 * @param id Identifier to refer to the fragment shader later.
	 * @param fragment Path to the fragment shader to use. See README for
	 * informations about data paths.
	 */
	void appendPostProcessingShader(QString const& id, QString const& fragment,
	                                QMap<QString, QString> const& defines = {});
	/**
	 * @brief Inserts a post-processing shader into the post-processing
	 * pipeline.
	 *
	 * The engine will take care of the vertex shader for you as long as you
	 * follow the post-processing instructions given in the class description.
	 *
	 * @param id Identifier to refer to the shader later.
	 * @param fragment Path to the fragment shader to use. See README for
	 * informations about data paths.
	 * @param pos Position at which to insert the shader.
	 */
	void insertPostProcessingShader(QString const& id, QString const& fragment,
	                                unsigned int pos);
	/**
	 * @brief Removes a post-processing shader from the post-processing
	 * pipeline.
	 *
	 * @param id Identifier given when appending or inserting the shader.
	 */
	void removePostProcessingShader(QString const& id);
	/**
	 * @brief Reloads the post-processing targets (2D and VR).
	 *
	 * The new render targets will use @ref GLHandler#defaultRenderTargetFormat.
	 * Will also call @ref VRHandler#reloadPostProcessingTargets if necessary.
	 */
	void reloadPostProcessingTargets();
	void updateFOV();
	void updateAngleShiftMat();
	bool getCalibrationCompass() const { return renderCompass; };
	CalibrationCompass* getCalibrationCompassPtr() { return compass; };
	CalibrationCompass const* getCalibrationCompassPtr() const
	{
		return compass;
	};
	void setCalibrationCompass(bool on)
	{
		if(on != renderCompass)
		{
			toggleCalibrationCompass();
		}
	};
	void toggleCalibrationCompass();
	double getDoubleFarRightPixelSubtendedAngle()
	{
		return CalibrationCompass::getDoubleFarRightPixelSubtendedAngle(
		    hFOV, getSize().width());
	};

	void renderVRControls() const;
	void renderFrame();
	void clean();
	~Renderer();

	bool renderControllersBeforeScene  = true;
	QString pathIdRenderingControllers = "default";

	bool wireframe = false;
	MainRenderTarget::Projection projection
	    = MainRenderTarget::Projection::DEFAULT;
	/**
	 * @brief Ordered list of render passes to apply as scene rendering.
	 *
	 * This member is read-only.
	 *
	 * For each QPair p :
	 *
	 * * p.first is the path identifier set when adding or inserting it in the
	 * pipeline using the corresponding methods.
	 * * p.second is the render path itself.
	 */
	QList<QPair<QString, RenderPath>> const& sceneRenderPipeline
	    = sceneRenderPipeline_;
	/**
	 * @brief Ordered list of post-processing shaders to apply at the end of
	 * scene rendering.
	 *
	 * This member is read-only.
	 *
	 * For each QPair p :
	 *
	 * * p.first is the shader identifier set when adding or inserting it in the
	 * pipeline using the corresponding methods.
	 * * p.second is the shader itself.
	 */
	std::list<std::pair<QString, GLShaderProgram>> const& postProcessingPipeline
	    = postProcessingPipeline_;

  private:
	bool initialized = false;

	void vrRenderSinglePath(RenderPath& renderPath, QString const& pathId,
	                        bool debug, bool debugInHeadset);
	void vrRender(Side side, bool debug, bool debugInHeadset);

	AbstractMainWin& window;
	VRHandler& vrHandler;
	DebugCamera* dbgCamera = nullptr;

	QMatrix4x4 angleShiftMat;
	float vFOV = 0.f;
	float hFOV = 0.f;

	QList<QPair<QString, RenderPath>> sceneRenderPipeline_;

	std::list<std::pair<QString, GLShaderProgram>> postProcessingPipeline_;
	float lastFrameAverageLuminance = 0.f;

	bool renderCompass          = false;
	CalibrationCompass* compass = nullptr;

	MainRenderTarget* mainRenderTarget = nullptr;
};

template <class T>
T const& Renderer::getCamera(QString const& pathId) const
{
	return dynamic_cast<T const&>(getCamera(pathId));
}

template <class T>
T& Renderer::getCamera(QString const& pathId)
{
	return dynamic_cast<T&>(getCamera(pathId));
}

#endif // RENDERER_H
