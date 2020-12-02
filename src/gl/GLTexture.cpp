/*
    Copyright (C) 2020 Florian Cabot <florian.cabot@hotmail.fr>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "gl/GLHandler.hpp"

#include "gl/GLTexture.hpp"

unsigned int& GLTexture::instancesCount()
{
	static unsigned int instancesCount = 0;
	return instancesCount;
}

GLTexture::GLTexture(Tex1DProperties const& properties, Sampler const& sampler,
                     Data const& data)
{
	++instancesCount();
	type           = Type::TEX1D;
	glTarget       = properties.target;
	internalFormat = properties.internalFormat;
	size[0]        = properties.width;

	GLHandler::glf().glGenTextures(1, &glTexture);
	initData(data);
	setSampler(sampler);
}

GLTexture::GLTexture(Tex2DProperties const& properties, Sampler const& sampler,
                     Data const& data)
{
	++instancesCount();
	type           = Type::TEX2D;
	glTarget       = properties.target;
	internalFormat = properties.internalFormat;
	size[0]        = properties.width;
	size[1]        = properties.height;

	GLHandler::glf().glGenTextures(1, &glTexture);
	initData(data);
	setSampler(sampler);
}

GLTexture::GLTexture(TexMultisampleProperties const& properties,
                     Sampler const& sampler)
{
	++instancesCount();
	type           = Type::TEXMULTISAMPLE;
	glTarget       = GL_TEXTURE_2D_MULTISAMPLE;
	internalFormat = properties.internalFormat;
	size[0]        = properties.width;
	size[1]        = properties.height;
	samples        = properties.samples;

	GLHandler::glf().glGenTextures(1, &glTexture);
	GLHandler::glf().glBindTexture(glTarget, glTexture);
	GLHandler::glf().glTexImage2DMultisample(glTarget, properties.samples,
	                                         internalFormat, properties.width,
	                                         properties.height, GL_TRUE);
	// glGenerateMipmap(target);
	GLHandler::glf().glBindTexture(glTarget, 0);
	setSampler(sampler);
}

GLTexture::GLTexture(Tex3DProperties const& properties, Sampler const& sampler,
                     Data const& data)
{
	++instancesCount();
	type           = Type::TEX3D;
	glTarget       = properties.target;
	internalFormat = properties.internalFormat;
	size[0]        = properties.width;
	size[1]        = properties.height;
	size[2]        = properties.depth;

	GLHandler::glf().glGenTextures(1, &glTexture);
	initData(data);
	setSampler(sampler);
}

GLTexture::GLTexture(TexCubemapProperties const& properties,
                     Sampler const& sampler, DataArray<6> const& data)
{
	++instancesCount();
	type           = Type::TEXCUBEMAP;
	glTarget       = properties.target;
	internalFormat = properties.internalFormat;
	size[0]        = properties.side;
	size[1]        = properties.side;

	GLHandler::glf().glGenTextures(1, &glTexture);
	initData(data);
	setSampler(sampler);
}

GLTexture::GLTexture(QImage const& image, bool sRGB)
    : GLTexture(GLTexture::Tex2DProperties(image.width(), image.height(), sRGB))
{
	QImage img_data = image.convertToFormat(QImage::Format_RGBA8888);
	setData({img_data.bits()});
}

GLTexture::GLTexture(const char* texturePath, bool sRGB)
    : GLTexture(getImage(texturePath), sRGB)
{
}

GLTexture::GLTexture(std::array<QImage, 6> const& images, bool sRGB)
    : GLTexture(GLTexture::TexCubemapProperties(images.at(0).width(), sRGB))
{
	std::array<GLvoid const*, 6> data = {};

	std::array<QImage, 6> img_data = {};

	for(unsigned int i(0); i < 6; ++i)
	{
		switch(i)
		{
			case static_cast<int>(CubemapFace::FRONT):
			case static_cast<int>(CubemapFace::TOP):
			case static_cast<int>(CubemapFace::BOTTOM):
			{
				QImage im(images.at(i).height(), images.at(i).width(),
				          QImage::Format_RGBA8888);
				for(int j(0); j < im.height(); ++j)
				{
					for(int k(0); k < im.width(); ++k)
					{
						im.setPixel(k, j, images.at(i).pixel(j, k));
					}
				}
				img_data.at(i) = im;
			}
			break;
			case static_cast<int>(CubemapFace::BACK):
			{
				QImage im(images.at(i).height(), images.at(i).width(),
				          QImage::Format_RGBA8888);
				for(int j(0); j < im.height(); ++j)
				{
					for(int k(0); k < im.width(); ++k)
					{
						im.setPixel(im.width() - k - 1, im.height() - j - 1,
						            images.at(i).pixel(j, k));
					}
				}
				img_data.at(i) = im;
			}
			break;
			case static_cast<int>(CubemapFace::LEFT):
				img_data.at(i) = images.at(i)
				                     .mirrored(false, true)
				                     .convertToFormat(QImage::Format_RGBA8888);
				break;
			case static_cast<int>(CubemapFace::RIGHT):
				img_data.at(i) = images.at(i)
				                     .mirrored(true, false)
				                     .convertToFormat(QImage::Format_RGBA8888);
				break;
			default:
				break;
		}
	}
	for(unsigned int i(0); i < 6; ++i)
	{
		data.at(i) = img_data.at(i).bits();
	}

	setData({data});
}

GLTexture::GLTexture(std::array<const char*, 6> const& texturesPaths, bool sRGB)
    : GLTexture(getImages(texturesPaths), sRGB)
{
}

QSize GLTexture::getSize(unsigned int level) const
{
	GLint width, height;
	GLHandler::glf().glBindTexture(glTarget, glTexture);
	GLHandler::glf().glGetTexLevelParameteriv(glTarget, level, GL_TEXTURE_WIDTH,
	                                          &width);
	GLHandler::glf().glGetTexLevelParameteriv(glTarget, level,
	                                          GL_TEXTURE_HEIGHT, &height);
	GLHandler::glf().glBindTexture(glTarget, 0);

	return {width, height};
}

void GLTexture::generateMipmap() const
{
	GLHandler::glf().glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	GLHandler::glf().glBindTexture(glTarget, glTexture);
	GLHandler::glf().glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER,
	                                 GL_LINEAR_MIPMAP_LINEAR);
	GLHandler::glf().glGenerateMipmap(glTarget);
	GLHandler::glf().glBindTexture(glTarget, 0);
}

unsigned int GLTexture::getHighestMipmapLevel() const
{
	QSize size(getSize());
	return static_cast<unsigned int>(
	    log2(size.width() > size.height() ? size.width() : size.height()));
}

QImage GLTexture::getContentAsImage(unsigned int level) const
{
	QSize size(getSize(level));

	GLint internalFormat;
	GLHandler::glf().glBindTexture(glTarget, glTexture);
	GLHandler::glf().glGetTexLevelParameteriv(
	    glTarget, level, GL_TEXTURE_INTERNAL_FORMAT,
	    &internalFormat);  // get internal format type of GL texture
	switch(internalFormat) // determine what type GL texture has...
	{
		case GL_RGB:
		{
			QImage result(size, QImage::Format::Format_RGB888);
			GLHandler::glf().glGetTexImage(glTarget, level, GL_RGBA,
			                               GL_UNSIGNED_BYTE, result.bits());
			return result;
		}
		break;
		case GL_RGBA:
		{
			QImage result(size, QImage::Format::Format_RGBA8888);
			GLHandler::glf().glGetTexImage(glTarget, level, GL_RGBA,
			                               GL_UNSIGNED_BYTE, result.bits());
			return result;
		}
		break;
		case GL_SRGB8_ALPHA8:
		{
			QImage result(size, QImage::Format::Format_RGBA8888);
			GLHandler::glf().glGetTexImage(glTarget, level, GL_RGBA,
			                               GL_UNSIGNED_BYTE, result.bits());
			return result;
		}
		default: // unsupported type for now
			break;
	}

	return {};
}

unsigned int GLTexture::getContentAsData(GLfloat** buff,
                                         unsigned int level) const
{
	QSize size(getSize(level));

	GLint internalFormat;
	GLHandler::glf().glBindTexture(glTarget, glTexture);
	GLHandler::glf().glGetTexLevelParameteriv(
	    glTarget, level, GL_TEXTURE_INTERNAL_FORMAT,
	    &internalFormat); // get internal format type of GL texture
	GLint numFloats = 0;
	if(internalFormat == GL_RGBA32F) // determine what type GL texture has...
	{
		numFloats = size.width() * size.height() * 4;
		*buff     = new GLfloat[numFloats];
		GLHandler::glf().glGetTexImage(glTarget, level, GL_RGBA, GL_FLOAT,
		                               *buff);
	}
	return numFloats;
}

float GLTexture::getAverageLuminance() const
{
	generateMipmap();
	unsigned int lvl = getHighestMipmapLevel() - 3;
	auto size        = getSize(lvl);
	GLfloat* buff;
	unsigned int allocated(getContentAsData(&buff, lvl));
	float lastFrameAverageLuminance = 0.f;
	if(allocated > 0)
	{
		float coeffSum = 0.f;
		float halfWidth((size.width() - 1) / 2.f);
		float halfHeight((size.height() - 1) / 2.f);
		for(int i(0); i < size.width(); ++i)
		{
			for(int j(0); j < size.height(); ++j)
			{
				unsigned int id(j * size.width() + i);
				float lum(0.2126 * buff[4 * id] + 0.7152 * buff[4 * id + 1]
				          + 0.0722 * buff[4 * id + 2]);
				float coeff
				    = exp(-1 * pow((i - halfWidth) * 4.5 / halfWidth, 2));
				coeff *= exp(-1 * pow((j - halfHeight) * 4.5 / halfHeight, 2));
				coeffSum += coeff;
				lastFrameAverageLuminance += coeff * lum;
			}
		}
		lastFrameAverageLuminance /= coeffSum;
		delete[] buff;
	}
	return lastFrameAverageLuminance;
}

void GLTexture::setSampler(Sampler const& sampler) const
{
	GLHandler::glf().glBindTexture(glTarget, glTexture);
	GLHandler::glf().glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER,
	                                 sampler.filter);
	GLHandler::glf().glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER,
	                                 sampler.filter);
	GLHandler::glf().glTexParameteri(glTarget, GL_TEXTURE_WRAP_S, sampler.wrap);
	GLHandler::glf().glTexParameteri(glTarget, GL_TEXTURE_WRAP_T, sampler.wrap);
	GLHandler::glf().glTexParameteri(glTarget, GL_TEXTURE_WRAP_R, sampler.wrap);
	/*GLfloat fLargest;
	glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest );
	glTexParameterf( format, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest );*/
	GLHandler::glf().glBindTexture(glTarget, 0);
}

