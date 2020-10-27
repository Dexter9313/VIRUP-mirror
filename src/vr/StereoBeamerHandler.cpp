#include "vr/StereoBeamerHandler.hpp"

bool StereoBeamerHandler::init()
{
	reloadPostProcessingTargets();
	hmdPosMatrix = QMatrix4x4();
	enabled      = true;
	return true;
}

QSize StereoBeamerHandler::getEyeRenderTargetSize() const
{
	return {1000, 1000};
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

float StereoBeamerHandler::getRenderTargetAverageLuminance(Side eye) const
{
	auto tex = GLHandler::getColorAttachmentTexture(
	    eye == Side::LEFT ? postProcessingTargetsLeft[0]
	                      : postProcessingTargetsRight[0]);
	return GLHandler::getTextureAverageLuminance(tex);
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

void StereoBeamerHandler::prepareRendering()
{
	// reload render targets if resolution per eye changed (supersampling)
	if(currentTargetSize != getEyeRenderTargetSize())
	{
		reloadPostProcessingTargets();
	}
}

void StereoBeamerHandler::beginRendering(Side eye)
{
	GLHandler::beginRendering(eye == Side::LEFT
	                              ? postProcessingTargetsLeft[0]
	                              : postProcessingTargetsRight[0]);
	currentRenderingEye = eye;
}

void StereoBeamerHandler::renderControllers() const {}

void StereoBeamerHandler::renderHands() const {}

void StereoBeamerHandler::reloadPostProcessingTargets()
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
}

void StereoBeamerHandler::submitRendering(Side /*eye*/, unsigned int /*i*/) {}

void StereoBeamerHandler::displayOnCompanion(unsigned int companionWidth,
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

bool StereoBeamerHandler::pollEvent(Event* /*e*/)
{
	return false;
}

void StereoBeamerHandler::close()
{
	GLHandler::deleteRenderTarget(postProcessingTargetsLeft[0]);
	GLHandler::deleteRenderTarget(postProcessingTargetsRight[0]);
	GLHandler::deleteRenderTarget(postProcessingTargetsLeft[1]);
	GLHandler::deleteRenderTarget(postProcessingTargetsRight[1]);
	enabled = false;
}

QMatrix4x4 StereoBeamerHandler::getEyeViewMatrix(Side eye) const
{
	QMatrix4x4 res;
	res.translate(
	    QVector3D(eye == Side::LEFT ? 0.03215 : -0.03215, 0.f, -0.015f));
	return res;
}

QMatrix4x4 StereoBeamerHandler::getProjectionMatrix(Side /*eye*/,
                                                    float nearPlan,
                                                    float farPlan) const
{
	const float fovV(40.f), fovH(36.f);
	QMatrix4x4 result;
	result.perspective(fovV, fovH / fovV, nearPlan, farPlan);
	return result;
}

void StereoBeamerHandler::resetPos() {}
