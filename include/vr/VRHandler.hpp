#ifndef VRHANDLER_H
#define VRHANDLER_H

#ifdef LEAP_MOTION
#include <Leap.h>
#endif

#include <array>
#include <iostream>
#include <openvr.h>

#include "../GLHandler.hpp"
#include "../PythonQtHandler.hpp"
#include "Controller.hpp"
#include "Hand.hpp"

class BasicCamera;
class Controller;
class Hand;

class VRHandler : public QObject
{
	Q_OBJECT
  public: // public types
	enum class EventType
	{
		NONE,
		BUTTON_PRESSED,
		BUTTON_UNPRESSED
	};
	Q_ENUM(EventType)

	enum class Button
	{
		NONE,
		GRIP,
		TRIGGER,
		TOUCHPAD,
		MENU,
		SYSTEM
	};
	Q_ENUM(Button)

	struct Event
	{
		EventType type;
		Side side;
		Button button;
	};

  public:
	VRHandler() = default;
	explicit operator bool() const { return vr_pointer != nullptr; }
	bool init();
	const Controller* getController(Side side) const;
	const Hand* getHand(Side side) const;
	void prepareRendering();
	void beginRendering(Side eye, bool postProcessed);
	void renderControllers() const;
	void renderHands();
	GLHandler::RenderTarget& getEyeTarget(Side side)
	{
		return side == Side::LEFT ? leftTarget : rightTarget;
	};
	GLHandler::RenderTarget& getPostProcessingTarget(unsigned int i)
	{
		return postProcessingTargets[i];
	};
	void reloadPostProcessingTargets();
	void submitRendering(Side eye);
	void displayOnCompanion(unsigned int companionWidth,
	                        unsigned int companionHeight) const;
	bool pollEvent(Event* e);
	void close();
	~VRHandler();

  public slots:
	QSize getEyeRenderTargetSize() const;
	QMatrix4x4 getEyeViewMatrix(Side eye) const;
	QMatrix4x4 getProjectionMatrix(Side eye, float nearPlan = 0.1f,
	                               float farPlan = 100.0f) const;
	QMatrix4x4 getHMDPosMatrix() const { return hmdPosMatrix; };
	void resetPos();
	Side getCurrentRenderingEye() const { return currentRenderingEye; };
	float getFrameTiming() const;

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

	GLHandler::RenderTarget leftTarget;
	GLHandler::RenderTarget rightTarget;
	GLHandler::RenderTarget postProcessingTargets[2];

	Side currentRenderingEye = Side::LEFT;

	QMatrix4x4 hmdPosMatrix;

	void updateController(Side side, int nDevice);
	void updateHands();

	vr::Hmd_Eye getEye(Side eye) const
	{
		return (eye == Side::LEFT) ? vr::Eye_Left : vr::Eye_Right;
	};
	static QMatrix4x4 toQt(const vr::HmdMatrix34_t& matrix);
	static QMatrix4x4 toQt(const vr::HmdMatrix44_t& matrix);
	std::string sideToStr(Side side) const
	{
		return (side == Side::LEFT) ? "left" : "right";
	};

	static Button getButton(int openvrButton);
};

#endif // VRHANDLER_H
