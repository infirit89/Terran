#include "trpch.h"
#include "Framebuffer.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	Framebuffer::Framebuffer(FramebufferParameters params)
		: m_Handle(0), m_DepthAttachment(0), m_Width(params.Width), 
		m_Height(params.Height)
	{
		m_ColorAttachmentsParameters = params.ColorAttachemnts;
		m_DepthAttachmentParameter = params.DepthAttachment;

		Create();
	}

	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &m_Handle);

		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);

	}

	void Framebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);
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
		glClearTexImage(m_ColorAttachments[colorAttachmentIndex], 0, GL_RED_INTEGER, GL_INT, &value);
	}

	void Framebuffer::Create()
	{
		if (m_Handle) 
		{
			glDeleteFramebuffers(1, &m_Handle);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_Handle = 0;
			m_DepthAttachment = 0;
			m_ColorAttachments.clear();
		}

		glCreateFramebuffers(1, &m_Handle);

		m_ColorAttachments.resize(m_ColorAttachmentsParameters.size());
		glCreateTextures(GL_TEXTURE_2D, m_ColorAttachments.size(), m_ColorAttachments.data());

		for (size_t i = 0; i < m_ColorAttachments.size(); i++)
		{
			glTextureParameteri(m_ColorAttachments[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_ColorAttachments[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			switch (m_ColorAttachmentsParameters[i])
			{
			case FramebufferColorAttachmentType::RGBA: 
			{
				glTextureStorage2D(m_ColorAttachments[i], 1, GL_RGBA8, m_Width, m_Height);
				break;
			}
			case FramebufferColorAttachmentType::Red32Integer: 
			{
				glTextureStorage2D(m_ColorAttachments[i], 1, GL_R32I, m_Width, m_Height);
				break;
			}
			}

			glNamedFramebufferTexture(m_Handle, GL_COLOR_ATTACHMENT0 + i, m_ColorAttachments[i], 0);
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		glTextureStorage2D(m_DepthAttachment, 1, GL_DEPTH24_STENCIL8, m_Width, m_Height);
		
		glNamedFramebufferTexture(m_Handle, GL_DEPTH_ATTACHMENT, m_DepthAttachment, 0);
		
		if (m_ColorAttachments.size() > 1)
		{
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glNamedFramebufferDrawBuffers(m_Handle, m_ColorAttachments.size(), buffers);
		}

		if (glCheckNamedFramebufferStatus(m_Handle, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			TR_ASSERT(false, "Framebuffer isn't complete");
	}
}
