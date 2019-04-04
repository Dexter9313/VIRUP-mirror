#include "methods/Method.hpp"

Method::Method(std::string const& shadersCommonName)
    : Method(shadersCommonName, shadersCommonName)
{
}

Method::Method(std::string const& vertexShaderPath,
               std::string const& fragmentShaderPath)
{
	shaderProgram = GLHandler::newShader(vertexShaderPath.c_str(),
	                                     fragmentShaderPath.c_str());
	resetAlpha();
}

void Method::init(std::string const& gazPath)
{
	init(gazPath, "", "");
}

void Method::init(std::string const& gazPath, std::string const& starsPath)
{
	init(gazPath, starsPath, "");
}

void Method::resetAlpha()
{
	setAlpha(0.011);
}

void Method::setAlpha(float alpha)
{
	this->alpha = alpha;
	GLHandler::setShaderParam(shaderProgram, "alpha", alpha);
}

BBox Method::globalBBox(std::vector<BBox> const& bboxes)
{
	BBox result;
	result.minx = FLT_MAX;
	result.maxx = -FLT_MAX;
	result.miny = FLT_MAX;
	result.maxy = -FLT_MAX;
	result.minz = FLT_MAX;
	result.maxz = -FLT_MAX;

	for(auto bbox : bboxes)
	{
		if(bbox.minx < result.minx)
		{
			result.minx = bbox.minx;
		}
		if(bbox.maxx > result.maxx)
		{
			result.maxx = bbox.maxx;
		}
		if(bbox.miny < result.miny)
		{
			result.miny = bbox.miny;
		}
		if(bbox.maxy > result.maxy)
		{
			result.maxy = bbox.maxy;
		}
		if(bbox.minz < result.minz)
		{
			result.minz = bbox.minz;
		}
		if(bbox.maxz > result.maxz)
		{
			result.maxz = bbox.maxz;
		}
	}
	result.diameter
	    = sqrtf((result.maxx - result.minx) * (result.maxx - result.minx)
	            + (result.maxy - result.miny) * (result.maxy - result.miny)
	            + (result.maxz - result.minz) * (result.maxz - result.minz));

	result.mid.setX((result.maxx + result.minx) / 2.0f);
	result.mid.setY((result.maxy + result.miny) / 2.0f);
	result.mid.setZ((result.maxz + result.minz) / 2.0f);

	return result;
}

Method::~Method()
{
	GLHandler::deleteShader(shaderProgram);
}
