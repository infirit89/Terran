#include "trpch.h"

#include "Texture.h"

#include <glad/glad.h>
#include <stb_image.h>
#include <filesystem>

namespace TerranEngine 
{
	struct NativeTexutreType
	{
		uint32_t InternalFormat;
		uint32_t DataFormat;
	};

	static NativeTexutreType ConvertTextureTypeToNativeType(TextureType type)
	{
		switch (type)
		{
		case TerranEngine::TextureType::RED:	return { GL_RED, GL_RED };
		case TerranEngine::TextureType::GREEN:	return { GL_GREEN, GL_GREEN };
		case TerranEngine::TextureType::BLUE:	return { GL_BLUE, GL_BLUE };
		case TerranEngine::TextureType::RGB:	return { GL_RGB8, GL_RGB };
		case TerranEngine::TextureType::RGBA:	return { GL_RGBA8, GL_RGBA };
		default:
			TR_WARN("The texture type isn't supported");
			break;
		}

		return { GL_RGBA8, GL_RGBA };
	}

	struct NativeTexutreFilter
	{
		uint32_t MinFilter;
		uint32_t MagFilter;
	};

	static NativeTexutreFilter ConvertTextureFilterToNativeFilter(TextureFilter minFilter, TextureFilter magFilter) 
	{
		NativeTexutreFilter filter = { GL_LINEAR, GL_LINEAR };

		switch (minFilter)
		{
		case TerranEngine::TextureFilter::LINEAR:
		case TerranEngine::TextureFilter::NEAREST:
			filter.MinFilter = GL_LINEAR - (uint32_t)minFilter;
			break;
		default:
			TR_WARN("The texture filter isn't supported");
			break;
		}

		switch (magFilter)
		{
		case TerranEngine::TextureFilter::LINEAR:
		case TerranEngine::TextureFilter::NEAREST:
			filter.MagFilter = GL_LINEAR - (uint32_t)magFilter;
			break;
		default:
			TR_WARN("The texture filter isn't supported");
			break;
		}

		return filter;
	}

	Texture::Texture() 
		: m_TextureID(0), m_Width(0), m_Height(0), m_Channels(0), m_InternalFormat(0), m_DataFormat(0), m_Name("") {}
	
	Texture::Texture(uint32_t width, uint32_t height, TextureParameters parameters) 
		: m_TextureID(0), m_Width(width), m_Height(height), m_Channels(0), 
		m_InternalFormat(0), m_DataFormat(0), m_TexParameters(parameters)
	{
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		
		NativeTexutreFilter filter = ConvertTextureFilterToNativeFilter(m_TexParameters.MinFilter, m_TexParameters.MagFilter);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter.MinFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter.MagFilter);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	Texture::Texture(const char* filePath, TextureParameters parameters)
		: m_TextureID(0), m_Width(0), m_Height(0), m_Channels(0), m_InternalFormat(0), m_DataFormat(0)
	{
		std::filesystem::path path = filePath;
		m_Name = path.stem().string();

		LoadTexture(filePath);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_TextureID);
	}

	void Texture::Bind(uint8_t textureSlot) const
	{	 
		glActiveTexture(GL_TEXTURE0 + textureSlot);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
	}	 
		 
	void Texture::Unbind() const
	{	 
		glBindTexture(GL_TEXTURE_2D, 0);
	}	 

	void Texture::SetData(void* data) 
	{
		NativeTexutreType nativeType = ConvertTextureTypeToNativeType(m_TexParameters.TextureType);

		glTexImage2D(GL_TEXTURE_2D, 0, nativeType.InternalFormat, m_Width, m_Height, 0, nativeType.DataFormat, GL_UNSIGNED_BYTE, data);
	}

	bool Texture::operator==(Texture& other) 
	{
		return m_Width == other.m_Width && m_Height == other.m_Height
			&& m_Channels == other.m_Channels && m_TextureID == other.m_TextureID;
	}

	bool Texture::operator==(const Texture& other) 
	{
		return m_Width == other.m_Width && m_Height == other.m_Height
			&& m_Channels == other.m_Channels && m_TextureID == other.m_TextureID;
	}

	void Texture::LoadTexture(const char* filePath)
	{
		stbi_set_flip_vertically_on_load(1);
		unsigned char* pixels = stbi_load(filePath, &m_Width, &m_Height, &m_Channels, 0);

		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);

		switch (m_Channels)
		{
			case 4: 
			{
				m_InternalFormat = GL_RGBA8;
				m_DataFormat = GL_RGBA;
				break;
			}
			case 3: 
			{
				m_InternalFormat = GL_RGB8;
				m_DataFormat = GL_RGB;
				break;
			}
			default: TR_TRACE(m_Channels); TR_ASSERT(false, "No other data format supported!");
		}

		NativeTexutreFilter filter = ConvertTextureFilterToNativeFilter(m_TexParameters.MinFilter, m_TexParameters.MagFilter);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter.MinFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter.MagFilter);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, pixels);
	}
}
