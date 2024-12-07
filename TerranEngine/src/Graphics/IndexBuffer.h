#pragma once

#include "Core/Buffer.h"

#include <memory>

namespace TerranEngine 
{
	class IndexBuffer 
	{
	public:
		IndexBuffer() 
			: m_Handle(0), m_Size(0) {}

		IndexBuffer(const int* indices, uint32_t size);
		~IndexBuffer();

		static Shared<IndexBuffer> Create(const int* indices, uint32_t size);

		uint32_t GetCount() const { return m_Size / sizeof(uint32_t); }
	private:
		void Release();

	private:
		uint32_t m_Handle;
		int m_Size;
		Buffer m_LocalData;

		friend class VertexArray;
	};
}
