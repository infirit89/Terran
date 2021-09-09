#include "trpch.h"
#include "Framebuffer.h"

#include "glad/glad.h"

namespace TerranEngine 
{
	Framebuffer::Framebuffer(uint32_t width, uint32_t height, bool SwapChainTarget)
		: m_Buffer(0), m_ColorAttachment(0), m_DepthAttachment(0), Width(width), Height(height), m_SwapChainTarget(SwapChainTarget)
	{
		Create();
	}

	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &m_Buffer);
	}

	void Framebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_Buffer);
	}

	void Framebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::BindColorAttachment() const
	{
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
	}

	void Framebuffer::UnbindColorAttachment() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		Width = width;
		Height = height;

		glViewport(0, 0, Width, Height);

		Create();
	}

	void Framebuffer::Create()
	{
		if (m_Buffer) 
		{
			glDeleteFramebuffers(1, &m_Buffer);
			glDeleteTextures(1, &m_ColorAttachment);
			glDeleteTextures(1, &m_DepthAttachment);
		}

		glGenFramebuffers(1, &m_Buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_Buffer);

		glGenTextures(1, &m_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

		glGenTextures(1, &m_DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, Width, Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			TR_ASSERT(false, "Framebuffer isn't complete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
