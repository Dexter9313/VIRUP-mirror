#ifndef VRHANDLER_H
#define VRHANDLER_H

#include <QObject>
#include <QSize>

#include "../gl/GLHandler.hpp"

class Controller;
class Hand;
class Renderer;

/** @ingroup pycall
 *
 * Callable in Python as the "VRHandler" object when relevant (VR is enabled).
 */
class VRHandler : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString drivername READ getDriverName)
	Q_PROPERTY(QSize eyerendertargetsize READ getEyeRenderTargetSize)
	Q_PROPERTY(QMatrix4x4 hmdposmatrix READ getHMDPosMatrix)
	Q_PROPERTY(Side currentrenderingeye READ getCurrentRenderingEye)
	Q_PROPERTY(float frametiming READ getFrameTiming)
	Q_PROPERTY(double stereomultiplier READ getStereoMultiplier WRITE
	               setStereoMultiplier)

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
	VRHandler()                           = default;
	virtual QString getDriverName() const = 0;
	virtual bool isEnabled() const        = 0;
	virtual bool init(Renderer const& renderer)
	{
		this->renderer = &renderer;
		return true;
	};
	virtual QSize getEyeRenderTargetSize() const = 0;
	QMatrix4x4 getHMDPosMatrix() const { return hmdPosMatrix; };
	Side getCurrentRenderingEye() const { return currentRenderingEye; };
	/**
	 * @getter{stereomultiplier}
	 */
	double getStereoMultiplier() const { return stereoMultiplier; };
	/**
	 * @setter{stereomultiplier}
	 */
	void setStereoMultiplier(double sm)
	{
		QSettings().setValue("vr/stereomultiplier", sm);
		stereoMultiplier = sm;
	};
	virtual float getFrameTiming() const                     = 0;
	virtual const Controller* getController(Side side) const = 0;
	virtual const Hand* getHand(Side side) const             = 0;
	// getRenderTargetAverageLuminanceFactor ? for OpenVR 1.041f
	// virtual float getRenderTargetAverageLuminance(Side eye) const     = 0;
	virtual QMatrix4x4 getSeatedToStandingAbsoluteTrackingPos() const = 0;
	virtual QSizeF getPlayAreaSize() const                            = 0;
	virtual std::vector<QVector3D> getPlayAreaQuad() const            = 0;
	// update state
	virtual void prepareRendering(Side eye) = 0;
	virtual void renderHiddenAreaMesh(Side /*eye*/){};
	virtual void renderControllers() const                       = 0;
	virtual void renderHands() const                             = 0;
	virtual void submitRendering(GLFramebufferObject const& fbo) = 0;

	virtual bool pollEvent(Event* e) = 0;
	virtual void close()             = 0;
	virtual ~VRHandler(){};

	const bool forceLeft  = QSettings().value("vr/forceleft").toBool();
	const bool forceRight = QSettings().value("vr/forceright").toBool();

  public slots:
	virtual QMatrix4x4 getEyeViewMatrix(Side eye) const                    = 0;
	virtual QMatrix4x4 getProjectionMatrix(Side eye, float nearPlan = 0.1f,
	                                       float farPlan = 10000.0f) const = 0;
	virtual void resetPos()                                                = 0;

  signals:
	void renderTargetSizeChanged(QSize newSize);

  protected:
	Renderer const* renderer;

	double stereoMultiplier
	    = QSettings().value("vr/stereomultiplier").toDouble();

	QMatrix4x4 hmdPosMatrix;
	Side currentRenderingEye = Side::LEFT;
	QString sideToStr(Side side) const
	{
		return (side == Side::LEFT) ? "left" : "right";
	};
};

#endif // VRHANDLER_H
