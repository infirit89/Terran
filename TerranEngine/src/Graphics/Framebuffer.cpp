#include "trpch.h"
#include "Framebuffer.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	static constexpr size_t s_MaxSupportedColorAttachments = 4;
	static GLenum s_Buffers[s_MaxSupportedColorAttachments] =
	{
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3
	};

	Framebuffer::Framebuffer(FramebufferParameters parameters)
		: m_Handle(0), m_Parameters(parameters)
	{
		Create();
	}

	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &m_Handle);
	}

	void Framebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);
		glViewport(0, 0, m_Parameters.Width, m_Parameters.Height);
	}

	void Framebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Parameters.Width = width;
		m_Parameters.Height = height;

		Create();
	}

	int Framebuffer::ReadPixel(uint32_t colorAttachmentIndex, int x, int y)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0 + colorAttachmentIndex);
		int pixel;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel);
		return pixel;
	}

	void Framebuffer::SetColorAttachmentValue(uint32_t colorAttachmentIndex, int value)
	{
		glClearTexImage(m_ColorAttachments[colorAttachmentIndex]->GetHandle(), 0, 
						GL_RED_INTEGER, GL_INT, &value);
	}

	void Framebuffer::Create()
	{
		if (m_Handle) 
		{
			glDeleteFramebuffers(1, &m_Handle);
			
			m_Handle = 0;
			m_DepthAttachment = nullptr;
			m_ColorAttachments.clear();
		}

		glCreateFramebuffers(1, &m_Handle);

		// NOTE: even though the attachments may include a depth attachment
		// we reserse it to avoid extra allocation;
		// also its just a vector of shared pointers it shouldn't be that big of a deal
		m_ColorAttachments.reserve(m_Parameters.Attachments.size());
		
		int colorAttachmentIndex = 0;
		for (const auto& attachmentFormat : m_Parameters.Attachments)
		{
			TextureParameters attachmentParameters;
			attachmentParameters.Format = attachmentFormat;
			attachmentParameters.Width = m_Parameters.Width;
			attachmentParameters.Height = m_Parameters.Height;
			attachmentParameters.Samples = m_Parameters.Samples;

			Shared<Texture2D> attachment = CreateShared<Texture2D>(attachmentParameters);

			if (attachmentFormat == TextureFormat::Depth24Stencil8) 
			{
				m_DepthAttachment = attachment;
				glNamedFramebufferTexture(m_Handle, GL_DEPTH_ATTACHMENT, 
										attachment->GetHandle(), 0);
			}
			else
			{
				m_ColorAttachments.push_back(attachment);
				glNamedFramebufferTexture(m_Handle,
										GL_COLOR_ATTACHMENT0 + colorAttachmentIndex,
										attachment->GetHandle(), 0);

				colorAttachmentIndex++;
			}
		}

		if (m_ColorAttachments.size() > 1)
			glNamedFramebufferDrawBuffers(m_Handle, static_cast<int>(m_ColorAttachments.size()), s_Buffers);

		if (glCheckNamedFramebufferStatus(m_Handle, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			TR_ASSERT(false, "Framebuffer isn't complete");
	}
}
