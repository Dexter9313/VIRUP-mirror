#ifndef BASELINEMETHODTEX_H
#define BASELINEMETHODTEX_H

#include "BaseLineMethod.hpp"

class BaseLineMethodTex : public BaseLineMethod
{
	Q_OBJECT
  public:
	BaseLineMethodTex();
	virtual void render(Camera const& camera, double scale,
	                    std::array<double, 3> const& translation) override;
	~BaseLineMethodTex();

  public slots:
	virtual std::string getName() const override
	{
		return "Base Line Texture";
	};

  private:
	GLHandler::Texture tex;
};

#endif // BASELINEMETHOD_H
