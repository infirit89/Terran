#include "trpch.h"

#include "Texture.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace TerranEngine 
{
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
			default: TR_ASSERT(false, "No other data format supported!");
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, pixels);
	}
}
