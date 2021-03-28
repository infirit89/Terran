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

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer);
		const std::shared_ptr<VertexBuffer>& GetVertexBuffer() const { return m_VBuffer; }

		void AddIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer) { m_IBuffer = buffer; }
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IBuffer; }
	private:
		uint32_t m_Vao;

		std::shared_ptr<VertexBuffer> m_VBuffer;
		std::shared_ptr<IndexBuffer> m_IBuffer;

		uint32_t m_ElementIndex;
	};
}