void GLTexture::setData(Data const& data) const
{
	GLHandler::glf().glBindTexture(glTarget, glTexture);
	switch(type)
	{
		case Type::TEX1D:
			GLHandler::glf().glTexSubImage1D(glTarget, 0, 0, size[0],
			                                 data.format, data.type, data.ptr);
			break;
		case Type::TEX2D:
			GLHandler::glf().glTexSubImage2D(glTarget, 0, 0, 0, size[0],
			                                 size[1], data.format, data.type,
			                                 data.ptr);
			break;
		case Type::TEXMULTISAMPLE:
			qWarning() << "Attempt to set data of a multisampled texture.";
			break;
		case Type::TEX3D:
			GLHandler::glf().glTexSubImage3D(glTarget, 0, 0, 0, 0, size[0],
			                                 size[1], size[2], data.format,
			                                 data.type, data.ptr);
			break;
		case Type::TEXCUBEMAP:
			qWarning()
			    << "Attempt to set data of cubemap texture for only one face.";
			break;
	}
	// glGenerateMipmap(format);
	GLHandler::glf().glBindTexture(glTarget, 0);
}

void GLTexture::setData(DataArray<6> const& data) const
{
	if(type != Type::TEXCUBEMAP)
	{
		qWarning() << "Attempt to set cubemap data on another texture type.";
		return;
	}
	GLHandler::glf().glBindTexture(glTarget, glTexture);
	for(unsigned int i(0); i < 6; ++i)
	{
		GLHandler::glf().glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
		                                 0, 0, size[0], size[1], data.format,
		                                 data.type, data.ptrs.at(i));
	}
	// glGenerateMipmap(format);
	GLHandler::glf().glBindTexture(glTarget, 0);
}

