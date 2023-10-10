#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace TerranEngine 
{
	class VertexArray 
	{
	public:
		VertexArray();
		~VertexArray();

		void Bind();
		void Unbind();

		void AddVertexBuffer(const Shared<VertexBuffer>& buffer);
		inline const Shared<VertexBuffer>& GetVertexBuffer() const { return m_VertexBuffer; }

		void AddIndexBuffer(const Shared<IndexBuffer>& buffer);
		inline const Shared<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }
	private:
		void Release();

		uint32_t m_Handle;

		Shared<VertexBuffer> m_VertexBuffer;
		Shared<IndexBuffer> m_IndexBuffer;

		uint32_t m_AttributeIndex;
	};
}
