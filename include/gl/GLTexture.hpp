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

#ifndef GLTEXTURE_HPP
#define GLTEXTURE_HPP

#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_0_Core>

#include "PythonQtHandler.hpp"

class GLHandler;

/** @ingroup pywrap
 * @brief Represents an OpenGL Texture.
 *
 * A GLTexture represents a texture as it sits on the GPU and
 * is manipulated by OpenGL API calls. It is not a CPU array of data.
 */
class GLTexture
{
  public:
	enum class Type
	{
		TEX1D,
		TEX2D,
		TEXMULTISAMPLE,
		TEX3D,
		TEXCUBEMAP
	};
	/**
	 * @brief Representing a cubemap face.
	 *
	 * Front : X+
	 *
	 * Back : X-
	 *
	 * Left : Y+
	 *
	 * Right : Y-
	 *
	 * Top : Z+
	 *
	 * Bottom : Z-
	 */
	enum class CubemapFace
	{
		FRONT = GL_TEXTURE_CUBE_MAP_POSITIVE_X - GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		BACK  = GL_TEXTURE_CUBE_MAP_NEGATIVE_X - GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		LEFT  = GL_TEXTURE_CUBE_MAP_POSITIVE_Y - GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		RIGHT = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y - GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		TOP   = GL_TEXTURE_CUBE_MAP_POSITIVE_Z - GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		BOTTOM
		= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z - GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	};

	struct Tex1DProperties
	{
		Tex1DProperties(unsigned int width, GLint internalFormat,
		                GLenum target = GL_TEXTURE_1D)
		    : width(width)
		    , internalFormat(internalFormat)
		    , target(target){};
		explicit Tex1DProperties(unsigned int width, bool sRGB = true,
		                         GLenum target = GL_TEXTURE_1D)
		    : width(width)
		    , internalFormat(sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8)
		    , target(target){};
		unsigned int width;
		GLint internalFormat;
		GLenum target;
	};
	struct Tex2DProperties
	{
		Tex2DProperties(unsigned int width, unsigned int height,
		                GLint internalFormat, GLenum target = GL_TEXTURE_2D)
		    : width(width)
		    , height(height)
		    , internalFormat(internalFormat)
		    , target(target){};
		Tex2DProperties(unsigned int width, unsigned int height,
		                bool sRGB = true, GLenum target = GL_TEXTURE_2D)
		    : width(width)
		    , height(height)
		    , internalFormat(sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8)
		    , target(target){};
		unsigned int width;
		unsigned int height;
		GLint internalFormat;
		GLenum target;
	};
	struct TexMultisampleProperties
	{
		TexMultisampleProperties(unsigned int width, unsigned int height,
		                         unsigned int samples,
		                         GLint internalFormat = GL_RGBA8)
		    : width(width)
		    , height(height)
		    , samples(samples)
		    , internalFormat(internalFormat){};
		unsigned int width;
		unsigned int height;
		unsigned int samples;
		GLint internalFormat;
	};
	struct Tex3DProperties
	{
		Tex3DProperties(unsigned int width, unsigned int height,
		                unsigned int depth, GLint internalFormat,
		                GLenum target = GL_TEXTURE_3D)
		    : width(width)
		    , height(height)
		    , depth(depth)
		    , internalFormat(internalFormat)
		    , target(target){};
		Tex3DProperties(unsigned int width, unsigned int height,
		                unsigned int depth, bool sRGB = true,
		                GLenum target = GL_TEXTURE_3D)
		    : width(width)
		    , height(height)
		    , depth(depth)
		    , internalFormat(sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8)
		    , target(target){};
		unsigned int width;
		unsigned int height;
		unsigned int depth;
		GLint internalFormat;
		GLenum target;
	};
	struct TexCubemapProperties
	{
		explicit TexCubemapProperties(unsigned int side,
		                              GLint internalFormat = GL_SRGB8_ALPHA8,
		                              GLenum target = GL_TEXTURE_CUBE_MAP)
		    : side(side)
		    , internalFormat(internalFormat)
		    , target(target){};
		explicit TexCubemapProperties(unsigned int side, bool sRGB = true,
		                              GLenum target = GL_TEXTURE_CUBE_MAP)
		    : side(side)
		    , internalFormat(sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8)
		    , target(target){};
		unsigned int side;
		GLint internalFormat;
		GLenum target;
	};

