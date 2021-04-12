#ifndef STEREOBEAMERHANDLER_H
#define STEREOBEAMERHANDLER_H

#ifdef LEAP_MOTION
#include <Leap.h>
#endif

#include <array>
#include <iostream>
#include <openvr/openvr.h>

#include "../PythonQtHandler.hpp"
#include "Controller.hpp"
#include "Hand.hpp"
#include "VRHandler.hpp"
class BasicCamera;

/** @ingroup pycall
 *
 * Callable in Python as the "VRHandler" object when relevant (VR is enabled).
 */
class StereoBeamerHandler : public VRHandler
{
	Q_OBJECT
  public:
	StereoBeamerHandler() = default;
	virtual QString getDriverName() const override { return "StereoBeamer"; }
	virtual bool isEnabled() const override { return enabled; }
	virtual bool init(Renderer const& renderer) override;
	virtual QSize getEyeRenderTargetSize() const override;
	virtual float getFrameTiming() const override;
	virtual const Controller* getController(Side side) const override;
	virtual const Hand* getHand(Side side) const override;
	// same order as in HmdQuad_t struct
	virtual QMatrix4x4 getSeatedToStandingAbsoluteTrackingPos() const override;
	virtual QSizeF getPlayAreaSize() const override;
	virtual std::vector<QVector3D> getPlayAreaQuad() const override;
	virtual void prepareRendering(Side eye) override;
	virtual void renderControllers() const override;
	virtual void renderHands() const override;
	virtual void submitRendering(GLFramebufferObject const& fbo) override;
	virtual bool pollEvent(Event* e) override;
	virtual void close() override;
	~StereoBeamerHandler() { close(); };

  public slots:
	virtual QMatrix4x4 getEyeViewMatrix(Side eye) const override;
	virtual QMatrix4x4 getProjectionMatrix(Side eye, float nearPlan,
	                                       float farPlan) const override;
	virtual void resetPos() override;

  private:
	bool enabled = false;
};

#endif // STEREOBEAMERHANDLER_H
