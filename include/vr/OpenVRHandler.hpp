#ifndef OPENVRHANDLER_H
#define OPENVRHANDLER_H

#ifdef LEAP_MOTION
#include <Leap.h>
#endif

#include <array>
#include <iostream>
#include <openvr.h>

#include "../PythonQtHandler.hpp"
#include "Controller.hpp"
#include "Hand.hpp"
#include "VRHandler.hpp"
class BasicCamera;

/** @ingroup pycall
 *
 * Callable in Python as the "VRHandler" object when relevant (VR is enabled).
 */
class OpenVRHandler : public VRHandler
{
	Q_OBJECT
  public:
	OpenVRHandler() = default;
	virtual QString getDriverName() const override { return "OpenVR"; }
	virtual bool isEnabled() const override { return vr_pointer != nullptr; }
	virtual bool init(Renderer const& renderer) override;
	virtual QSize getEyeRenderTargetSize() const override;
	virtual float getFrameTiming() const override;
	virtual const Controller* getController(Side side) const override;
	virtual const Hand* getHand(Side side) const override;
	virtual float getRenderTargetAverageLuminance(Side eye) const override;
	// same order as in HmdQuad_t struct
	virtual QMatrix4x4 getSeatedToStandingAbsoluteTrackingPos() const override;
	virtual QSizeF getPlayAreaSize() const override;
	virtual std::vector<QVector3D> getPlayAreaQuad() const override;
	virtual void prepareRendering() override;
	virtual void beginRendering(Side eye) override;
	virtual void renderControllers() const override;
	virtual void renderHands() const override;
	GLHandler::RenderTarget const&
	    getPostProcessingTarget(unsigned int i, Side side) const override
	{
		return side == Side::LEFT ? postProcessingTargetsLeft[i]
		                          : postProcessingTargetsRight[i];
	};
	virtual void reloadPostProcessingTargets() override;
	virtual void submitRendering(Side eye, unsigned int i) override;
	virtual void
	    displayOnCompanion(unsigned int companionWidth,
	                       unsigned int companionHeight) const override;
	virtual bool pollEvent(Event* e) override;
	virtual void close() override;
	~OpenVRHandler() { close(); };

  public slots:
	virtual QMatrix4x4 getEyeViewMatrix(Side eye) const override;
	virtual QMatrix4x4 getProjectionMatrix(Side eye, float nearPlan,
	                                       float farPlan) const override;
	virtual void resetPos() override;

  private:
	vr::IVRSystem* vr_pointer        = nullptr;
	vr::IVRCompositor* vr_compositor = nullptr;
	std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount>
	    tracked_device_pose;
	std::array<QMatrix4x4, vr::k_unMaxTrackedDeviceCount>
	    tracked_device_pose_matrix;
	vr::IVRRenderModels* vr_render_models;

	Controller* leftController  = nullptr;
	Controller* rightController = nullptr;

#ifdef LEAP_MOTION
	Leap::Controller leapController;
#endif
	Hand* leftHand  = nullptr;
	Hand* rightHand = nullptr;

	unsigned int submittedIndex = 0;
	GLHandler::RenderTarget postProcessingTargetsLeft[2];
	GLHandler::RenderTarget postProcessingTargetsRight[2];
	QSize currentTargetSize;

	void updateController(Side side, int nDevice);
	void updateHands();

	vr::Hmd_Eye getEye(Side eye) const
	{
		return (eye == Side::LEFT) ? vr::Eye_Left : vr::Eye_Right;
	};
	static QVector3D toQt(const vr::HmdVector3_t& vector);
	static QMatrix4x4 toQt(const vr::HmdMatrix34_t& matrix);
	static QMatrix4x4 toQt(const vr::HmdMatrix44_t& matrix);
	static Button getButton(int openvrButton);
};

#endif // OPENVRHANDLER_H
