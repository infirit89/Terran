#pragma once

namespace TerranEngine 
{
	class Texture 
	{
	public:
		Texture();

		Texture(uint32_t width, uint32_t height);
		Texture(const char* filePath);
		~Texture();

		void Bind(uint8_t textureSlot) const;
		void Unbind() const;

		void SetData(void* data, uint32_t internalFormat, uint32_t dataFormat);
		void SetData(void* data);

		static Texture& EmptyTexture();

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }

		bool operator==(Texture& other);
		bool operator==(const Texture& other);
	private:
		void LoadTexture(const char* filePath);

		uint32_t m_TextureID;
		int m_Width, m_Height, m_Channels;
		uint32_t m_InternalFormat, m_DataFormat;
	};
}