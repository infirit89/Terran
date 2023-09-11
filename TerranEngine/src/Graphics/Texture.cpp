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

	static NativeTexutreType ConvertTextureTypeToNativeType(TextureType type)
	{
		switch (type)
		{
		case TerranEngine::TextureType::Red:			return { GL_R8, GL_RED };
		case TerranEngine::TextureType::Red32Integer:	return { GL_R32I, GL_RED_INTEGER };
		case TerranEngine::TextureType::RGB:			return { GL_RGB8, GL_RGB };
		case TerranEngine::TextureType::RGBA:			return { GL_RGBA8, GL_RGBA };
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

	static NativeTexutreFilter ConvertTextureFilterToNativeFilter(TextureFilter minFilter, TextureFilter magFilter) 
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

	static uint32_t ConvertTextureWrapModeToOpenGLWrapMode(TextureWrapMode wrapMode) 
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

	Texture::Texture() 
		: m_Handle(0), m_Width(0), m_Height(0), m_Channels(0), m_InternalFormat(0), m_DataFormat(0), m_Path("") {}
	
	Texture::Texture(uint32_t width, uint32_t height, TextureParameters parameters) 
		: m_Handle(0), m_Width(width), m_Height(height), m_Channels(0), 
		m_InternalFormat(0), m_DataFormat(0), m_TexParameters(parameters)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);
		
		NativeTexutreFilter filter = ConvertTextureFilterToNativeFilter(m_TexParameters.MinFilter, m_TexParameters.MagFilter);

		glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, filter.MagFilter);
		glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, filter.MinFilter);

		uint32_t wrapMode = ConvertTextureWrapModeToOpenGLWrapMode(m_TexParameters.WrapMode);

		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, wrapMode);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, wrapMode);

		NativeTexutreType nativeType = ConvertTextureTypeToNativeType(m_TexParameters.TextureType);
		glTextureStorage2D(m_Handle, 1, nativeType.InternalFormat, m_Width, m_Height);
	}

	Texture::Texture(const std::filesystem::path& filePath, TextureParameters parameters)
		: m_Handle(0), m_Width(0), m_Height(0), m_Channels(0), m_InternalFormat(0), m_DataFormat(0), m_Path(filePath)
	{
		LoadTexture(filePath);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_Handle);
	}

	void Texture::Bind(uint8_t textureSlot) const
	{	 
		glBindTextureUnit(textureSlot, m_Handle);
	}	 
		 
	void Texture::Unbind() const
	{	 
		glBindTexture(GL_TEXTURE_2D, 0);
	}	 

	void Texture::SetData(const void* data) 
	{
		NativeTexutreType nativeType = ConvertTextureTypeToNativeType(m_TexParameters.TextureType);
		glTextureSubImage2D(m_Handle, 0, 0, 0, m_Width, m_Height, nativeType.DataFormat, GL_UNSIGNED_BYTE, data);
	}

	bool Texture::operator==(Texture& other) 
	{
		return m_Width == other.m_Width && m_Height == other.m_Height
			&& m_Channels == other.m_Channels && m_Handle == other.m_Handle;
	}

	bool Texture::operator==(const Texture& other) 
	{
		return m_Width == other.m_Width && m_Height == other.m_Height
			&& m_Channels == other.m_Channels && m_Handle == other.m_Handle;
	}

	void Texture::LoadTexture(const std::filesystem::path& filePath)
	{
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* pixels = stbi_load(filePath.string().c_str(), &m_Width, &m_Height, &m_Channels, 0);

		if (pixels == nullptr)
		{
			TR_ERROR("Couldn't load the texture with path: {0}", filePath);
			return;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);
		
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
			case 2: 
			{
				m_InternalFormat = GL_RG8;
				m_DataFormat = GL_RG;

				// NOTE: for grayscale images
				/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_GREEN);*/
				break;
			}
			default: TR_TRACE(m_Channels); TR_ASSERT(false, "No other data format supported!");
		}

		NativeTexutreFilter filter = ConvertTextureFilterToNativeFilter(m_TexParameters.MinFilter, m_TexParameters.MagFilter);

		glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, filter.MinFilter);
		glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, filter.MagFilter);

		uint32_t wrapMode = ConvertTextureWrapModeToOpenGLWrapMode(m_TexParameters.WrapMode);

		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, wrapMode);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, wrapMode);

		glTextureStorage2D(m_Handle, 1, m_InternalFormat, m_Width, m_Height);
		glTextureSubImage2D(m_Handle, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, pixels);

		stbi_image_free(pixels);
	}
}
