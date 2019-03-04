#include "methods/BaseLineMethod.hpp"
BaseLineMethod::BaseLineMethod()
    : BaseLineMethod("invsq")
{
}

BaseLineMethod::BaseLineMethod(std::string const& shadersCommonName)
    : BaseLineMethod(shadersCommonName, shadersCommonName)
{
}

BaseLineMethod::BaseLineMethod(std::string const& vertexShaderPath,
                               std::string const& fragmentShaderPath)
    : Method(vertexShaderPath, fragmentShaderPath)
    , mesh(GLHandler::newMesh())
    , pointSize(1)
{
	GLHandler::setShaderParam(shaderProgram, "color",
	                          QVector3D(1.0f, 1.0f, 1.0f));
}

void BaseLineMethod::init(std::vector<float> const& gazVertices,
                          std::vector<float> const& starsVertices,
                          std::vector<float> const& darkMatterVertices)
{
	std::vector<float> vertices(gazVertices);
	vertices.insert(vertices.end(), starsVertices.begin(), starsVertices.end());
	vertices.insert(vertices.end(), darkMatterVertices.begin(),
	                darkMatterVertices.end());
	GLHandler::setVertices(mesh, vertices, shaderProgram, {{"position", 3}});
	std::cout << "VRAM Loaded with " << vertices.size() * sizeof(vertices[0])
	          << " bytes worth of data" << std::endl;
}

void BaseLineMethod::init(std::string const& gazPath,
                          std::string const& starsPath,
                          std::string const& darkMatterPath)
{
	std::vector<float> vertices;
	if(gazPath != "")
	{
		std::ifstream file;
		file.open(gazPath, std::fstream::in | std::fstream::binary);
		Octree tree;
		file.seekg(0);
		tree.init(file);
		tree.readData(file);
		std::vector<float> tData(tree.getAllData());
		vertices.insert(vertices.end(), tData.begin(), tData.end());
		file.close();
	}
	if(starsPath != "")
	{
		std::ifstream file;
		file.open(starsPath, std::fstream::in | std::fstream::binary);
		Octree tree;
		file.seekg(0);
		tree.init(file);
		tree.readData(file);
		std::vector<float> tData(tree.getAllData());
		vertices.insert(vertices.end(), tData.begin(), tData.end());
		file.close();
	}
	if(darkMatterPath != "")
	{
		std::ifstream file;
		file.open(darkMatterPath, std::fstream::in | std::fstream::binary);
		Octree tree;
		file.seekg(0);
		tree.init(file);
		tree.readData(file);
		std::vector<float> tData(tree.getAllData());
		vertices.insert(vertices.end(), tData.begin(), tData.end());
		file.close();
	}
	GLHandler::setVertices(mesh, vertices, shaderProgram, {{"position", 3}});
	std::cout << "VRAM Loaded with " << vertices.size() * sizeof(vertices[0])
	          << " bytes worth of data" << std::endl;
}

void BaseLineMethod::render(Camera const& camera, QMatrix4x4 const& model)
{
	GLHandler::setShaderParam(shaderProgram, "alpha", model(0, 0) * getAlpha());
	GLHandler::setShaderParam(shaderProgram, "view",
	                          camera.hmdScaledSpaceToWorldTransform().inverted()
	                              * model);
	GLHandler::setPointSize(pointSize);
	GLHandler::beginTransparent();
	GLHandler::setUpRender(shaderProgram, model);
	GLHandler::render(mesh);
	GLHandler::endTransparent();
}

BaseLineMethod::~BaseLineMethod()
{
	GLHandler::deleteMesh(mesh);
}
