#include "trpch.h"
#include "IndexBuffer.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	IndexBuffer::IndexBuffer(const int* indices, uint32_t size)
		: m_Size(size)
	{
		glCreateBuffers(1, &m_Handle);
		glNamedBufferData(m_Handle, size, indices, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &m_Handle);
	}

	Shared<IndexBuffer> IndexBuffer::Create(const int* indices, uint32_t size)
	{
		return CreateShared<IndexBuffer>(indices, size);
	}
}
