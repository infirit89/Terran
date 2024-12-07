#pragma once

#include "Asset/Asset.h"
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
		Grayscale
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

	struct TextureParameters final
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
		~Texture() override {}

		virtual void Bind(uint32_t textureSlot) = 0;
		//void Unbind() const;

		virtual void SetData(const Buffer& data) = 0;
		virtual void SetTextureFilter(TextureFilter filter) = 0;

		virtual const TextureParameters& GetTextureParameters() const = 0;
		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;
		virtual uint32_t GetHandle() const = 0;
		virtual uint32_t GetSamples() const = 0;

		virtual bool operator==(Texture& other) = 0;
		virtual bool operator==(const Texture& other) = 0;
	};

	class Texture2D final : public Texture
	{
	public:
		Texture2D(const TextureParameters& parameters = {}, Buffer buffer = Buffer());
		static Shared<Texture2D> Create(TextureParameters parameters = {}, Buffer buffer = Buffer());

		~Texture2D() override;

		void Bind(uint32_t textureSlot) override;
		
		void SetData(const Buffer& data) override;

		void SetTextureFilter(TextureFilter filter) override;

		const TextureParameters& GetTextureParameters() const override 
		{
			return m_TextureParameters;
		}

		int GetWidth() const override 
		{
			return m_TextureParameters.Width;
		}
		int GetHeight() const override 
		{
			return m_TextureParameters.Height; 
		}

		uint32_t GetHandle() const override { return m_Handle; }

		uint32_t GetSamples() const override 
		{
			return m_TextureParameters.Samples;
		}

		bool operator==(Texture& other) override;
		bool operator==(const Texture& other) override;

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
