#pragma once

#include "Assets/Asset.h"

#include <string>
#include <filesystem>
#include <stdint.h>

namespace TerranEngine 
{
	enum class TextureType
	{
		Red = 0,
		Red32Integer,
		RGB,
		RGBA
	};

	enum class TextureFilter
	{
		Linear = 0,
		Nearest
	};

	enum class TextureWrapMode
	{
		Repeat = 0,
		Mirror,
		MirrorOnce,
		ClampToEdge
	};

	struct TextureParameters 
	{
		// TODO: put pixel data, width and height into the texture parameters
		TextureType TextureType = TextureType::RGBA;

		TextureFilter MinFilter = TextureFilter::Linear;
		TextureFilter MagFilter = TextureFilter::Linear;
		
		TextureWrapMode WrapMode = TextureWrapMode::ClampToEdge;
	};

	class Texture : public Asset
	{
	public:
		Texture();

		Texture(uint32_t width, uint32_t height, TextureParameters parameters = {});
		Texture(const std::filesystem::path& filePath, TextureParameters parameters = {});

		~Texture();

		void Bind(uint8_t textureSlot) const;
		void Unbind() const;

		void SetData(const void* data);

		inline const TextureParameters GetTextureParameters() const { return m_TexParameters; }
		inline const int GetWidth() const { return m_Width; }
		inline const int GetHeight() const { return m_Height; }
		inline const uint32_t GetTextureID() const { return m_TextureID; }

		inline const std::filesystem::path GetPath() const { return m_Path; }
		inline const std::string GetName() const { return m_Path.stem().string(); }

		bool operator==(Texture& other);
		bool operator==(const Texture& other);

		ASSET_CLASS_TYPE(Texture)
	private:
		void LoadTexture(const std::filesystem::path& filePath);

		uint32_t m_TextureID;
		int m_Width, m_Height, m_Channels;
		uint32_t m_InternalFormat, m_DataFormat;
		std::filesystem::path m_Path;

		TextureParameters m_TexParameters;

		friend class TextureAssetLoader;
	};
}