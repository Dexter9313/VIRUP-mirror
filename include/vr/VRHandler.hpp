#ifndef VRHANDLER_H
#define VRHANDLER_H

#include <QObject>
#include <QSize>

#include "../gl/GLHandler.hpp"

class Controller;
class Hand;

/** @ingroup pycall
 *
 * Callable in Python as the "VRHandler" object when relevant (VR is enabled).
 */
class VRHandler : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QSize eyerendertargetsize READ getEyeRenderTargetSize)
	Q_PROPERTY(QMatrix4x4 hmdposmatrix READ getHMDPosMatrix)
	Q_PROPERTY(Side currentrenderingeye READ getCurrentRenderingEye)
	Q_PROPERTY(float frametiming READ getFrameTiming)

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
	VRHandler()                                  = default;
	virtual bool isEnabled() const               = 0;
	virtual bool init()                          = 0;
	virtual QSize getEyeRenderTargetSize() const = 0;
	QMatrix4x4 getHMDPosMatrix() const { return hmdPosMatrix; };
	Side getCurrentRenderingEye() const { return currentRenderingEye; };
	virtual float getFrameTiming() const                              = 0;
	virtual const Controller* getController(Side side) const          = 0;
	virtual const Hand* getHand(Side side) const                      = 0;
	virtual float getRenderTargetAverageLuminance(Side eye) const     = 0;
	virtual QMatrix4x4 getSeatedToStandingAbsoluteTrackingPos() const = 0;
	virtual QSizeF getPlayAreaSize() const                            = 0;
	virtual std::vector<QVector3D> getPlayAreaQuad() const            = 0;
	// update state
	virtual void prepareRendering()        = 0;
	virtual void beginRendering(Side eye)  = 0;
	virtual void renderControllers() const = 0;
	virtual void renderHands() const       = 0;
	virtual GLHandler::RenderTarget const&
	    getPostProcessingTarget(unsigned int i, Side side) const = 0;
	virtual void reloadPostProcessingTargets()                   = 0;
	virtual void submitRendering(Side eye, unsigned int i)       = 0;

	virtual void displayOnCompanion(unsigned int companionWidth,
	                                unsigned int companionHeight) const = 0;
	virtual bool pollEvent(Event* e)                                    = 0;
	virtual void close()                                                = 0;
	virtual ~VRHandler(){};

  public slots:
	virtual QMatrix4x4 getEyeViewMatrix(Side eye) const                  = 0;
	virtual QMatrix4x4 getProjectionMatrix(Side eye, float nearPlan = 0.1f,
	                                       float farPlan = 100.0f) const = 0;
	virtual void resetPos()                                              = 0;

  protected:
	QMatrix4x4 hmdPosMatrix;
	Side currentRenderingEye = Side::LEFT;
	QString sideToStr(Side side) const
	{
		return (side == Side::LEFT) ? "left" : "right";
	};
};

#endif // VRHANDLER_H