void GLTexture::setData(const unsigned char* red, const unsigned char* green,
                        const unsigned char* blue, const unsigned char* alpha)
{
	const unsigned int nPix(size[0] * size[1] * size[2]);
	auto image = new GLubyte[nPix * 4];
	for(unsigned int i(0); i < nPix; ++i)
	{
		image[4 * i]     = red[i];
		image[4 * i + 1] = green[i];
		image[4 * i + 2] = blue[i];
		image[4 * i + 3] = alpha != nullptr ? alpha[i] : 255;
	}
	setData({image});
	delete[] image;
}

void GLTexture::use(GLenum textureUnit) const
{
	GLHandler::glf().glActiveTexture(textureUnit);
	GLHandler::glf().glBindTexture(glTarget, glTexture);
}

void GLTexture::cleanUp()
{
	--instancesCount();
	GLHandler::glf().glDeleteTextures(1, &glTexture);
}

void GLTexture::initData(Data const& data) const
{
	GLHandler::glf().glBindTexture(glTarget, glTexture);
	switch(type)
	{
		case Type::TEX1D:
			GLHandler::glf().glTexImage1D(glTarget, 0, internalFormat, size[0],
			                              0, data.format, data.type, data.ptr);
			break;
		case Type::TEX2D:
			GLHandler::glf().glTexImage2D(glTarget, 0, internalFormat, size[0],
			                              size[1], 0, data.format, data.type,
			                              data.ptr);
			break;
		case Type::TEXMULTISAMPLE:
			qWarning() << "Attempt to set data of a multisampled texture.";
			break;
		case Type::TEX3D:
			GLHandler::glf().glTexImage3D(glTarget, 0, internalFormat, size[0],
			                              size[1], size[2], 0, data.format,
			                              data.type, data.ptr);
			break;
		case Type::TEXCUBEMAP:
			qWarning()
			    << "Attempt to set data of cubemap texture for only one face.";
			break;
	}
	// glGenerateMipmap(format);
	GLHandler::glf().glBindTexture(glTarget, 0);
}

void GLTexture::initData(DataArray<6> const& data) const
{
	if(type != Type::TEXCUBEMAP)
	{
		qWarning() << "Attempt to set cubemap data on another texture type.";
		return;
	}
	GLHandler::glf().glBindTexture(glTarget, glTexture);
	for(unsigned int i(0); i < 6; ++i)
	{
		GLHandler::glf().glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
		                              internalFormat, size[0], size[1], 0,
		                              data.format, data.type, data.ptrs.at(i));
	}
	// glGenerateMipmap(format);
	GLHandler::glf().glBindTexture(glTarget, 0);
}

QImage GLTexture::getImage(const char* const& path)
{
	QImage img_data;
	if(!img_data.load(path))
	{
		qWarning() << "Could not load Texture \"" << path << "\"" << '\n';
		return {};
	}
	return img_data;
}

std::array<QImage, 6>
    GLTexture::getImages(std::array<const char*, 6> const& paths)
{
	std::array<QImage, 6> images;
	for(unsigned int i(0); i < 6; ++i)
	{
		if(!images.at(i).load(paths.at(i)))
		{
			qWarning() << "Could not load Texture \"" << paths.at(i) << "\""
			           << '\n';
			return {};
		}
	}

	return images;
}
