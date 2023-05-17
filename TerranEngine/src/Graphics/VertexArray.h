#pragma once

#include "Buffer.h"

namespace TerranEngine 
{
	class VertexArray 
	{
	public:
		virtual ~VertexArray() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void AddVertexBufferLayout(const VertexBufferLayout& layout) = 0;

		inline void AddVertexBuffer(const Shared<VertexBuffer>& buffer) { m_VertexBuffer = buffer; }
		inline const Shared<VertexBuffer>& GetVertexBuffer() const { return m_VertexBuffer; }

		void AddIndexBuffer(const Shared<IndexBuffer>& buffer) { m_IndexBuffer = buffer; }
		inline const Shared<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }
	
		static Shared<VertexArray> Create();

	private:
		Shared<VertexBuffer> m_VertexBuffer;
		Shared<IndexBuffer> m_IndexBuffer;
	};
}
