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
	if(!forceLeft && !forceRight)
	{
		fullRTSize.setWidth(fullRTSize.width() / 2);
	}
	return fullRTSize;
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
	res.translate(stereoMultiplier
	              * QVector3D(eye == Side::LEFT ? 0.03215 : -0.03215, 0.f,
	                          0.f /*-0.015f*/));
	return res;
}

QMatrix4x4 StereoBeamerHandler::getProjectionMatrix(Side /*eye*/,
                                                    float nearPlan,
                                                    float farPlan) const
{
	QVector3D deltaRel(QSettings()
	                       .value("vr/virtualcamshift")
	                       .value<QVector3D>()); // move cam in height units
	float vFOV(renderer->getVerticalFOV() * 3.1415 / 180.0),
	    a(renderer->getAspectRatioFromFOV());
	float l(-nearPlan * a * tan(vFOV / 2.0)), t(nearPlan * tan(vFOV / 2.0));
	float r(-l), b(-t);

	float height(t - b);

	r -= deltaRel.x() * height;
	l -= deltaRel.x() * height;
	t -= deltaRel.y() * height;
	b -= deltaRel.y() * height;

	float n(nearPlan + deltaRel.z() * height);
	r *= nearPlan / n;
	l *= nearPlan / n;
	t *= nearPlan / n;
	b *= nearPlan / n;

	QMatrix4x4 result;
	result.frustum(l, r, b, t, nearPlan, farPlan);
	return result;
}

void StereoBeamerHandler::resetPos() {}
