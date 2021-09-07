#pragma once

#include <stdint.h>

namespace TerranEngine 
{
	class Framebuffer 
	{
	public:
		~Framebuffer();

		void Bind() const;
		void Unbind() const;

		void Resize();
	private:
		void Create();

		uint32_t m_Buffer;
	};
}