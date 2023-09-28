#pragma once

#include "Core/Assert.h"

#include <memory>
#include <vector>
#include <initializer_list>

namespace TerranEngine 
{
	enum class VertexBufferElementType 
	{
		Float,
		Int
	};

	struct VertexBufferElement
	{
		VertexBufferElementType Type;
		bool Normalised;
		uint8_t Count;
		uint32_t Offset;

		VertexBufferElement(VertexBufferElementType type, uint8_t count, bool normalised = false)
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
			: m_Handle(0) {}

		VertexBuffer(uint32_t size);
		VertexBuffer(const float* vertices, uint32_t size);
		
		static Shared<VertexBuffer> Create(uint32_t size);
		static Shared<VertexBuffer> Create(const float* vertices, uint32_t size);

		~VertexBuffer();

		void SetData(const void* vertices, uint32_t size);
		void SetLayout(const VertexBufferLayout& layout) { m_Layout = layout; }
		const VertexBufferLayout& GetLayout() const { return m_Layout; }

	private:
		uint32_t m_Handle;
		VertexBufferLayout m_Layout;

		friend class VertexArray;
	};
}
