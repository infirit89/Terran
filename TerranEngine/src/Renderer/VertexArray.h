#pragma once

#include "Buffer.h"

#include <memory>

namespace TerranEngine 
{
	class VertexArray 
	{
	public:
		VertexArray();
		~VertexArray();

		const void Bind() const;
		const void Unbind() const;

		void AddVertexBufferLayout(const VertexBufferLayout& layout);
		const VertexBufferLayout& GetVertexBufferLayout() const { return m_Layout; }

		void AddVertexBuffer(const VertexBuffer& buffer) { m_VBuffer = buffer; }
		const VertexBuffer& GetVertexBuffer() const { return m_VBuffer; }

		void AddIndexBuffer(const IndexBuffer& buffer) { m_IBuffer = buffer; }
		const IndexBuffer& GetIndexBuffer() const { return m_IBuffer; }
	private:
		uint32_t m_Vao;

		VertexBuffer m_VBuffer;
		IndexBuffer m_IBuffer;
		VertexBufferLayout m_Layout;

		uint32_t m_ElementIndex;
	};
}