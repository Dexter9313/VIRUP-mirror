#include "vr/VRHandler.hpp"

bool VRHandler::init()
{
	if(!vr::VR_IsRuntimeInstalled())
	{
		return false;
	}

// Start SteamVR if not running on unix-based
#ifdef Q_OS_UNIX
	// check if vrcompositor is running, and if not, attempt to run it
	QProcess pgrep;
	QString cmd("pgrep");
	QStringList args = QStringList() << "vrcompositor";
	pgrep.start(cmd, args);
	pgrep.waitForReadyRead();
	if(pgrep.readAllStandardOutput().isEmpty())
	{
		std::cout << "Starting SteamVR..." << std::endl;
		std::cout << "Runtime path : " << vr::VR_RuntimePath() << std::endl;
		QProcess vrstartup;
		cmd = QString(vr::VR_RuntimePath()) + "bin/vrstartup.sh";
		vrstartup.start(cmd);
		QThread::sleep(7);
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
	if((vr_compositor = vr::VRCompositor()) == nullptr)
	{
		std::cerr
		    << "Compositor initialization failed. See log file for details"
		    << std::endl;
		return false;
	}
	std::cout << "VR compositor initialized..." << std::endl;

	vr_render_models = static_cast<vr::IVRRenderModels*>(
	    vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &eError));
	if(vr_render_models == nullptr)
	{
		vr::VR_Shutdown();
		std::cerr << "Couldn't load generic render models" << std::endl;
	}
	else
	{
		std::cout << "Render models loaded successfully" << std::endl;
	}

	leftTarget  = GLHandler::newRenderTarget(getEyeRenderTargetSize().width(),
                                            getEyeRenderTargetSize().height(),
                                            GL_RGBA8);
	rightTarget = GLHandler::newRenderTarget(getEyeRenderTargetSize().width(),
	                                         getEyeRenderTargetSize().height(),
	                                         GL_RGBA8);
	postProcessingTargets[0] = GLHandler::newRenderTarget(
	    getEyeRenderTargetSize().width(), getEyeRenderTargetSize().height());
	postProcessingTargets[1] = GLHandler::newRenderTarget(
	    getEyeRenderTargetSize().width(), getEyeRenderTargetSize().height());

#ifdef LEAP_MOTION
	if(leapController.isConnected())
	{
		std::cout << "Leap controller connected !" << std::endl;
	}
	else
	{
		std::cout << "No Leap controller connected." << std::endl;
	}
#endif

	leftHand  = new Hand(Side::LEFT);
	rightHand = new Hand(Side::RIGHT);

	PythonQtHandler::addObject("leftHand", leftHand);
	PythonQtHandler::addObject("leftHand", rightHand);

	return true;
}

