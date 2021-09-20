#include "trpch.h"

#include "Texture.h"

#include <glad/glad.h>
#include <stb_image.h>
#include <filesystem>

namespace TerranEngine 
{
	Texture::Texture() 
		: m_TextureID(0), m_Width(0), m_Height(0), m_Channels(0), m_InternalFormat(0), m_DataFormat(0), m_Name(nullptr) {}
	
	Texture::Texture(uint32_t width, uint32_t height) 
		: m_TextureID(0), m_Width(width), m_Height(height), m_Channels(0), 
		m_InternalFormat(GL_RGBA8), m_DataFormat(GL_RGBA) 
	{
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	Texture::Texture(const char* filePath)
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

	void Texture::SetData(void* data, TextureType type) 
	{
		NativeTexutreType nativeType = ConvertTextureTypeToNativeType(type);

		glTexImage2D(GL_TEXTURE_2D, 0, nativeType.InternalFormat, m_Width, m_Height, 0, nativeType.DataFormat, GL_UNSIGNED_BYTE, data);
	}

	Texture& Texture::EmptyTexture()
	{
		Texture tex(1, 1);
		tex.SetData(0x00000000);

		return tex;
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

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, pixels);
	}

	Texture::NativeTexutreType Texture::ConvertTextureTypeToNativeType(TextureType type)
	{
		uint32_t dataFormat = 0;
		uint32_t internalType = 0;

		switch (type)
		{
		case TerranEngine::TextureType::RED:
			dataFormat = GL_RED;
			internalType = GL_RED;
			break;
		case TerranEngine::TextureType::GREEN:
			dataFormat = GL_GREEN;
			internalType = GL_GREEN;
			break;
		case TerranEngine::TextureType::BLUE:
			dataFormat = GL_BLUE;
			internalType = GL_BLUE;
			break;
		case TerranEngine::TextureType::RGB:
			dataFormat = GL_RGB;
			internalType = GL_RGB8;
			break;
		case TerranEngine::TextureType::RGBA:
			dataFormat = GL_RGBA;
			internalType = GL_RGBA8;
			break;
		}

		return { internalType, dataFormat };
	}
}
