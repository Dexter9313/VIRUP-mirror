#ifndef BASELINEMETHODTEX_H
#define BASELINEMETHODTEX_H

#include "BaseLineMethod.hpp"

class BaseLineMethodTex : public BaseLineMethod
{
  public:
	BaseLineMethodTex();
	virtual std::string getName() const override { return "Base Line Texture"; };
	virtual void render(Camera const& camera) override;
	~BaseLineMethodTex();

  private:
	GLHandler::Texture tex;
};

#endif // BASELINEMETHOD_H
