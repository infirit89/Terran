#pragma once

#include "Core/Assert.h"
#include "Core/Base.h"

#include <memory>
#include <vector>
#include <initializer_list>

namespace TerranEngine 
{
	enum class ShaderDataType 
	{
		Float,
		Int,
		Bool
	};

	struct VertexBufferElement 
	{
		ShaderDataType Type;
		bool Normalised;
		uint8_t Count;
		uint32_t Offset;

		VertexBufferElement(ShaderDataType type, uint8_t count, bool normalised = false) 
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
		inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
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
		virtual ~VertexBuffer() = default;

		virtual void SetData(const void* vertices, uint32_t size) = 0;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		static Shared<VertexBuffer> Create(uint32_t size);
		static Shared<VertexBuffer> Create(const float* vertices, uint32_t size);
	};

	class IndexBuffer 
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		inline uint32_t GetCount() const { return m_Size / sizeof(uint32_t); }

		static Shared<IndexBuffer> Create(const int* indices, uint32_t size);

	protected:
		uint32_t m_Size;
	};
}