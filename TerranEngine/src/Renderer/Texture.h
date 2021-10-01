#pragma once

namespace TerranEngine 
{
	enum class TextureType : uint32_t
	{
		RED,
		GREEN,
		BLUE,
		RGB,
		RGBA
	};

	class Texture 
	{
	public:
		Texture();

		Texture(uint32_t width, uint32_t height);
		Texture(const char* filePath);
		~Texture();

		void Bind(uint8_t textureSlot) const;
		void Unbind() const;

		void SetData(void* data, TextureType type = TextureType::RGBA);

		static Texture& EmptyTexture();

		inline const int GetWidth() const { return m_Width; }
		inline const int GetHeight() const { return m_Height; }

		inline const std::string GetName() const { return m_Name; }

		bool operator==(Texture& other);
		bool operator==(const Texture& other);
	private:
		struct NativeTexutreType 
		{
			uint32_t InternalFormat;
			uint32_t DataFormat;
		};

		void LoadTexture(const char* filePath);
		NativeTexutreType ConvertTextureTypeToNativeType(TextureType type);

		uint32_t m_TextureID;
		int m_Width, m_Height, m_Channels;
		uint32_t m_InternalFormat, m_DataFormat;
		std::string m_Name;
	};
}