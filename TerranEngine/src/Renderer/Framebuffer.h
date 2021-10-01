#pragma once

#include <stdint.h>

namespace TerranEngine 
{
	class Framebuffer 
	{
	public:
		Framebuffer(uint32_t width, uint32_t height, bool SwapChainTarget);
		~Framebuffer();

		void Bind() const;
		void Unbind() const;

		void BindColorAttachment() const;
		void UnbindColorAttachment() const;

		void Resize(uint32_t width, uint32_t height);

		inline bool IsSwapChainTarget() { return m_SwapChainTarget; }

		inline uint32_t GetColorAttachmentID() { return m_ColorAttachment; }

		uint32_t Width, Height;
	private:
		void Create();

		uint32_t m_Buffer, m_ColorAttachment, m_DepthAttachment;
		bool m_SwapChainTarget;
	};
}