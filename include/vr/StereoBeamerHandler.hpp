#ifndef STEREOBEAMERHANDLER_H
#define STEREOBEAMERHANDLER_H

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
	virtual float getRenderTargetAverageLuminance(Side eye) const override;
	// same order as in HmdQuad_t struct
	virtual QMatrix4x4 getSeatedToStandingAbsoluteTrackingPos() const override;
	virtual QSizeF getPlayAreaSize() const override;
	virtual std::vector<QVector3D> getPlayAreaQuad() const override;
	virtual void prepareRendering() override;
	virtual void beginRendering(Side eye) override;
	virtual void renderControllers() const override;
	virtual void renderHands() const override;
	GLFramebufferObject const& getPostProcessingTarget(unsigned int i,
	                                                   Side side) const override
	{
		return side == Side::LEFT ? *postProcessingTargetsLeft[i]
		                          : *postProcessingTargetsRight[i];
	};
	virtual void reloadPostProcessingTargets() override;
	virtual void submitRendering(Side eye, unsigned int i) override;
	virtual void
	    displayOnCompanion(unsigned int companionWidth,
	                       unsigned int companionHeight) const override;
	virtual bool pollEvent(Event* e) override;
	virtual void close() override;
	~StereoBeamerHandler() { close(); };

  public slots:
	virtual QMatrix4x4 getEyeViewMatrix(Side eye) const override;
	virtual QMatrix4x4 getProjectionMatrix(Side eye, float nearPlan,
	                                       float farPlan) const override;
	virtual void resetPos() override;

  private:
	bool enabled                = false;
	unsigned int submittedIndex = 0;
	std::array<GLFramebufferObject*, 2> postProcessingTargetsLeft
	    = {{nullptr, nullptr}};
	std::array<GLFramebufferObject*, 2> postProcessingTargetsRight
	    = {{nullptr, nullptr}};
	QSize currentTargetSize;
};

#endif // STEREOBEAMERHANDLER_H
