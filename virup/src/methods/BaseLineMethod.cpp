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
	size_t totalSize(0);
	gazMesh.setVertexShaderMapping(shaderProgram,
	                               {{"position", 3}, {"radius", 1}});
	gazMesh.setVertices(gazVertices);
	shaderProgram.setUnusedAttributesValues({{"luminosity", {1.f}}});
	totalSize += gazVertices.size();
	starsMesh.setVertexShaderMapping(shaderProgram,
	                                 {{"position", 3}, {"radius", 1}});
	starsMesh.setVertices(starsVertices);
	shaderProgram.setUnusedAttributesValues({{"luminosity", {1.f}}});
	totalSize += starsVertices.size();
	darkMatterMesh.setVertexShaderMapping(shaderProgram,
	                                      {{"position", 3}, {"radius", 1}});
	darkMatterMesh.setVertices(darkMatterVertices);
	shaderProgram.setUnusedAttributesValues({{"luminosity", {1.f}}});
	totalSize += darkMatterVertices.size();
	std::cout << "VRAM Loaded with " << totalSize * sizeof(float)
	          << " bytes worth of data" << std::endl;
}

void BaseLineMethod::init(std::string const& gazPath,
                          std::string const& starsPath,
                          std::string const& darkMatterPath)
{
	cleanUp();
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
		gazMesh.setVertexShaderMapping(shaderProgram,
		                               {{"position", 3}, {"radius", 1}});
		gazMesh.setVertices(data);
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
		starsMesh.setVertexShaderMapping(shaderProgram, {{"position", 3}});
		starsMesh.setVertices(data);
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
		darkMatterMesh.setVertexShaderMapping(shaderProgram, {{"position", 3}});
		darkMatterMesh.setVertices(data);
		totalSize += data.size();
		file.close();
	}
	dataBBox = globalBBox(bboxes);
	std::cout << "VRAM Loaded with " << totalSize * sizeof(float)
	          << " bytes worth of data" << std::endl;
}

void BaseLineMethod::render(Camera const& camera)
{
	QMatrix4x4 model(camera.dataToWorldTransform());

	shaderProgram.setUniform(
	    "alpha", static_cast<float>(camera.scale * camera.scale * getAlpha()));
	shaderProgram.setUniform(
	    "view", camera.hmdScaledSpaceToWorldTransform().inverted() * model);
	GLHandler::beginTransparent(GL_SRC_ALPHA, GL_ONE);
	GLHandler::setUpRender(shaderProgram, model);
	shaderProgram.setUniform(
	    "color", QSettings().value("data/gazcolor").value<QColor>());
	shaderProgram.setUnusedAttributesValues({{"luminosity", {1.f}}});
	gazMesh.render();
	shaderProgram.setUniform(
	    "color", QSettings().value("data/starscolor").value<QColor>());
	shaderProgram.setUnusedAttributesValues(
	    {{"radius", {1.f}}, {"luminosity", {1.f}}});
	starsMesh.render();
	shaderProgram.setUniform(
	    "color", QSettings().value("data/darkmattercolor").value<QColor>());
	shaderProgram.setUnusedAttributesValues(
	    {{"radius", {1.f}}, {"luminosity", {1.f}}});
	darkMatterMesh.render();
	GLHandler::endTransparent();
}