QSize VRHandler::getEyeRenderTargetSize() const
{
	unsigned int w, h;
	vr_pointer->GetRecommendedRenderTargetSize(&w, &h);
	return {static_cast<int>(w), static_cast<int>(h)};
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
			if(leftHand != nullptr)
			{
				return leftHand->isValid() ? leftHand : nullptr;
			}
			break;
		case Side::RIGHT:
			if(rightHand != nullptr)
			{
				return rightHand->isValid() ? rightHand : nullptr;
			}
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

//-----------------------------------------------------------------------------
// Purpose: Helper to get a string from a tracked device type class
//-----------------------------------------------------------------------------
std::string GetTrackedDeviceClassString(vr::ETrackedDeviceClass td_class)
{
	std::string str_td_class = "Unknown class";

	switch(td_class)
	{
		case vr::TrackedDeviceClass_Invalid: // = 0, the ID was not valid.
			str_td_class = "Invalid";
			break;
		case vr::TrackedDeviceClass_HMD: // = 1, Head-Mounted Displays
			str_td_class = "HMD";
			break;
		case vr::TrackedDeviceClass_Controller: // = 2, Tracked controllers
			str_td_class = "Controller";
			break;
		case vr::TrackedDeviceClass_GenericTracker: // = 3, Generic trackers,
		                                            // similar to controllers
			str_td_class = "Generic Tracker";
			break;
		case vr::TrackedDeviceClass_TrackingReference: // = 4, Camera and base
		                                               // stations that serve as
		                                               // tracking reference
		                                               // points
			str_td_class = "Tracking Reference";
			break;
		case vr::TrackedDeviceClass_DisplayRedirect: // = 5, Accessories that
		                                             // aren't necessarily
		                                             // tracked themselves, but
		                                             // may redirect video
		                                             // output from other
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
	    &tracked_device_pose[0], vr::k_unMaxTrackedDeviceCount, nullptr, 0);

	int nDeviceLeft  = -1;
	int nDeviceRight = -1;

	for(unsigned int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount;
	    nDevice++)
	{
		vr::ETrackedDeviceClass tracked_device_class
		    = vr_pointer->GetTrackedDeviceClass(nDevice);
		if(tracked_device_class == vr::TrackedDeviceClass_Invalid)
		{
			continue;
		}
		if(tracked_device_pose.at(nDevice).bPoseIsValid)
		{
			tracked_device_pose_matrix.at(nDevice) = toQt(
			    tracked_device_pose.at(nDevice).mDeviceToAbsoluteTracking);
		}
		vr::ETrackedControllerRole role
		    = vr_pointer->GetControllerRoleForTrackedDeviceIndex(nDevice);
		if(role == vr::TrackedControllerRole_LeftHand)
		{
			nDeviceLeft = nDevice;
		}
		if(role == vr::TrackedControllerRole_RightHand)
		{
			nDeviceRight = nDevice;
		}
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
	{
		GLHandler::beginRendering(postProcessingTargets[0]);
	}
	else
	{
		GLHandler::beginRendering(eye == Side::LEFT ? leftTarget : rightTarget);
	}
	currentRenderingEye = eye;
}

void VRHandler::renderControllers() const
{
	if(leftController != nullptr)
	{
		leftController->render();
	}
	if(rightController != nullptr)
	{
		rightController->render();
	}
}

void VRHandler::renderHands()
{
	if(leftHand->isValid())
	{
		leftHand->render();
	}
	if(rightHand->isValid())
	{
		rightHand->render();
	}
}

void VRHandler::reloadPostProcessingTargets()
{
	GLHandler::deleteRenderTarget(postProcessingTargets[0]);
	GLHandler::deleteRenderTarget(postProcessingTargets[1]);
	postProcessingTargets[0] = GLHandler::newRenderTarget(
	    getEyeRenderTargetSize().width(), getEyeRenderTargetSize().height());
	postProcessingTargets[1] = GLHandler::newRenderTarget(
	    getEyeRenderTargetSize().width(), getEyeRenderTargetSize().height());
}

void VRHandler::submitRendering(Side eye)
{
	GLHandler::RenderTarget* frame
	    = eye == Side::LEFT ? &leftTarget : &rightTarget;
	vr::Texture_t texture = {
	    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
	    reinterpret_cast<void*>(static_cast<uintptr_t>(GLHandler::getGLTexture(
	        GLHandler::getColorAttachmentTexture(*frame)))),
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
	GLHandler::showOnScreen(leftTarget, 0, 0,
	                        static_cast<int>(companionWidth / 2),
	                        static_cast<int>(companionHeight));
	GLHandler::showOnScreen(rightTarget, static_cast<int>(companionWidth / 2),
	                        0, static_cast<int>(companionWidth),
	                        static_cast<int>(companionHeight));
}

float VRHandler::getFrameTiming() const
{
	vr::Compositor_FrameTiming result = {};
	result.m_nSize                    = sizeof(vr::Compositor_FrameTiming);
	vr_compositor->GetFrameTiming(&result);

	// see https://developer.valvesoftware.com/wiki/SteamVR/Frame_Timing
	return result.m_flTotalRenderGpuMs + 11 * (result.m_nNumFramePresents - 1);
}

bool VRHandler::pollEvent(Event* e)
{
	vr::VREvent_t vrevent = {};

	e->type   = EventType::NONE;
	e->side   = Side::NONE;
	e->button = Button::NONE;
	if(vr_pointer->PollNextEvent(&vrevent, sizeof(vrevent)))
	{
		switch(vrevent.eventType)
		{
			case vr::VREvent_ButtonPress:
				e->type = EventType::BUTTON_PRESSED;
				if(leftController != nullptr)
				{
					if(leftController->nDevice == vrevent.trackedDeviceIndex)
					{
						e->side = Side::LEFT;
					}
				}
				if(rightController != nullptr)
				{
					if(rightController->nDevice == vrevent.trackedDeviceIndex)
					{
						e->side = Side::RIGHT;
					}
				}
				e->button = getButton(vrevent.data.controller.button);
				return true;
			case vr::VREvent_ButtonUnpress:
				e->type = EventType::BUTTON_UNPRESSED;
				if(leftController != nullptr)
				{
					if(leftController->nDevice == vrevent.trackedDeviceIndex)
					{
						e->side = Side::LEFT;
					}
				}
				if(rightController != nullptr)
				{
					if(rightController->nDevice == vrevent.trackedDeviceIndex)
					{
						e->side = Side::RIGHT;
					}
				}
				e->button = getButton(vrevent.data.controller.button);
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
	{
		return;
	}
	updateController(Side::LEFT, -1);
	updateController(Side::RIGHT, -1);
	delete leftHand;
	delete rightHand;
	PythonQtHandler::addObject("leftHand", nullptr);
	PythonQtHandler::addObject("leftHand", nullptr);
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
	{
		controller = &leftController;
	}
	else if(side == Side::RIGHT)
	{
		controller = &rightController;
	}
	else
	{
		return;
	}

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
		(*controller)->update(tracked_device_pose_matrix.at(nDevice));
	}
	PythonQtHandler::addObject(
	    ((side == Side::LEFT) ? QString("left") : QString("right"))
	        + "Controller",
	    *controller);
}

void VRHandler::updateHands()
{
	leftHand->invalidate();
	rightHand->invalidate();
#ifdef LEAP_MOTION
	if(!leapController.isConnected())
	{
		return;
	}

	for(Leap::Hand hand : leapController.frame().hands())
	{
		if(hand.isLeft())
		{
			leftHand->update(hand);
		}
		else
		{
			rightHand->update(hand);
		}
	}
#endif
}

QMatrix4x4 VRHandler::toQt(const vr::HmdMatrix34_t& matrix)
{
	return {matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[0][3],
	        matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3],
	        matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3],
	        0.0f,           0.0f,           0.0f,           1.0f};
}

QMatrix4x4 VRHandler::toQt(const vr::HmdMatrix44_t& matrix)
{
	return {matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[0][3],
	        matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3],
	        matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3],
	        matrix.m[3][0], matrix.m[3][1], matrix.m[3][2], matrix.m[3][3]};
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
		default:
			return Button::NONE;
	}
	return Button::NONE;
}
