#pragma once

#include <stdint.h>

#include <vector>

namespace TerranEngine 
{
	enum class FramebufferColorAttachmentType
	{
		None = 0,
		RGBA,
		Red32Integer
	};

	enum class FramebufferDepthAttachmentType
	{
		None = 0,
		Depth24Stencil8
	};

	struct FramebufferParameters
	{
		uint32_t Width = 1080, Height = 790;
		std::vector<FramebufferColorAttachmentType> ColorAttachemnts;
		FramebufferDepthAttachmentType DepthAttachment;
	};

	class Framebuffer 
	{
	public:
		Framebuffer(FramebufferParameters params);
		~Framebuffer();

		void Bind() const;
		void Unbind() const;

		void Resize(uint32_t width, uint32_t height);
		int ReadPixel(uint32_t colorAttachmentIndex, int x, int y);

		inline uint32_t GetColorAttachmentID(uint32_t colorAttachmentIndex) { return m_ColorAttachments[colorAttachmentIndex]; }
		
		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		void SetColorAttachment(uint32_t colorAttachmentIndex, int value);

	private:
		void Create();

		uint32_t m_Width, m_Height;
		uint32_t m_Handle;
		uint32_t m_DepthAttachment;
		std::vector<uint32_t> m_ColorAttachments;
		
		std::vector<FramebufferColorAttachmentType> m_ColorAttachmentsParameters;
		FramebufferDepthAttachmentType m_DepthAttachmentParameter;
	};
}
