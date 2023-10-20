#pragma once

#include "Assets/Asset.h"
#include "Core/Buffer.h"

#include <string>
#include <filesystem>
#include <stdint.h>

namespace TerranEngine
{
	enum class TextureFormat
	{
		Red = 0,
		Red32I,
		RGB,
		RGBA,
		Depth24Stencil8,
		RG
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
		TextureFormat Format = TextureFormat::RGBA;
		TextureFilter Filter = TextureFilter::Linear;
		TextureWrapMode WrapMode = TextureWrapMode::ClampToEdge;
		uint32_t Width;
		uint32_t Height;
		uint32_t Samples = 1;
	};

	class Texture : public Asset
	{
	public:
		
		virtual ~Texture() {};

		virtual void Bind(uint32_t textureSlot) = 0;
		//void Unbind() const;

		virtual void SetData(const Buffer& data) = 0;
		virtual void SetTextureFilter(TextureFilter filter) = 0;

		virtual const TextureParameters& GetTextureParameters() const = 0;
		virtual int GetWidth() const = 0;
		virtual const int GetHeight() const = 0;
		virtual const uint32_t GetHandle() const = 0;
		virtual const uint32_t GetSamples() const = 0;

		virtual bool operator==(Texture& other) = 0;
		virtual bool operator==(const Texture& other) = 0;
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D(TextureParameters parameters = {}, Buffer buffer = Buffer());
		static Shared<Texture2D> Create(TextureParameters parameters = {}, Buffer buffer = Buffer());

		virtual ~Texture2D() override;

		virtual void Bind(uint32_t textureSlot) override;
		
		virtual void SetData(const Buffer& data) override;

		virtual void SetTextureFilter(TextureFilter filter) override;

		virtual const TextureParameters& GetTextureParameters() const override 
		{
			return m_TextureParameters;
		}

		virtual int GetWidth() const override 
		{
			return m_TextureParameters.Width;
		}
		virtual const int GetHeight() const override 
		{
			return m_TextureParameters.Height; 
		}

		virtual const uint32_t GetHandle() const override { return m_Handle; }

		virtual const uint32_t GetSamples() const override 
		{
			return m_TextureParameters.Samples;
		}

		virtual bool operator==(Texture& other) override;
		virtual bool operator==(const Texture& other) override;

		void Release();
		ASSET_CLASS_TYPE(Texture2D)
	private:
		void CreateTexture();

		//void LoadTexture(const std::filesystem::path& filePath);
		Buffer m_LocalData;
		uint32_t m_Handle;
		TextureParameters m_TextureParameters;
		friend class TextureAssetLoader;
	};
}
