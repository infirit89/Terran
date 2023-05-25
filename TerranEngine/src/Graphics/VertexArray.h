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

		virtual void AddVertexBuffer(const Shared<VertexBuffer>& vertexBuffer, const VertexBufferLayout& layout) = 0;
		virtual void AddIndexBuffer(const Shared<IndexBuffer>& buffer) = 0;
	
		static Shared<VertexArray> Create();
	};
}
