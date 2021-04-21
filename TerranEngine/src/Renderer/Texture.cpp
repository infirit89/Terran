#include "trpch.h"

#include "Texture.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace TerranEngine 
{
	Texture::Texture() 
		: m_TextureID(0), m_Width(0), m_Height(0), m_Channels(0), m_InternalFormat(0), m_DataFormat(0) {}
	
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
		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, data);
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
}
