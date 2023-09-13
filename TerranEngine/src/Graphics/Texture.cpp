#include "trpch.h"
#include "Texture.h"

#include "Utils/Debug/Profiler.h"

#include <glad/glad.h>
#include <stb_image.h>

#include <filesystem>

namespace TerranEngine 
{
	struct NativeTexutreType
	{
		uint32_t InternalFormat;
		uint32_t DataFormat;
	};

	static NativeTexutreType GetNativeTextureType(TextureType type)
	{
		switch (type)
		{
		case TerranEngine::TextureType::Red:	return { GL_R8, GL_RED };
		case TerranEngine::TextureType::Red32I:	return { GL_R32I, GL_RED_INTEGER };
		case TerranEngine::TextureType::RGB:	return { GL_RGB8, GL_RGB };
		case TerranEngine::TextureType::RGBA:	return { GL_RGBA8, GL_RGBA };
		default:
			TR_WARN("The texture type isn't supported");
			break;
		}

		return { GL_RGBA8, GL_RGBA };
	}

	struct NativeTexutreFilter
	{
		uint32_t MinFilter;
		uint32_t MagFilter;
	};

	static NativeTexutreFilter GetNativeTextureFilter(TextureFilter minFilter, TextureFilter magFilter) 
	{
		NativeTexutreFilter filter = { GL_LINEAR, GL_LINEAR };

		switch (minFilter)
		{
		case TerranEngine::TextureFilter::Linear:
		case TerranEngine::TextureFilter::Nearest:
			filter.MinFilter = GL_LINEAR - (uint32_t)minFilter;
			break;
		default:
			TR_WARN("The texture filter isn't supported");
			break;
		}

		switch (magFilter)
		{
		case TerranEngine::TextureFilter::Linear:
		case TerranEngine::TextureFilter::Nearest:
			filter.MagFilter = GL_LINEAR - (uint32_t)magFilter;
			break;
		default:
			TR_WARN("The texture filter isn't supported");
			break;
		}

		return filter;
	}

	static uint32_t GetNativeWrapMode(TextureWrapMode wrapMode) 
	{
		switch (wrapMode)
		{
		case TerranEngine::TextureWrapMode::Repeat:			return GL_REPEAT;
		case TerranEngine::TextureWrapMode::Mirror:			return GL_MIRRORED_REPEAT;
		case TerranEngine::TextureWrapMode::MirrorOnce:		return GL_MIRROR_CLAMP_TO_EDGE;
		case TerranEngine::TextureWrapMode::ClampToEdge:	return GL_CLAMP_TO_EDGE;
		default:
			TR_WARN("The texture wrap mode isn't supported");
			break;
		}

		return { GL_CLAMP_TO_EDGE };
	}

	Texture2D::Texture2D(uint32_t width, uint32_t height, TextureParameters parameters) 
		: m_Handle(0), m_Width(width), m_Height(height), m_TextureParameters(parameters)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);
		
		NativeTexutreFilter filter = GetNativeTextureFilter(
														m_TextureParameters.MinFilter,
														m_TextureParameters.MagFilter);

		glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, filter.MagFilter);
		glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, filter.MinFilter);

		uint32_t wrapMode = GetNativeWrapMode(m_TextureParameters.WrapMode);

		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, wrapMode);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, wrapMode);

		NativeTexutreType nativeType = GetNativeTextureType(m_TextureParameters.TextureType);
		glTextureStorage2D(m_Handle, 1, nativeType.InternalFormat, m_Width, m_Height);
	}

	Texture2D::Texture2D(const std::filesystem::path& filePath, TextureParameters parameters)
		: m_Handle(0), m_Width(0), m_Height(0)
	{
		LoadTexture(filePath);
	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &m_Handle);
	}

	void Texture2D::Bind(uint8_t textureSlot)
	{	 
		glBindTextureUnit(textureSlot, m_Handle);
	}	 
		 
	//void Texture::Unbind() const
	//{	 
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//}	 

	void Texture2D::SetData(const void* data) 
	{
		NativeTexutreType nativeType = GetNativeTextureType(m_TextureParameters.TextureType);
		glTextureSubImage2D(m_Handle, 0, 0, 0, m_Width, m_Height,
							nativeType.DataFormat, GL_UNSIGNED_BYTE, data);
	}

	bool Texture2D::operator==(Texture& other) 
	{
		return m_Handle == other.GetHandle();
	}

	bool Texture2D::operator==(const Texture& other) 
	{
		return m_Handle == other.GetHandle();
	}

	void Texture2D::LoadTexture(const std::filesystem::path& filePath)
	{
		stbi_set_flip_vertically_on_load(1);

		int channels;
		stbi_uc* pixels = stbi_load(filePath.string().c_str(), 
									&m_Width, &m_Height, &channels, 0);

		if (pixels == nullptr)
		{
			TR_ERROR("Couldn't load the texture with path: {0}", filePath);
			return;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);
		
		uint32_t internalFormat, dataFormat;
		switch (channels)
		{
			case 4: 
			{
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
				break;
			}
			case 3: 
			{
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
				break;
			}
			case 2: 
			{
				internalFormat = GL_RG8;
				dataFormat = GL_RG;

				// NOTE: for grayscale images
				/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_GREEN);*/
				break;
			}
			default: TR_ASSERT(false, "{} channels is not a supported data format", channels);
		}

		NativeTexutreFilter filter = GetNativeTextureFilter(
														m_TextureParameters.MinFilter,
														m_TextureParameters.MagFilter);

		glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, filter.MinFilter);
		glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, filter.MagFilter);

		uint32_t wrapMode = GetNativeWrapMode(m_TextureParameters.WrapMode);

		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, wrapMode);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, wrapMode);

		glTextureStorage2D(m_Handle, 1, internalFormat, m_Width, m_Height);
		glTextureSubImage2D(m_Handle, 0, 0, 0, m_Width, m_Height,
							dataFormat, GL_UNSIGNED_BYTE, pixels);

		stbi_image_free(pixels);
	}
}
