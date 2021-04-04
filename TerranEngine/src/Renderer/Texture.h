#pragma once

namespace TerranEngine 
{
	class Texture 
	{
	public:
		Texture(const char* filePath);
		~Texture();

		void Bind(uint8_t textureSlot) const;
		void Unbind() const;
	private:
		void LoadTexture(const char* filePath);

		uint32_t m_TextureID;
		int m_Width, m_Height, m_Channels;
		uint32_t m_InternalFormat, m_DataFormat;
	};
}