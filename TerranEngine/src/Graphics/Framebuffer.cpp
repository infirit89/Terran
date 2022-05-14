#include "trpch.h"
#include "Framebuffer.h"

#include "RenderCommand.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	Framebuffer::Framebuffer(FramebufferParameters params)
		: m_ClearTextureData(nullptr), m_Buffer(0), m_DepthAttachment(0), m_Width(params.Width), 
		m_Height(params.Height)
	{
		m_ColorAttachmentsParameters = params.ColorAttachemnts;
		m_DepthAttachmentParameter = params.DepthAttachment;

		Create();
	}

	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &m_Buffer);

		if (m_ClearTextureData)
			free(m_ClearTextureData);

		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);

	}

	void Framebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_Buffer);
		glViewport(0, 0, m_Width, m_Height);
	}

	void Framebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;

		if (m_ClearTextureData)
			free(m_ClearTextureData);

		m_ClearTextureData = (int*)malloc(m_Width * m_Height * sizeof(int));
		memset(m_ClearTextureData, -1, m_Width * m_Height * sizeof(int));

		Create();
	}

	int Framebuffer::ReadPixel(uint32_t colorAttachmentIndex, int x, int y)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0 + colorAttachmentIndex);
		int pixel;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel);
		return pixel;
	}

	void Framebuffer::SetColorAttachment(uint32_t colorAttachmentIndex, int value)
	{
		if (RenderCommand::GetAPIVersion() < 450) 
		{
			// this is so so so fucking slow
			glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[colorAttachmentIndex]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, GL_RED_INTEGER, GL_INT, m_ClearTextureData);
		}
		else 
			glClearTexImage(m_ColorAttachments[colorAttachmentIndex], 0, GL_RED_INTEGER, GL_INT, &value);
	}

	void Framebuffer::Create()
	{
		if (m_Buffer) 
		{
			glDeleteFramebuffers(1, &m_Buffer);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_Buffer = 0;
			m_DepthAttachment = 0;
			m_ColorAttachments.clear();
		}

		glGenFramebuffers(1, &m_Buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_Buffer);

		m_ColorAttachments.resize(m_ColorAttachmentsParameters.size());
		glGenTextures(m_ColorAttachments.size(), m_ColorAttachments.data());

		for (size_t i = 0; i < m_ColorAttachments.size(); i++)
		{
			glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			switch (m_ColorAttachmentsParameters[i])
			{
			case FramebufferColorAttachmentType::RGBA: 
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
				break;
			}
			case FramebufferColorAttachmentType::Red32Integer: 
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, m_Width, m_Height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
				break;
			}
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_ColorAttachments[i], 0);
		}

		glGenTextures(1, &m_DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

		if (m_ColorAttachments.size() > 1)
		{
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			TR_ASSERT(false, "Framebuffer isn't complete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
