#ifndef TREEMETHODTEX_H
#define TREEMETHODTEX_H

#include "TreeMethodLOD.hpp"

class TreeMethodTex : public TreeMethodLOD
{
	Q_OBJECT
  public:
	TreeMethodTex();
	virtual std::string getName() const override { return "Tree Texture"; };
	virtual void render(Camera const& camera, QMatrix4x4 const& model) override;
	virtual ~TreeMethodTex();

  private:
	GLHandler::Texture tex;
};

#endif // TREEMETHOD_H
