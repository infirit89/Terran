#pragma once

#include "Core/Assert.h"

#include <memory>
#include <vector>
#include <initializer_list>

namespace TerranEngine 
{

	struct BufferElementType
	{
		enum class Type
		{
			None = 0,
			FLOAT, 
			FLOAT2,
			FLOAT3,
			FLOAT4,
			INT, 
			INT2,
			INT3,
			BOOL,
		};

		static uint32_t GetSizeOfType(Type type) 
		{
			switch (type)
			{
			case Type::FLOAT:	return 4;
			case Type::FLOAT2:	return 4 * 2;
			case Type::FLOAT3:	return 4 * 3;
			case Type::FLOAT4:	return 4 * 4;
			case Type::INT:		return 4;
			case Type::INT2:	return 4 * 2;
			case Type::INT3:	return 4 * 3;
			case Type::BOOL:	return 1;
			default:			TR_ASSERT(false, "No other type supported!");
			}

			return 0;
		}
	};
	struct VertexBufferElement 
	{
		BufferElementType::Type Type;
		bool Normalised;
		uint32_t Offset;

		VertexBufferElement(BufferElementType::Type type, bool normalised = false) 
			: Type(type), Normalised(normalised), Offset(0) {}

		uint32_t GetCountOfType() 
		{
			switch (Type)
			{
			case BufferElementType::Type::FLOAT:	return 1;
			case BufferElementType::Type::FLOAT2:	return 2;
			case BufferElementType::Type::FLOAT3:	return 3;
			case BufferElementType::Type::FLOAT4:	return 4;
			case BufferElementType::Type::INT:		return 1;
			case BufferElementType::Type::INT2:		return 2;
			case BufferElementType::Type::INT3:		return 3;
			case BufferElementType::Type::BOOL:		return 1;
			default:								TR_ASSERT(false, "No other type supported!");
			}

			return 0;
		}
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

		uint32_t GetStride() { return m_Stride; }
		std::vector<VertexBufferElement> GetElements() { return m_Elements; }
	private:
		void CalculateStrideAndOffset() 
		{
			uint32_t offset = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += BufferElementType::GetSizeOfType(element.Type);
				m_Stride += BufferElementType::GetSizeOfType(element.Type);
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

		void SetLayout(const VertexBufferLayout& layout) { m_Layout = layout; }
		VertexBufferLayout GetLayout() { return m_Layout; }

		void SetData(const void* vertices, uint32_t size);
		const void Bind() const;
		const void Unbind() const;
	private:
		uint32_t m_Buffer;
		VertexBufferLayout m_Layout;
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

		uint32_t GetCount() const { return m_Size / sizeof(uint32_t); }
	private:
		uint32_t m_Buffer;
		int m_Size;
	};
}