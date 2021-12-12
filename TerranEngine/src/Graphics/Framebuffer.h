#pragma once

#include <stdint.h>

namespace TerranEngine 
{
	class Framebuffer 
	{
	public:
		Framebuffer(uint32_t width, uint32_t height);
		~Framebuffer();

		void Bind() const;
		void Unbind() const;

		void BindColorAttachment() const;
		void UnbindColorAttachment() const;

		void Resize(uint32_t width, uint32_t height);

		inline uint32_t GetColorAttachmentID() { return m_ColorAttachment; }

		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }

	private:
		void Create();

		uint32_t m_Width, m_Height;
		uint32_t m_Buffer, m_ColorAttachment, m_DepthAttachment;
	};
}