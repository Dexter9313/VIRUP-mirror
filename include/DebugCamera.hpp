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

#ifndef DEBUGCAMERA_H
#define DEBUGCAMERA_H

#include "BasicCamera.hpp"

/**
 * @brief A camera that the scene doesn't know about to see how the scene reacts
 * to the rendering camera from another point of view.
 *
 * Its matrix transformations are independant from the rendering camera. When
 * enabled, you will see a mesh representing the real-time view frustum of the
 * rendering camera. If VR is enabled and @ref followhmd is set to true, two
 * meshes will be used to display the view frustums of both eyes, red for left
 * and green for right.
 *
 * This camera won't be passed to AbstractMainWin#updateScene or
 * AbstractMainWin#renderScene so that the scene will be tricked to believe the
 * rendering camera is the true camera with which it will be rendered. That way
 * you can graphically debug frustum culling for example.
 */
class DebugCamera : public BasicCamera
{
	Q_OBJECT
	/**
	 * @brief Wether the debug camera mode is enabled or not.
	 *
	 * @accessors isEnabled(), setEnabled()
	 */
	Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
	/**
	 * @brief Wether the debug camera should be used in the HMD or not.
	 *
	 * Only useful if VR is enabled.
	 *
	 * If false, the companion window will render its own 2D view of the scene
	 * using the debug camera.
	 *
	 * @attention If false and VR is enabled, the scene will be rendered three
	 * times : once normally for each eye and once in debug mode for the
	 * companion window.
	 */
	Q_PROPERTY(bool debuginheadset READ debugInHeadset WRITE setDebugInHeadset)
	/**
	 * @brief
	 *
	 * Only useful if VR is enabled.
	 *
	 * If false, the 2D version of the @ref BasicCamera view frustum will be
	 * drawn. If true, the VR version of the @ref BasicCamera view frustum will
	 * be drawn (both eyes and the user's head movements can be seen).
	 */
	Q_PROPERTY(bool followhmd READ followHMD WRITE setFollowHMD)

  public:
	/**
	 * @brief Constructs a @ref BasicCamera.
	 *
	 * The camera's full transformation will be the identity transform by
	 * default.
	 *
	 * @param vrHandler The engine's @ref VRHandler, wether it is active or not.
	 * It is mostly used to get VR transformations.
	 */
	explicit DebugCamera(VRHandler const& vrHandler);
	/**
	 * @getter{enabled}
	 */
	bool isEnabled() const;
	/**
	 * @setter{enabled, enabled}
	 */
	void setEnabled(bool enabled);
	/**
	 * @getter{debuginheadset}
	 */
	bool debugInHeadset() const;
	/**
	 * @setter{debuginheadset, debuginheadset}
	 */
	void setDebugInHeadset(bool debuginheadset);
	/**
	 * @getter{followhmd}
	 */
	bool followHMD() const;
	/**
	 * @setter{followhmd, followhmd}
	 */
	void setFollowHMD(bool followhmd);
	/**
	 * @brief Updates all the camera transformation matrices.
	 *
	 * See BasicCamera#update.
	 */
	virtual void update(QMatrix4x4 const& angleShiftMat) override;
	/**
	 * @brief Render's the @p cam's frustum mesh.
	 */
	void renderCamera(BasicCamera const* cam);

  public slots:
	/**
	 * @toggle{enabled}
	 */
	void toggle();
	/**
	 * @toggle{debuginheadset}
	 */
	void toggleDebugInHeadset();
	/**
	 * @toggle{followhmd}
	 */
	void toggleFollowHMD();

  private:
	GLMesh camMesh;
	GLShaderProgram camMeshShader;
};

#endif // DEBUGCAMERA_H
