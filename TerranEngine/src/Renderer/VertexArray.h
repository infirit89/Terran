#pragma once

#include "Buffer.h"

namespace TerranEngine 
{
	class VertexArray 
	{
	public:
		VertexArray();
		~VertexArray();

		void Bind();
		void Unbind();

		void AddVertexBuffer(VertexBuffer& buffer);
		const VertexBuffer& GetVertexBuffer() const { return m_VBuffer; }

		void AddIndexBuffer(IndexBuffer& buffer) { m_IBuffer = buffer; }
		const IndexBuffer& GetIndexBuffer() const { return m_IBuffer; }
	private:
		unsigned int m_Vao;

		VertexBuffer m_VBuffer;
		IndexBuffer m_IBuffer;

		uint32_t m_ElementIndex;
	};
}