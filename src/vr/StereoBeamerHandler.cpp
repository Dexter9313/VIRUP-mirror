#include "vr/StereoBeamerHandler.hpp"

#include "Renderer.hpp"

bool StereoBeamerHandler::init(Renderer const& renderer)
{
	VRHandler::init(renderer);
	reloadPostProcessingTargets();
	hmdPosMatrix = QMatrix4x4();
	enabled      = true;
	return true;
}

QSize StereoBeamerHandler::getEyeRenderTargetSize() const
{
	auto fullRTSize(renderer->getSize());
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

float StereoBeamerHandler::getRenderTargetAverageLuminance(Side eye) const
{
	return (eye == Side::LEFT ? postProcessingTargetsLeft[0]
	                          : postProcessingTargetsRight[0])
	    ->getColorAttachmentTexture()
	    .getAverageLuminance();
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
	                              ? *postProcessingTargetsLeft[0]
	                              : *postProcessingTargetsRight[0]);
	currentRenderingEye = eye;
}

void StereoBeamerHandler::renderControllers() const {}

void StereoBeamerHandler::renderHands() const {}

void StereoBeamerHandler::reloadPostProcessingTargets()
{
	delete postProcessingTargetsLeft[0];
	delete postProcessingTargetsRight[0];
	delete postProcessingTargetsLeft[1];
	delete postProcessingTargetsRight[1];

	currentTargetSize = getEyeRenderTargetSize();
	postProcessingTargetsLeft[0]
	    = new GLFramebufferObject(GLTexture::Tex2DProperties(
	        currentTargetSize.width(), currentTargetSize.height(), GL_RGBA32F));
	postProcessingTargetsLeft[1]
	    = new GLFramebufferObject(GLTexture::Tex2DProperties(
	        currentTargetSize.width(), currentTargetSize.height(), GL_RGBA32F));
	postProcessingTargetsRight[0]
	    = new GLFramebufferObject(GLTexture::Tex2DProperties(
	        currentTargetSize.width(), currentTargetSize.height(), GL_RGBA32F));
	postProcessingTargetsRight[1]
	    = new GLFramebufferObject(GLTexture::Tex2DProperties(
	        currentTargetSize.width(), currentTargetSize.height(), GL_RGBA32F));
}

void StereoBeamerHandler::submitRendering(Side /*eye*/, unsigned int /*i*/) {}

void StereoBeamerHandler::displayOnCompanion(unsigned int companionWidth,
                                             unsigned int companionHeight) const
{
	getPostProcessingTarget(submittedIndex, Side::LEFT)
	    .showOnScreen(0, 0, static_cast<int>(companionWidth / 2),
	                  static_cast<int>(companionHeight));
	getPostProcessingTarget(submittedIndex, Side::RIGHT)
	    .showOnScreen(static_cast<int>(companionWidth / 2), 0,
	                  static_cast<int>(companionWidth),
	                  static_cast<int>(companionHeight));
}

bool StereoBeamerHandler::pollEvent(Event* /*e*/)
{
	return false;
}

void StereoBeamerHandler::close()
{
	delete postProcessingTargetsLeft[0];
	delete postProcessingTargetsRight[0];
	delete postProcessingTargetsLeft[1];
	delete postProcessingTargetsRight[1];
	postProcessingTargetsLeft[0]  = nullptr;
	postProcessingTargetsLeft[1]  = nullptr;
	postProcessingTargetsRight[0] = nullptr;
	postProcessingTargetsRight[1] = nullptr;
	enabled                       = false;
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
