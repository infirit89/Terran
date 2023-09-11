#pragma once

#include <stdint.h>

namespace TerranEngine 
{
	class UniformBuffer 
	{
	public:
		UniformBuffer(uint32_t bufferSize, uint16_t bindingPoint);
		~UniformBuffer();

		void SetData(const void* data, uint32_t offset, uint32_t size);
		//void SetData(const void* newData, uint32_t size);

	private:
		uint32_t m_Handle;
	};
}
