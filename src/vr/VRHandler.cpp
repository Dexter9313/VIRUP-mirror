#include "vr/VRHandler.hpp"

bool VRHandler::init()
{
	if(!vr::VR_IsRuntimeInstalled())
		return false;

// Start SteamVR if not running on unix-based
#ifdef Q_OS_UNIX
	if(system("pgrep vrcompositor > /dev/null 2>&1") != 0)
	{
		std::cout << "Starting SteamVR..." << std::endl;
		std::cout << "Runtime path : " << vr::VR_RuntimePath() << std::endl;
		system((std::string("cd ") + vr::VR_RuntimePath() + "/bin && "
		        + "./vrstartup.sh > /dev/null 2>&1 &")
		           .c_str());
		system("sleep 7");
	}
#endif

	vr::EVRInitError eError = vr::VRInitError_None;

	vr_pointer = VR_Init(&eError, vr::VRApplication_Scene);
	// If the init failed because of an error
	if(eError != vr::VRInitError_None)
	{
		vr_pointer = nullptr;
		std::cerr << "Unable to init VR runtime: "
		          << VR_GetVRInitErrorAsEnglishDescription(eError) << std::endl;
		return false;
	}
	std::cout << "VR runtime initialized..." << std::endl;
	if(!(vr_compositor = vr::VRCompositor()))
	{
		std::cerr
		    << "Compositor initialization failed. See log file for details"
		    << std::endl;
		return false;
	}
	std::cout << "VR compositor initialized..." << std::endl;

	vr_render_models = (vr::IVRRenderModels*) vr::VR_GetGenericInterface(
	    vr::IVRRenderModels_Version, &eError);
	if(!vr_render_models)
	{
		vr::VR_Shutdown();
		std::cerr << "Couldn't load generic render models" << std::endl;
	}
	else
		std::cout << "Render models loaded successfully" << std::endl;

	leftTarget
	    = GLHandler::newRenderTarget(getEyeDims().first, getEyeDims().second);
	rightTarget
	    = GLHandler::newRenderTarget(getEyeDims().first, getEyeDims().second);
	postProcessingTargets[0]
	    = GLHandler::newRenderTarget(getEyeDims().first, getEyeDims().second);
	postProcessingTargets[1]
	    = GLHandler::newRenderTarget(getEyeDims().first, getEyeDims().second);

#ifdef LEAP_MOTION
	if(leapController.isConnected())
		std::cout << "Leap controller connected !" << std::endl;
	else
		std::cout << "No Leap controller connected." << std::endl;
#endif

	leftHand  = new Hand(Side::LEFT);
	rightHand = new Hand(Side::RIGHT);

	return true;
}

std::pair<unsigned int, unsigned int> VRHandler::getEyeDims() const
{
	std::pair<unsigned int, unsigned int> result;
	vr_pointer->GetRecommendedRenderTargetSize(&result.first, &result.second);
	return result;
}

QMatrix4x4 VRHandler::getEyeViewMatrix(Side eye) const
{
	return toQt(vr_pointer->GetEyeToHeadTransform(getEye(eye))).inverted();
}

QMatrix4x4 VRHandler::getProjectionMatrix(Side eye, float nearPlan,
                                          float farPlan) const
{
	return toQt(
	    vr_pointer->GetProjectionMatrix(getEye(eye), nearPlan, farPlan));
}

const Controller* VRHandler::getController(Side side) const
{
	switch(side)
	{
		case Side::LEFT:
			return leftController;
		case Side::RIGHT:
			return rightController;
		default:
			return nullptr;
	}
}

const Hand* VRHandler::getHand(Side side) const
{
	switch(side)
	{
		case Side::LEFT:
			if(leftHand)
				return leftHand->isValid() ? leftHand : nullptr;
			break;
		case Side::RIGHT:
			if(rightHand)
				return rightHand->isValid() ? rightHand : nullptr;
			break;
		default:
			return nullptr;
	}
	return nullptr;
}

void VRHandler::resetPos()
{
	vr_pointer->ResetSeatedZeroPose();
	vr_compositor->SetTrackingSpace(
	    vr::ETrackingUniverseOrigin::TrackingUniverseSeated);
}

using namespace std;
using namespace vr;

