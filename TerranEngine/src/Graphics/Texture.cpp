#include "trpch.h"
#include "Texture.h"

#include "Utils/Debug/Profiler.h"

#include "Renderer.h"

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

	Texture2D::Texture2D(TextureParameters parameters, Buffer buffer)
		: m_TextureParameters(parameters), m_Handle(0)
	{
		TR_ASSERT(m_TextureParameters.Width >= 1 || m_TextureParameters.Height >= 1, "Width and Height can't be less than 1");
		TR_ASSERT(m_TextureParameters.Samples > 0, "Samples cant be less than 1");

		CreateTexture();

		if (buffer)
			SetData(buffer);
	}

	Shared<Texture2D> Texture2D::Create(TextureParameters parameters, Buffer buffer)
	{
		return CreateShared<Texture2D>(parameters, buffer);
	}

	Texture2D::~Texture2D()
	{
		Release();
	}

	void Texture2D::Bind(uint32_t textureSlot)
	{	 
		Renderer::Submit([this, textureSlot]()
		{
			if (m_Handle == 0)
				TR_ASSERT(false, "Handle is 0");
			glBindTextureUnit(textureSlot, m_Handle);
		});
	}	 
		 
	//void Texture::Unbind() const
	//{	 
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//}	 

	void Texture2D::SetData(const Buffer& data) 
	{
		m_LocalData = Buffer::Copy(data);
		Renderer::Submit([this]()
		{
			NativeTexutreType nativeType = GetNativeTextureType(m_TextureParameters.Format);
			glTextureSubImage2D(m_Handle, 0, 0, 0,
								m_TextureParameters.Width, m_TextureParameters.Height,
								nativeType.DataFormat, GL_UNSIGNED_BYTE, m_LocalData.GetData());
		});
	}

	void Texture2D::SetTextureFilter(TextureFilter filter)
	{
		Renderer::Submit([this, filter]()
		{
			uint32_t nativeFilter = GetNativeTextureFilter(filter);
			m_TextureParameters.Filter = filter;
			glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, nativeFilter);
			glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, nativeFilter);
		});
	}

	bool Texture2D::operator==(Texture& other) 
	{
		return m_Handle == other.GetHandle();
	}

	bool Texture2D::operator==(const Texture& other) 
	{
		return m_Handle == other.GetHandle();
	}

	void Texture2D::CreateTexture()
	{
		Renderer::SubmitCreate([textureParameters = m_TextureParameters, this]()
		{
			if (textureParameters.Samples > 1)
			{
				glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_Handle);
			}
			else
			{
				glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);

				uint32_t filter = GetNativeTextureFilter(textureParameters.Filter);

				glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, filter);
				glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, filter);

				uint32_t wrapMode = GetNativeWrapMode(textureParameters.WrapMode);

				glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, wrapMode);
				glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, wrapMode);
			}

			NativeTexutreType nativeType = GetNativeTextureType(textureParameters.Format);


			if (textureParameters.Samples > 1)
			{
				glTextureStorage2DMultisample(m_Handle, textureParameters.Samples, nativeType.InternalFormat,
					textureParameters.Width, textureParameters.Height, false);
			}
			else
			{
				glTextureStorage2D(m_Handle, 1, nativeType.InternalFormat,
					textureParameters.Width, textureParameters.Height);
			}
		});
	}

	void Texture2D::Release()
	{
		if (m_LocalData)
			m_LocalData.Free();

		Renderer::SubmitFree([this]()
		{
			glDeleteTextures(1, &m_Handle);
		});
	}
}
