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
		inline const VertexBufferLayout& GetVertexBufferLayout() const { return m_Layout; }

		inline void AddVertexBuffer(const Shared<VertexBuffer>& buffer) { m_VBuffer = buffer; }
		inline const Shared<VertexBuffer> GetVertexBuffer() const { return m_VBuffer; }

		void AddIndexBuffer(const Shared<IndexBuffer>& buffer) { m_IBuffer = buffer; }
		inline const Shared<IndexBuffer> GetIndexBuffer() const { return m_IBuffer; }
	private:
		uint32_t m_Vao;

		Shared<VertexBuffer> m_VBuffer;
		Shared<IndexBuffer> m_IBuffer;
		VertexBufferLayout m_Layout;

		uint32_t m_ElementIndex;
	};
}