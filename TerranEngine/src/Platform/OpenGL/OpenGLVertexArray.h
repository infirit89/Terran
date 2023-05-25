#pragma once

#include "Graphics/VertexArray.h"

namespace TerranEngine 
{
	class OpenGLVertexArray : public VertexArray 
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray() override;

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void AddVertexBuffer(const Shared<VertexBuffer>& vertexBuffer, const VertexBufferLayout& layout) override;
		virtual void AddIndexBuffer(const Shared<IndexBuffer>& buffer) override;

	private:
		uint32_t m_ArrayID;
		uint32_t m_ElementIndex;
	};
}
