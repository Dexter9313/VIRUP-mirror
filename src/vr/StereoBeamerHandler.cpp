#include "vr/StereoBeamerHandler.hpp"

#include "Renderer.hpp"

bool StereoBeamerHandler::init(Renderer const& renderer)
{
	VRHandler::init(renderer);
	hmdPosMatrix = QMatrix4x4();
	enabled      = true;
	return true;
}

QSize StereoBeamerHandler::getEyeRenderTargetSize() const
{
	auto fullRTSize(renderer->getSize(true));
	return {fullRTSize.width() / 2, fullRTSize.height()};
}

float StereoBeamerHandler::getFrameTiming() const
{
	return -1.f;
}

const Controller* StereoBeamerHandler::getController(Side /*side*/) const
{
	return nullptr;
}

const Hand* StereoBeamerHandler::getHand(Side /*side*/) const
{
	return nullptr;
}

QMatrix4x4 StereoBeamerHandler::getSeatedToStandingAbsoluteTrackingPos() const
{
	return {};
}

QSizeF StereoBeamerHandler::getPlayAreaSize() const
{
	return {0.f, 0.f};
}

std::vector<QVector3D> StereoBeamerHandler::getPlayAreaQuad() const
{
	std::vector<QVector3D> result;
	result.emplace_back();
	result.emplace_back();
	result.emplace_back();
	result.emplace_back();

	return result;
}

void StereoBeamerHandler::prepareRendering(Side eye)
{
	currentRenderingEye = eye;
}

void StereoBeamerHandler::renderControllers() const {}

void StereoBeamerHandler::renderHands() const {}

void StereoBeamerHandler::submitRendering(GLFramebufferObject const& /*fbo*/) {}

bool StereoBeamerHandler::pollEvent(Event* /*e*/)
{
	return false;
}

void StereoBeamerHandler::close()
{
	enabled = false;
}

QMatrix4x4 StereoBeamerHandler::getEyeViewMatrix(Side eye) const
{
	QMatrix4x4 res;
	res.translate(
	    stereoMultiplier
	    * QVector3D(eye == Side::LEFT ? 0.03215 : -0.03215, 0.f, -0.015f));
	return res;
}

QMatrix4x4 StereoBeamerHandler::getProjectionMatrix(Side /*eye*/,
                                                    float nearPlan,
                                                    float farPlan) const
{
	QMatrix4x4 result;
	result.perspective(renderer->getVerticalFOV(),
	                   renderer->getAspectRatioFromFOV(), nearPlan, farPlan);
	return result;
}

void StereoBeamerHandler::resetPos() {}
