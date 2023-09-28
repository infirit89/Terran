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

	static NativeTexutreType GetNativeTextureType(TextureFormat type)
	{
		switch (type)
		{
		case TextureFormat::Red:				return { GL_R8, GL_RED };
		case TextureFormat::Red32I:				return { GL_R32I, GL_RED_INTEGER };
		case TextureFormat::RGB:				return { GL_RGB8, GL_RGB };
		case TextureFormat::RGBA:				return { GL_RGBA8, GL_RGBA };
		case TextureFormat::Depth24Stencil8:	return { GL_DEPTH24_STENCIL8, GL_DEPTH24_STENCIL8 };
		default:
			TR_WARN("The texture type isn't supported");
			break;
		}

		return { GL_RGBA8, GL_RGBA };
	}

	static uint32_t GetNativeTextureFilter(TextureFilter filter) 
	{
		uint32_t nativeFilter = GL_LINEAR;

		switch (filter)
		{
		case TextureFilter::Linear:
		case TextureFilter::Nearest:
			nativeFilter = GL_LINEAR - (uint32_t)filter;
			break;
		default:
			TR_WARN("The texture filter isn't supported");
			break;
		}

		return nativeFilter;
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

	static uint32_t GetNativeWrapMode(TextureWrapMode wrapMode) 
	{
		switch (wrapMode)
		{
		case TextureWrapMode::Repeat:			return GL_REPEAT;
		case TextureWrapMode::Mirror:			return GL_MIRRORED_REPEAT;
		case TextureWrapMode::MirrorOnce:		return GL_MIRROR_CLAMP_TO_EDGE;
		case TextureWrapMode::ClampToEdge:		return GL_CLAMP_TO_EDGE;
		default:
			TR_WARN("The texture wrap mode isn't supported");
			break;
		}

		return { GL_CLAMP_TO_EDGE };
	}

	Texture2D::Texture2D(TextureParameters parameters, const void* data)
		: m_TextureParameters(parameters)
	{
		TR_ASSERT(m_TextureParameters.Samples > 0, "Samples cant be less than 1");

		if (m_TextureParameters.Samples > 1) 
		{
			glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_Handle);
		}
		else 
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);

			uint32_t filter = GetNativeTextureFilter(m_TextureParameters.Filter);

			glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, filter);
			glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, filter);

			uint32_t wrapMode = GetNativeWrapMode(m_TextureParameters.WrapMode);

			glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, wrapMode);
			glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, wrapMode);
		}
		
		NativeTexutreType nativeType = GetNativeTextureType(m_TextureParameters.Format);

		if (m_TextureParameters.Samples > 1)
		{
			glTextureStorage2DMultisample(m_Handle, m_TextureParameters.Samples, nativeType.InternalFormat, 
											m_TextureParameters.Width, m_TextureParameters.Height, false);
		}
		else 
		{
			glTextureStorage2D(m_Handle, 1, nativeType.InternalFormat, 
								m_TextureParameters.Width, m_TextureParameters.Height);
		}

		if (data)
			SetData(data);
	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &m_Handle);
	}

	void Texture2D::Bind(uint32_t textureSlot)
	{	 
		glBindTextureUnit(textureSlot, m_Handle);
	}	 
		 
	//void Texture::Unbind() const
	//{	 
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//}	 

	void Texture2D::SetData(const void* data) 
	{
		NativeTexutreType nativeType = GetNativeTextureType(m_TextureParameters.Format);
		glTextureSubImage2D(m_Handle, 0, 0, 0, 
							m_TextureParameters.Width, m_TextureParameters.Height,
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
}
