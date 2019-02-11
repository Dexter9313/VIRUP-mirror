#ifndef MAINWIN_H
#define MAINWIN_H

#include <QCoreApplication>

#include "AbstractMainWin.hpp"

#include "methods/BaseLineMethod.hpp"
#include "methods/BaseLineMethodTex.hpp"
#include "methods/TreeMethodLOD.hpp"
#include "methods/TreeMethodTex.hpp"

class MainWin : public AbstractMainWin
{
	Q_OBJECT
  public:
	MainWin();
	~MainWin();

  protected:
	virtual void keyPressEvent(QKeyEvent* e) override;
	virtual void vrEvent(VRHandler::Event const& e) override;

	// declare drawn resources
	virtual void initScene() override;

	// update physics/controls/meshes, etc...
	// prepare for rendering
	virtual void updateScene(BasicCamera& camera) override;

	// render user scene on camera
	// (no controllers or hands)
	virtual void renderScene(BasicCamera const& camera) override;

	virtual void applyPostProcShaderParams(QString const& id, GLHandler::ShaderProgram shader) const override;

  private:
	static std::vector<float> generateVertices(unsigned int number, unsigned int seed);
	static GLHandler::Mesh createCube(GLHandler::ShaderProgram const& shader);
	static void deleteCube(GLHandler::Mesh mesh, GLHandler::ShaderProgram shader);

	GLHandler::Mesh cube;
	GLHandler::ShaderProgram cubeShader;
	Method* method;
	bool showCube = QSettings().value("misc/showcube").toBool();
};

#endif // MAINWIN_H
