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
		Red32I,
		RGB,
		RGBA
	};

	enum class TextureFilter
	{
		Linear = 0,
		Nearest
	};

	const char* TextureFilterToString(TextureFilter filter);
	TextureFilter TextureFilterFromString(const std::string& filterString);

	enum class TextureWrapMode
	{
		Repeat = 0,
		Mirror,
		MirrorOnce,
		ClampToEdge
	};

	struct TextureParameters 
	{
		// TODO: put width and height into the texture parameters
		TextureType TextureType = TextureType::RGBA;
		/*TextureFilter MinFilter = TextureFilter::Linear;
		TextureFilter MagFilter = TextureFilter::Linear;*/
		TextureFilter Filter = TextureFilter::Linear;
		TextureWrapMode WrapMode = TextureWrapMode::ClampToEdge;
	};

	class Texture : public Asset
	{
	public:
		
		virtual ~Texture() {};

		virtual void Bind(uint8_t textureSlot) = 0;
		//void Unbind() const;

		virtual void SetData(const void* data) = 0;
		virtual void SetTextureFilter(TextureFilter filter) = 0;

		virtual const TextureParameters& GetTextureParameters() const = 0;
		virtual int GetWidth() const = 0;
		virtual const int GetHeight() const = 0;
		virtual const uint32_t GetHandle() const = 0;

		virtual bool operator==(Texture& other) = 0;
		virtual bool operator==(const Texture& other) = 0;
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D(uint32_t width, uint32_t height, TextureParameters parameters = {});
		Texture2D(const std::filesystem::path& filePath, TextureParameters parameters = {});

		virtual ~Texture2D() override;

		virtual void Bind(uint8_t textureSlot) override;
		
		virtual void SetData(const void* data) override;

		virtual void SetTextureFilter(TextureFilter filter) override;

		virtual const TextureParameters& GetTextureParameters() const override 
		{
			return m_TextureParameters;
		}

		virtual int GetWidth() const override { return m_Width; }
		virtual const int GetHeight() const override { return m_Height; }
		virtual const uint32_t GetHandle() const override { return m_Handle; }

		virtual bool operator==(Texture& other) override;
		virtual bool operator==(const Texture& other) override;

		ASSET_CLASS_TYPE(Texture2D)
	private:
		void LoadTexture(const std::filesystem::path& filePath);

		uint32_t m_Handle;
		int m_Width, m_Height;

		TextureParameters m_TextureParameters;
		friend class TextureAssetLoader;
	};
}
