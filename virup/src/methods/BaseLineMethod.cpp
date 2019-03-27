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
    , pointSize(1)
{
	GLHandler::setShaderParam(shaderProgram, "color",
	                          QVector3D(1.0f, 1.0f, 1.0f));
}

void BaseLineMethod::init(std::vector<float>& gazVertices,
                          std::vector<float>& starsVertices,
                          std::vector<float>& darkMatterVertices)
{
	cleanUp();
	mesh = GLHandler::newMesh();
	std::vector<float> vertices(gazVertices);
	gazVertices.clear();
	vertices.insert(vertices.end(), starsVertices.begin(), starsVertices.end());
	starsVertices.clear();
	vertices.insert(vertices.end(), darkMatterVertices.begin(),
	                darkMatterVertices.end());
	darkMatterVertices.clear();
	GLHandler::setVertices(mesh, vertices, shaderProgram, {{"position", 3}});
	std::cout << "VRAM Loaded with " << vertices.size() * sizeof(vertices[0])
	          << " bytes worth of data" << std::endl;
}

void BaseLineMethod::init(std::string const& gazPath,
                          std::string const& starsPath,
                          std::string const& darkMatterPath)
{
	cleanUp();
	mesh = GLHandler::newMesh();
	std::vector<float> vertices;
	if(!gazPath.empty())
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
	if(!starsPath.empty())
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
	if(!darkMatterPath.empty())
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

void BaseLineMethod::render(Camera const& camera, double scale,
                            std::array<double, 3> const& translation)
{
	QMatrix4x4 model;
	model.translate(QVector3D(translation[0], translation[1], translation[2]));
	model.scale(scale);

	GLHandler::setShaderParam(shaderProgram, "view",
	                          camera.hmdScaledSpaceToWorldTransform().inverted()
	                              * model);
	GLHandler::setPointSize(pointSize);
	GLHandler::beginTransparent();
	GLHandler::setUpRender(shaderProgram, model);
	GLHandler::render(mesh);
	GLHandler::endTransparent();
}

void BaseLineMethod::cleanUp()
{
	GLHandler::deleteMesh(mesh);
}

BaseLineMethod::~BaseLineMethod()
{
	GLHandler::deleteMesh(mesh);
}
