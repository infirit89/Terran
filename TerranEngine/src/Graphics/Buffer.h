#pragma once

#include "Core/Assert.h"

#include <memory>
#include <vector>
#include <initializer_list>

namespace TerranEngine 
{
	struct VertexBufferElement 
	{
		uint32_t Type;
		bool Normalised;
		uint8_t Count;
		uint32_t Offset;

		VertexBufferElement(uint32_t type, uint8_t count, bool normalised = false) 
			: Type(type), Normalised(normalised), Count(count), Offset(0) {}

		uint8_t GetSize();
	};

	class VertexBufferLayout 
	{
	public:
		VertexBufferLayout()
			: m_Stride(0) {}

		VertexBufferLayout(std::initializer_list<VertexBufferElement> initList)
			: m_Elements(initList), m_Stride(0) 
		{
			CalculateStrideAndOffset();
		}

		inline uint32_t GetStride() const { return m_Stride; }
		inline std::vector<VertexBufferElement> GetElements() const { return m_Elements; }
	private:
		void CalculateStrideAndOffset() 
		{
			uint32_t offset = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.GetSize() * element.Count;
				m_Stride += element.GetSize() * element.Count;
			}
		}

		std::vector<VertexBufferElement> m_Elements;
		uint32_t m_Stride;
	};

	class VertexBuffer 
	{
	public:
		VertexBuffer() 
			: m_Buffer(0) {}

		VertexBuffer(uint32_t size);
		VertexBuffer(const float* vertices, uint32_t size);

		~VertexBuffer();

		void SetData(const void* vertices, uint32_t size);
		const void Bind() const;
		const void Unbind() const;
	private:
		uint32_t m_Buffer;
	};

	class IndexBuffer 
	{
	public:
		IndexBuffer() 
			: m_Buffer(0), m_Size(0) {}

		IndexBuffer(const int* indices, uint32_t size);
		~IndexBuffer();

		const void Bind() const;
		const void Unbind() const;

		inline uint32_t GetCount() const { return m_Size / sizeof(uint32_t); }
	private:
		uint32_t m_Buffer;
		int m_Size;
	};
}