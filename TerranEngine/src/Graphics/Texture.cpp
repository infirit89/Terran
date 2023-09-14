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

	/*struct NativeTexutreFilter
	{
		uint32_t MinFilter;
		uint32_t MagFilter;
	};*/

	static uint32_t GetNativeTextureFilter(TextureFilter filter) 
	{
		uint32_t nativeFilter = GL_LINEAR;

		switch (filter)
		{
		case TerranEngine::TextureFilter::Linear:
		case TerranEngine::TextureFilter::Nearest:
			nativeFilter = GL_LINEAR - (uint32_t)filter;
			break;
		default:
			TR_WARN("The texture filter isn't supported");
			break;
		}

		return nativeFilter;
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
		
		uint32_t filter = GetNativeTextureFilter(m_TextureParameters.Filter);

		glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, filter);
		glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, filter);

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

	void Texture2D::SetTextureFilter(TextureFilter filter)
	{
		uint32_t nativeFilter = GetNativeTextureFilter(filter);
		m_TextureParameters.Filter = filter;
		glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, nativeFilter);
		glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, nativeFilter);
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

		uint32_t nativeFilter = GetNativeTextureFilter(m_TextureParameters.Filter);

		glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, nativeFilter);
		glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, nativeFilter);

		uint32_t wrapMode = GetNativeWrapMode(m_TextureParameters.WrapMode);

		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, wrapMode);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, wrapMode);

		glTextureStorage2D(m_Handle, 1, internalFormat, m_Width, m_Height);
		glTextureSubImage2D(m_Handle, 0, 0, 0, m_Width, m_Height,
							dataFormat, GL_UNSIGNED_BYTE, pixels);

		stbi_image_free(pixels);
	}

	const char* TextureFilterToString(TextureFilter filter)
	{
		switch (filter)
		{
		case TextureFilter::Linear:		return "Linear";
		case TextureFilter::Nearest:	return "Nearest";
		}

		TR_ASSERT(false, "Unknown texture filter");
		return "";
	}
	TextureFilter TextureFilterFromString(const std::string& filterString)
	{
		if (filterString == "Linear")	return TextureFilter::Linear;
		if (filterString == "Nearest")	return TextureFilter::Nearest;

		TR_ASSERT(false, "Unknown texture filter");
		return TextureFilter::Linear;
	}
}
