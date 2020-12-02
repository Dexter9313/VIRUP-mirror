#include "vr/OpenVRHandler.hpp"

bool OpenVRHandler::init(Renderer const& renderer)
{
	if(!vr::VR_IsRuntimeInstalled())
	{
		return false;
	}
	VRHandler::init(renderer);

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
		char rtPath[1024];
		uint32_t unRequiredSize;
		if(vr::VR_GetRuntimePath(rtPath, sizeof(rtPath), &unRequiredSize)
		   && unRequiredSize < sizeof(rtPath))
		{
			qDebug() << "Starting SteamVR...";
			qDebug() << QString("Runtime path : ") + rtPath;
			QProcess vrstartup;
			cmd = QString(rtPath);
			if(cmd.at(cmd.length() - 1) != '/')
			{
				cmd += '/';
			}
			cmd += "bin/vrstartup.sh";
			vrstartup.start(cmd);
			QThread::sleep(7);
		}
	}
#endif

	vr::EVRInitError eError = vr::VRInitError_None;

	vr_pointer = VR_Init(&eError, vr::VRApplication_Scene);
	// If the init failed because of an error
	if(eError != vr::VRInitError_None)
	{
		vr_pointer = nullptr;
		qCritical() << "Unable to init VR runtime: "
		            << VR_GetVRInitErrorAsEnglishDescription(eError);
		return false;
	}
	qDebug() << "VR runtime initialized...";
	if((vr_compositor = vr::VRCompositor()) == nullptr)
	{
		qCritical()
		    << "Compositor initialization failed. See log file for details";
		return false;
	}
	qDebug() << "VR compositor initialized...";

	vr_render_models = static_cast<vr::IVRRenderModels*>(
	    vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &eError));
	if(vr_render_models == nullptr)
	{
		vr::VR_Shutdown();
		qCritical() << "Couldn't load generic render models";
	}
	else
	{
		qDebug() << "Render models loaded successfully";
	}

	reloadPostProcessingTargets();

#ifdef LEAP_MOTION
	if(leapController.isConnected())
	{
		qDebug() << "Leap controller connected !";
	}
	else
	{
		qDebug() << "No Leap controller connected.";
	}
#endif

	leftHand  = new Hand(Side::LEFT);
	rightHand = new Hand(Side::RIGHT);

	PythonQtHandler::addObject("leftHand", leftHand);
	PythonQtHandler::addObject("leftHand", rightHand);

	return true;
}

QSize OpenVRHandler::getEyeRenderTargetSize() const
{
	unsigned int w, h;
	vr_pointer->GetRecommendedRenderTargetSize(&w, &h);
	return {static_cast<int>(w), static_cast<int>(h)};
}

float OpenVRHandler::getFrameTiming() const
{
	vr::Compositor_FrameTiming result = {};
	result.m_nSize                    = sizeof(vr::Compositor_FrameTiming);
	vr_compositor->GetFrameTiming(&result);

	// see https://developer.valvesoftware.com/wiki/SteamVR/Frame_Timing
	return result.m_flTotalRenderGpuMs + 11 * (result.m_nNumFramePresents - 1);
}

const Controller* OpenVRHandler::getController(Side side) const
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

const Hand* OpenVRHandler::getHand(Side side) const
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

float OpenVRHandler::getRenderTargetAverageLuminance(Side eye) const
{
	auto const& tex = GLHandler::getColorAttachmentTexture(
	    eye == Side::LEFT ? postProcessingTargetsLeft[0]
	                      : postProcessingTargetsRight[0]);
	return tex.getAverageLuminance()
	       * 1.041f; // compensate for hidden area mesh
}

QMatrix4x4 OpenVRHandler::getSeatedToStandingAbsoluteTrackingPos() const
{
	return toQt(vr_pointer->GetSeatedZeroPoseToStandingAbsoluteTrackingPose());
}

QSizeF OpenVRHandler::getPlayAreaSize() const
{
	float width(0.f), height(0.f);
	vr::VRChaperone()->GetPlayAreaSize(&width, &height);
	return {width, height};
}

std::vector<QVector3D> OpenVRHandler::getPlayAreaQuad() const
{
	vr::HmdQuad_t rect = {};
	vr::VRChaperone()->GetPlayAreaRect(&rect);

	std::vector<QVector3D> result;
	result.push_back(toQt(rect.vCorners[0]));
	result.push_back(toQt(rect.vCorners[1]));
	result.push_back(toQt(rect.vCorners[2]));
	result.push_back(toQt(rect.vCorners[3]));

	return result;
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
void OpenVRHandler::prepareRendering()
{
	vr::EVRCompositorError error = vr::VRCompositor()->WaitGetPoses(
	    &tracked_device_pose[0], vr::k_unMaxTrackedDeviceCount, nullptr, 0);

	// reload render targets if resolution per eye changed (supersampling)
	if(currentTargetSize != getEyeRenderTargetSize())
	{
		reloadPostProcessingTargets();
	}

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
		qCritical() << QString("ERROR in prepare: ") + error;
	}
}

