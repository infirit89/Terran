#pragma once

#include <stdint.h>
#include <vector>

#include "Texture.h"

namespace TerranEngine 
{
	struct FramebufferParameters
	{
		uint32_t Width = 1080, Height = 790;
		std::vector<TextureFormat> Attachments;
		uint32_t Samples = 1;
	};

	class Framebuffer 
	{
	public:
		Framebuffer(FramebufferParameters parameters);
		~Framebuffer();

		void Bind() const;
		void Unbind() const;

		void Resize(uint32_t width, uint32_t height);
		int ReadPixel(uint32_t colorAttachmentIndex, int x, int y);

		inline Shared<Texture2D> GetColorAttachment(uint32_t colorAttachmentIndex) { return m_ColorAttachments[colorAttachmentIndex]; }
		inline Shared<Texture2D> GetDepthAttachment() { return m_DepthAttachment; }
		
		uint32_t GetWidth() const { return m_Parameters.Width; }
		uint32_t GetHeight() const { return m_Parameters.Height; }

		void SetColorAttachmentValue(uint32_t colorAttachmentIndex, int value);

	private:
		void Create();

		uint32_t m_Handle;
		Shared<Texture2D> m_DepthAttachment;
		std::vector<Shared<Texture2D>> m_ColorAttachments;
		FramebufferParameters m_Parameters;
	};
}