//-----------------------------------------------------------------------------
// Purpose: Helper to get a string from a tracked device type class
//-----------------------------------------------------------------------------
string GetTrackedDeviceClassString(vr::ETrackedDeviceClass td_class)
{
	string str_td_class = "Unknown class";

	switch(td_class)
	{
		case TrackedDeviceClass_Invalid: // = 0, the ID was not valid.
			str_td_class = "Invalid";
			break;
		case TrackedDeviceClass_HMD: // = 1, Head-Mounted Displays
			str_td_class = "HMD";
			break;
		case TrackedDeviceClass_Controller: // = 2, Tracked controllers
			str_td_class = "Controller";
			break;
		case TrackedDeviceClass_GenericTracker: // = 3, Generic trackers,
			                                    // similar to controllers
			str_td_class = "Generic Tracker";
			break;
		case TrackedDeviceClass_TrackingReference: // = 4, Camera and base
			                                       // stations that serve as
			                                       // tracking reference points
			str_td_class = "Tracking Reference";
			break;
		case TrackedDeviceClass_DisplayRedirect: // = 5, Accessories that aren't
			                                     // necessarily tracked
			                                     // themselves, but may redirect
			                                     // video output from other
			                                     // tracked devices
			str_td_class = "Display Redirecd";
			break;
		default:
			break;
	}

	return str_td_class;
}
void VRHandler::prepareRendering()
{
	vr::EVRCompositorError error = vr::VRCompositor()->WaitGetPoses(
	    tracked_device_pose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	int nDeviceLeft  = -1;
	int nDeviceRight = -1;

	for(unsigned int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount;
	    nDevice++)
	{
		vr::ETrackedDeviceClass tracked_device_class
		    = vr_pointer->GetTrackedDeviceClass(nDevice);
		if(tracked_device_class == TrackedDeviceClass_Invalid)
			continue;
		if(tracked_device_pose[nDevice].bPoseIsValid)
		{
			tracked_device_pose_matrix[nDevice]
			    = toQt(tracked_device_pose[nDevice].mDeviceToAbsoluteTracking);
		}
		vr::ETrackedControllerRole role
		    = vr_pointer->GetControllerRoleForTrackedDeviceIndex(nDevice);
		if(role == vr::TrackedControllerRole_LeftHand)
			nDeviceLeft = nDevice;
		if(role == vr::TrackedControllerRole_RightHand)
			nDeviceRight = nDevice;
	}

	if(tracked_device_pose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
	{
		hmdPosMatrix
		    = tracked_device_pose_matrix[vr::k_unTrackedDeviceIndex_Hmd];
	}

	updateController(Side::LEFT, nDeviceLeft);
	updateController(Side::RIGHT, nDeviceRight);
	updateHands();

	if(error != vr::VRCompositorError_None)
	{
		std::cerr << "ERROR in prepare: " << error << std::endl;
	}
}

void VRHandler::beginRendering(Side eye, bool postProcessed)
{
	if(postProcessed)
		GLHandler::beginRendering(postProcessingTargets[0]);
	else
		GLHandler::beginRendering(eye == Side::LEFT ? leftTarget : rightTarget);
	currentRenderingEye = eye;
}

void VRHandler::renderControllers() const
{
	if(leftController)
		leftController->render();
	if(rightController)
		rightController->render();
}

void VRHandler::renderHands()
{
	if(leftHand->isValid())
		leftHand->render();
	if(rightHand->isValid())
		rightHand->render();
}

void VRHandler::reloadPostProcessingTargets()
{
	GLHandler::deleteRenderTarget(postProcessingTargets[0]);
	GLHandler::deleteRenderTarget(postProcessingTargets[1]);
	postProcessingTargets[0]
	    = GLHandler::newRenderTarget(getEyeDims().first, getEyeDims().second);
	postProcessingTargets[1]
	    = GLHandler::newRenderTarget(getEyeDims().first, getEyeDims().second);
}

void VRHandler::submitRendering(Side eye)
{
	GLHandler::RenderTarget* frame
	    = eye == Side::LEFT ? &leftTarget : &rightTarget;
	vr::Texture_t texture = {(void*) (uintptr_t) frame->texColorBuffer,
	                         vr::TextureType_OpenGL, vr::ColorSpace_Gamma};
	vr::EVRCompositorError error = vr_compositor->Submit(getEye(eye), &texture);
	if(error != vr::VRCompositorError_None)
	{
		std::cerr << "ERROR in submit: " << error << std::endl;
	}
}

void VRHandler::displayOnCompanion(unsigned int companionWidth,
                                   unsigned int companionHeight) const
{
	GLHandler::showOnScreen(
	    leftTarget, {0, 0, (int) companionWidth / 2, (int) companionHeight});
	GLHandler::showOnScreen(rightTarget,
	                        {(int) companionWidth / 2, 0, (int) companionWidth,
	                         (int) companionHeight});
}

float VRHandler::getFrameTiming() const
{
	vr::Compositor_FrameTiming result;
	result.m_nSize = sizeof(vr::Compositor_FrameTiming);
	vr_compositor->GetFrameTiming(&result);

	// see https://developer.valvesoftware.com/wiki/SteamVR/Frame_Timing
	return result.m_flTotalRenderGpuMs + 11 * (result.m_nNumFramePresents - 1);
}

bool VRHandler::pollEvent(Event* e)
{
	vr::VREvent_t vrevent;

	e->type   = EventType::NONE;
	e->side   = Side::NONE;
	e->button = Button::NONE;
	if(vr_pointer->PollNextEvent(&vrevent, sizeof(vrevent)))
	{
		switch(vrevent.eventType)
		{
			case VREvent_ButtonPress:
				e->type = EventType::BUTTON_PRESSED;
				if(leftController)
					if(leftController->nDevice == vrevent.trackedDeviceIndex)
						e->side = Side::LEFT;
				if(rightController)
					if(rightController->nDevice == vrevent.trackedDeviceIndex)
						e->side = Side::RIGHT;
				e->button       = getButton(vrevent.data.controller.button);
				return true;
			case VREvent_ButtonUnpress:
				e->type = EventType::BUTTON_UNPRESSED;
				if(leftController)
					if(leftController->nDevice == vrevent.trackedDeviceIndex)
						e->side = Side::LEFT;
				if(rightController)
					if(rightController->nDevice == vrevent.trackedDeviceIndex)
						e->side = Side::RIGHT;
				e->button       = getButton(vrevent.data.controller.button);
				return true;
			default:
				return false;
		}
	}
	return false;
}

void VRHandler::close()
{
	if(vr_pointer == nullptr)
		return;
	updateController(Side::LEFT, -1);
	updateController(Side::RIGHT, -1);
	delete leftHand;
	delete rightHand;
	GLHandler::deleteRenderTarget(leftTarget);
	GLHandler::deleteRenderTarget(rightTarget);
	GLHandler::deleteRenderTarget(postProcessingTargets[0]);
	GLHandler::deleteRenderTarget(postProcessingTargets[1]);
	std::cout << "Closing VR runtime..." << std::endl;
	vr::VR_Shutdown();
	vr_pointer = nullptr;
}

VRHandler::~VRHandler()
{
	close();
}

void VRHandler::updateController(Side side, int nDevice)
{
	Controller** controller;
	if(side == Side::LEFT)
		controller = &leftController;
	else if(side == Side::RIGHT)
		controller = &rightController;
	else
		return;

	if(*controller != nullptr && nDevice == -1)
	{
		std::cout << "Disconnecting " << sideToStr(side) << " controller..."
		          << std::endl;
		delete *controller;
		*controller = nullptr;
	}
	else if(*controller == nullptr && nDevice != -1)
	{
		std::cout << "Connecting " << sideToStr(side) << " controller..."
		          << std::endl;
		*controller = new Controller(vr_pointer, nDevice, side);
	}
	else if(*controller != nullptr)
	{
		(*controller)->update(tracked_device_pose_matrix[nDevice]);
	}
}

void VRHandler::updateHands()
{
	leftHand->invalidate();
	rightHand->invalidate();
#ifdef LEAP_MOTION
	if(!leapController.isConnected())
		return;

	for(Leap::Hand hand : leapController.frame().hands())
	{
		if(hand.isLeft())
			leftHand->update(hand);
		else
			rightHand->update(hand);
	}
#endif
}

QMatrix4x4 VRHandler::toQt(const vr::HmdMatrix34_t& matrix)
{
	return QMatrix4x4(matrix.m[0][0], matrix.m[0][1], matrix.m[0][2],
	                  matrix.m[0][3], matrix.m[1][0], matrix.m[1][1],
	                  matrix.m[1][2], matrix.m[1][3], matrix.m[2][0],
	                  matrix.m[2][1], matrix.m[2][2], matrix.m[2][3], 0.0f,
	                  0.0f, 0.0f, 1.0f);
}

QMatrix4x4 VRHandler::toQt(const vr::HmdMatrix44_t& matrix)
{
	return QMatrix4x4(
	    matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[0][3],
	    matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3],
	    matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3],
	    matrix.m[3][0], matrix.m[3][1], matrix.m[3][2], matrix.m[3][3]);
}

VRHandler::Button VRHandler::getButton(int openvrButton)
{
	switch(openvrButton)
	{
		case vr::k_EButton_Grip:
			return Button::GRIP;
		case vr::k_EButton_SteamVR_Trigger:
			return Button::TRIGGER;
		case vr::k_EButton_SteamVR_Touchpad:
			return Button::TOUCHPAD;
		case vr::k_EButton_ApplicationMenu:
			return Button::MENU;
		case vr::k_EButton_System:
			return Button::SYSTEM;
	}
	return Button::NONE;
}
