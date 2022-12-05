#pragma once

#include <stdint.h>

namespace TerranEngine 
{
	class UniformBuffer 
	{
	public:
		UniformBuffer(uint32_t bufferSize, uint16_t bindingPoint);
		UniformBuffer(uint32_t bufferSize, uint16_t bindingPoint, uint32_t offset, uint32_t size);
		~UniformBuffer();

		void SetData(const void* data, uint32_t offset, uint32_t size);
		void SetData(const void* newData, uint32_t size);

		void Bind() const;
		void Unbind() const;

	private:
		uint32_t m_Buffer;
	};
}