void OpenVRHandler::beginRendering(Side eye)
{
	GLHandler::beginRendering(eye == Side::LEFT
	                              ? postProcessingTargetsLeft[0]
	                              : postProcessingTargetsRight[0]);
	currentRenderingEye = eye;
}

void OpenVRHandler::renderControllers() const
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

void OpenVRHandler::renderHands() const
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

void OpenVRHandler::reloadPostProcessingTargets()
{
	GLHandler::deleteRenderTarget(postProcessingTargetsLeft[0]);
	GLHandler::deleteRenderTarget(postProcessingTargetsRight[0]);
	GLHandler::deleteRenderTarget(postProcessingTargetsLeft[1]);
	GLHandler::deleteRenderTarget(postProcessingTargetsRight[1]);

	currentTargetSize            = getEyeRenderTargetSize();
	postProcessingTargetsLeft[0] = GLHandler::newRenderTarget(
	    currentTargetSize.width(), currentTargetSize.height());
	postProcessingTargetsLeft[1] = GLHandler::newRenderTarget(
	    currentTargetSize.width(), currentTargetSize.height());
	postProcessingTargetsRight[0] = GLHandler::newRenderTarget(
	    currentTargetSize.width(), currentTargetSize.height());
	postProcessingTargetsRight[1] = GLHandler::newRenderTarget(
	    currentTargetSize.width(), currentTargetSize.height());

	// Render hidden area mesh

	GLHandler::setBackfaceCulling(false);
	GLShaderProgram s("hiddenarea");

	GLHandler::glf().glClearStencil(0x0);
	GLHandler::glf().glEnable(GL_STENCIL_TEST);
	GLHandler::glf().glStencilMask(0xFF);
	GLHandler::glf().glStencilFunc(GL_ALWAYS, 1, 0xFF);
	GLHandler::glf().glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// LEFT
	auto hiddenAreaMesh = new GLMesh;
	hiddenAreaMesh->setVertices(
	    &(vr_pointer->GetHiddenAreaMesh(vr::Eye_Left).pVertexData[0].v[0]),
	    2 * 3 * vr_pointer->GetHiddenAreaMesh(vr::Eye_Left).unTriangleCount, s,
	    {{"position", 2}});

	GLHandler::beginRendering(static_cast<GLuint>(GL_COLOR_BUFFER_BIT)
	                              | static_cast<GLuint>(GL_DEPTH_BUFFER_BIT)
	                              | static_cast<GLuint>(GL_STENCIL_BUFFER_BIT),
	                          postProcessingTargetsLeft[0]);
	s.use();
	hiddenAreaMesh->render(PrimitiveType::TRIANGLES);
	delete hiddenAreaMesh;

	// RIGHT
	hiddenAreaMesh = new GLMesh;
	hiddenAreaMesh->setVertices(
	    &(vr_pointer->GetHiddenAreaMesh(vr::Eye_Right).pVertexData[0].v[0]),
	    2 * 3 * vr_pointer->GetHiddenAreaMesh(vr::Eye_Right).unTriangleCount, s,
	    {{"position", 2}});

	GLHandler::beginRendering(static_cast<GLuint>(GL_COLOR_BUFFER_BIT)
	                              | static_cast<GLuint>(GL_DEPTH_BUFFER_BIT)
	                              | static_cast<GLuint>(GL_STENCIL_BUFFER_BIT),
	                          postProcessingTargetsRight[0]);
	s.use();
	hiddenAreaMesh->render(PrimitiveType::TRIANGLES);
	delete hiddenAreaMesh;

	GLHandler::setBackfaceCulling(true);

	GLHandler::glf().glStencilMask(0x00);
	GLHandler::glf().glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	GLHandler::glf().glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void OpenVRHandler::submitRendering(Side eye, unsigned int i)
{
	submittedIndex = i % 2;
	GLHandler::RenderTarget const& frame
	    = getPostProcessingTarget(submittedIndex, eye);
	vr::Texture_t texture
	    = {// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
	       reinterpret_cast<void*>(static_cast<uintptr_t>(
	           GLHandler::getColorAttachmentTexture(frame).getGLTexture())),
	       vr::TextureType_OpenGL, vr::ColorSpace_Gamma};
	vr::EVRCompositorError error = vr_compositor->Submit(getEye(eye), &texture);
	if(error != vr::VRCompositorError_None)
	{
		qCritical() << QString("ERROR in submit: ") + error;
	}
}

void OpenVRHandler::displayOnCompanion(unsigned int companionWidth,
                                       unsigned int companionHeight) const
{
	GLHandler::showOnScreen(getPostProcessingTarget(submittedIndex, Side::LEFT),
	                        0, 0, static_cast<int>(companionWidth / 2),
	                        static_cast<int>(companionHeight));
	GLHandler::showOnScreen(
	    getPostProcessingTarget(submittedIndex, Side::RIGHT),
	    static_cast<int>(companionWidth / 2), 0,
	    static_cast<int>(companionWidth), static_cast<int>(companionHeight));
}

bool OpenVRHandler::pollEvent(Event* e)
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
					if(leftController->getNDevice()
					   == vrevent.trackedDeviceIndex)
					{
						e->side = Side::LEFT;
					}
				}
				if(rightController != nullptr)
				{
					if(rightController->getNDevice()
					   == vrevent.trackedDeviceIndex)
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
					if(leftController->getNDevice()
					   == vrevent.trackedDeviceIndex)
					{
						e->side = Side::LEFT;
					}
				}
				if(rightController != nullptr)
				{
					if(rightController->getNDevice()
					   == vrevent.trackedDeviceIndex)
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

void OpenVRHandler::close()
{
	if(vr_pointer == nullptr)
	{
		return;
	}
	GLHandler::glf().glDisable(GL_STENCIL_TEST);
	updateController(Side::LEFT, -1);
	updateController(Side::RIGHT, -1);
	delete leftHand;
	delete rightHand;
	PythonQtHandler::addObject("leftHand", nullptr);
	PythonQtHandler::addObject("leftHand", nullptr);
	GLHandler::deleteRenderTarget(postProcessingTargetsLeft[0]);
	GLHandler::deleteRenderTarget(postProcessingTargetsRight[0]);
	GLHandler::deleteRenderTarget(postProcessingTargetsLeft[1]);
	GLHandler::deleteRenderTarget(postProcessingTargetsRight[1]);
	qDebug() << "Closing VR runtime...";
	vr::VR_Shutdown();
	vr_pointer = nullptr;
}

QMatrix4x4 OpenVRHandler::getEyeViewMatrix(Side eye) const
{
	return toQt(vr_pointer->GetEyeToHeadTransform(getEye(eye))).inverted();
}

QMatrix4x4 OpenVRHandler::getProjectionMatrix(Side eye, float nearPlan,
                                              float farPlan) const
{
	return toQt(
	    vr_pointer->GetProjectionMatrix(getEye(eye), nearPlan, farPlan));
}

void OpenVRHandler::resetPos()
{
	vr::VRChaperone()->ResetZeroPose(
	    vr::ETrackingUniverseOrigin::TrackingUniverseSeated);
	vr_compositor->SetTrackingSpace(
	    vr::ETrackingUniverseOrigin::TrackingUniverseSeated);
}

void OpenVRHandler::updateController(Side side, int nDevice)
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
		qDebug() << QString("Disconnecting ") + sideToStr(side)
		                + " controller...";
		delete *controller;
		*controller = nullptr;
	}
	else if(*controller == nullptr && nDevice != -1)
	{
		qDebug() << QString("Connecting ") + sideToStr(side) + " controller...";
		*controller = new Controller(vr_pointer, nDevice, side);
	}
	else if(*controller != nullptr)
	{
		(*controller)->update(tracked_device_pose_matrix.at(nDevice), nDevice);
	}
	PythonQtHandler::addObject(
	    ((side == Side::LEFT) ? QString("left") : QString("right"))
	        + "Controller",
	    *controller);
}

void OpenVRHandler::updateHands()
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

QVector3D OpenVRHandler::toQt(const vr::HmdVector3_t& vector)
{
	return {vector.v[0], vector.v[1], vector.v[2]};
}

QMatrix4x4 OpenVRHandler::toQt(const vr::HmdMatrix34_t& matrix)
{
	return {matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[0][3],
	        matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3],
	        matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3],
	        0.0f,           0.0f,           0.0f,           1.0f};
}

QMatrix4x4 OpenVRHandler::toQt(const vr::HmdMatrix44_t& matrix)
{
	return {matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[0][3],
	        matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3],
	        matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3],
	        matrix.m[3][0], matrix.m[3][1], matrix.m[3][2], matrix.m[3][3]};
}

OpenVRHandler::Button OpenVRHandler::getButton(int openvrButton)
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
