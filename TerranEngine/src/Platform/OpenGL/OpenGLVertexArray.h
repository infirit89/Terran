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

		virtual void AddVertexBufferLayout(const VertexBufferLayout& layout) override;

	private:
		uint32_t m_ArrayID;
		uint32_t m_ElementIndex;
	};
}
