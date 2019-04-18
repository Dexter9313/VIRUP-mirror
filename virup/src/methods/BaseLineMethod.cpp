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
{
}

void BaseLineMethod::init(std::vector<float>& gazVertices,
                          std::vector<float>& starsVertices,
                          std::vector<float>& darkMatterVertices)
{
	cleanUp();
	gazMesh        = GLHandler::newMesh();
	starsMesh      = GLHandler::newMesh();
	darkMatterMesh = GLHandler::newMesh();
	size_t totalSize(0);
	GLHandler::setVertices(gazMesh, gazVertices, shaderProgram,
	                       {{"position", 3}, {"radius", 1}});
	GLHandler::setShaderUnusedAttributesValues(shaderProgram,
	                                           {{"luminosity", {1.f}}});
	totalSize += gazVertices.size();
	GLHandler::setVertices(starsMesh, starsVertices, shaderProgram,
	                       {{"position", 3}, {"radius", 1}});
	GLHandler::setShaderUnusedAttributesValues(shaderProgram,
	                                           {{"luminosity", {1.f}}});
	totalSize += starsVertices.size();
	GLHandler::setVertices(darkMatterMesh, darkMatterVertices, shaderProgram,
	                       {{"position", 3}, {"radius", 1}});
	GLHandler::setShaderUnusedAttributesValues(shaderProgram,
	                                           {{"luminosity", {1.f}}});
	totalSize += darkMatterVertices.size();
	std::cout << "VRAM Loaded with " << totalSize * sizeof(float)
	          << " bytes worth of data" << std::endl;
}

void BaseLineMethod::init(std::string const& gazPath,
                          std::string const& starsPath,
                          std::string const& darkMatterPath)
{
	cleanUp();
	gazMesh        = GLHandler::newMesh();
	starsMesh      = GLHandler::newMesh();
	darkMatterMesh = GLHandler::newMesh();
	size_t totalSize(0);
	std::vector<BBox> bboxes;
	if(!gazPath.empty())
	{
		std::ifstream file;
		file.open(gazPath, std::fstream::in | std::fstream::binary);
		OctreeLOD tree(shaderProgram);
		file.seekg(0);
		tree.init(file);
		tree.readData(file);
		bboxes.push_back(tree.getBoundingBox());
		std::vector<float> data(tree.getData());
		GLHandler::setVertices(gazMesh, data, shaderProgram,
		                       {{"position", 3}, {"radius", 1}});
		totalSize += data.size();
		file.close();
	}
	if(!starsPath.empty())
	{
		std::ifstream file;
		file.open(starsPath, std::fstream::in | std::fstream::binary);
		OctreeLOD tree(shaderProgram);
		file.seekg(0);
		tree.init(file);
		tree.readData(file);
		bboxes.push_back(tree.getBoundingBox());
		std::vector<float> data(tree.getData());
		GLHandler::setVertices(starsMesh, data, shaderProgram,
		                       {{"position", 3}});
		totalSize += data.size();
		file.close();
	}
	if(!darkMatterPath.empty())
	{
		std::ifstream file;
		file.open(darkMatterPath, std::fstream::in | std::fstream::binary);
		OctreeLOD tree(shaderProgram);
		file.seekg(0);
		tree.init(file);
		tree.readData(file);
		bboxes.push_back(tree.getBoundingBox());
		std::vector<float> data(tree.getData());
		GLHandler::setVertices(darkMatterMesh, data, shaderProgram,
		                       {{"position", 3}});
		totalSize += data.size();
		file.close();
	}
	dataBBox = globalBBox(bboxes);
	std::cout << "VRAM Loaded with " << totalSize * sizeof(float)
	          << " bytes worth of data" << std::endl;
}

void BaseLineMethod::render(Camera const& camera, double scale,
                            std::array<double, 3> const& translation)
{
	QMatrix4x4 model;
	model.translate(QVector3D(translation[0], translation[1], translation[2]));
	model.scale(scale);

	GLHandler::setShaderParam(shaderProgram, "alpha",
	                          static_cast<float>(scale * scale * getAlpha()));
	GLHandler::setShaderParam(shaderProgram, "view",
	                          camera.hmdScaledSpaceToWorldTransform().inverted()
	                              * model);
	GLHandler::beginTransparent(GL_SRC_ALPHA, GL_ONE);
	GLHandler::setUpRender(shaderProgram, model);
	GLHandler::setShaderParam(
	    shaderProgram, "color",
	    QSettings().value("data/gazcolor").value<QColor>());
	GLHandler::setShaderUnusedAttributesValues(shaderProgram,
	                                           {{"luminosity", {1.f}}});
	GLHandler::render(gazMesh);
	GLHandler::setShaderParam(
	    shaderProgram, "color",
	    QSettings().value("data/starscolor").value<QColor>());
	GLHandler::setShaderUnusedAttributesValues(
	    shaderProgram, {{"radius", {1.f}}, {"luminosity", {1.f}}});
	GLHandler::render(starsMesh);
	GLHandler::setShaderParam(
	    shaderProgram, "color",
	    QSettings().value("data/darkmattercolor").value<QColor>());
	GLHandler::setShaderUnusedAttributesValues(
	    shaderProgram, {{"radius", {1.f}}, {"luminosity", {1.f}}});
	GLHandler::render(darkMatterMesh);
	GLHandler::endTransparent();
}

void BaseLineMethod::cleanUp()
{
	GLHandler::deleteMesh(gazMesh);
	GLHandler::deleteMesh(starsMesh);
	GLHandler::deleteMesh(darkMatterMesh);
}

BaseLineMethod::~BaseLineMethod()
{
	GLHandler::deleteMesh(gazMesh);
	GLHandler::deleteMesh(starsMesh);
	GLHandler::deleteMesh(darkMatterMesh);
}
