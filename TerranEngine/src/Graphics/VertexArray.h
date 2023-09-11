#pragma once

#include "Buffer.h"

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

		void AddVertexBuffer(const Shared<VertexBuffer>& buffer);
		inline const Shared<VertexBuffer>& GetVertexBuffer() const { return m_VertexBuffer; }

		void AddIndexBuffer(const Shared<IndexBuffer>& buffer);
		inline const Shared<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }
	private:
		uint32_t m_Handle;

		Shared<VertexBuffer> m_VertexBuffer;
		Shared<IndexBuffer> m_IndexBuffer;
		VertexBufferLayout m_Layout;

		uint32_t m_AttributeIndex;
	};
}