	struct Sampler
	{
		Sampler(GLint filter = GL_LINEAR, GLint wrap = GL_CLAMP_TO_EDGE)
		    : filter(filter)
		    , wrap(wrap){};
		GLint filter;
		GLint wrap;
	};

	struct Data
	{
		Data(GLvoid const* ptr = nullptr, GLenum type = GL_UNSIGNED_BYTE,
		     GLenum format = GL_RGBA)
		    : type(type)
		    , format(format)
		    , ptr(ptr){};
		GLenum type;
		GLenum format;
		GLvoid const* ptr;
	};
	template <unsigned int U>
	struct DataArray
	{
		DataArray(std::array<GLvoid const*, U> ptrs = {nullptr},
		          GLenum type = GL_UNSIGNED_BYTE, GLenum format = GL_RGBA)
		    : type(type)
		    , format(format)
		    , ptrs(ptrs){};
		GLenum type;
		GLenum format;
		std::array<GLvoid const*, U> ptrs;
	};

	// implement those in protected if and only if they're needed for the Python
	// API
	GLTexture(GLTexture const& other) = delete;
	GLTexture& operator=(GLTexture const& other) = delete;
	/**
	 * @brief Returns the number of allocated OpenGL textures.
	 */
	static unsigned int getInstancesCount() { return instancesCount(); };

	GLTexture(GLTexture&& other)
	    : type(other.type)
	    , glTexture(other.glTexture)
	    , glTarget(other.glTarget)
	    , internalFormat(other.internalFormat)
	    , size(other.size)
	    , samples(other.samples)
	    , doClean(other.doClean)
	{
		// prevent other from cleaning shader if it destroys itself
		other.doClean = false;
	};

	explicit GLTexture(Tex1DProperties const& properties,
	                   Sampler const& sampler = {}, Data const& data = {});
	explicit GLTexture(Tex2DProperties const& properties,
	                   Sampler const& sampler = {}, Data const& data = {});
	explicit GLTexture(TexMultisampleProperties const& properties,
	                   Sampler const& sampler = {});
	explicit GLTexture(Tex3DProperties const& properties,
	                   Sampler const& sampler = {}, Data const& data = {});
	explicit GLTexture(TexCubemapProperties const& properties,
	                   Sampler const& sampler   = {},
	                   DataArray<6> const& data = {});

	explicit GLTexture(QImage const& image, bool sRGB = true);
	explicit GLTexture(const char* texturePath, bool sRGB = true);
	explicit GLTexture(std::array<QImage, 6> const& images, bool sRGB = true);
	explicit GLTexture(std::array<const char*, 6> const& texturesPaths,
	                   bool sRGB = true);

	// dangerous !
	GLuint getGLTexture() const { return glTexture; };
	// level = level of mipmapping
	QSize getSize(unsigned int level = 0) const;
	void generateMipmap() const;
	unsigned int getHighestMipmapLevel() const;
	QImage getContentAsImage(unsigned int level = 0) const;
	// allocates buff ; don't forget to delete ; returns allocated size (zero if
	// error)
	unsigned int getContentAsData(GLfloat** buff, unsigned int level = 0) const;
	float getAverageLuminance() const;

	void setSampler(Sampler const& sampler) const;
	void setData(Data const& data) const;
	void setData(DataArray<6> const& data) const;

	void setData(const unsigned char* red, const unsigned char* green,
	             const unsigned char* blue,
	             const unsigned char* alpha = nullptr);

	void use(GLenum textureUnit = GL_TEXTURE0) const;

	virtual ~GLTexture() { cleanUp(); };

  protected:
	/**
	 * @brief Frees the underlying OpenGL buffers.
	 */
	void cleanUp();

  private:
	void initData(Data const& data) const;
	void initData(DataArray<6> const& data) const;

	static QImage getImage(const char* const& path);
	static std::array<QImage, 6>
	    getImages(std::array<const char*, 6> const& paths);

	friend GLHandler;
	Type type;
	GLuint glTexture = 0;
	GLenum glTarget;
	GLint internalFormat;
	std::array<unsigned int, 3> size = {1, 1, 1};
	unsigned int samples             = 1;

	bool doClean = true;
	static unsigned int& instancesCount();
};

#endif // GLTEXTURE_HPP
