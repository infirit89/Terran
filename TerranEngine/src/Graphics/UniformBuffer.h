#pragma once

#include <stdint.h>

namespace TerranEngine 
{
	class UniformBuffer 
	{
	public:
		UniformBuffer(uint32_t bufferSize, uint32_t bindingPoint);
		~UniformBuffer();

		static Shared<UniformBuffer> Create(uint32_t size, uint32_t bindingPoint);

		void SetData(const void* data, uint32_t offset, uint32_t size);
		//void SetData(const void* newData, uint32_t size);

	private:
		void Release();

		uint32_t m_Handle;
	};
}
