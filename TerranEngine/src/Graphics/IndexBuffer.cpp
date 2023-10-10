#include "trpch.h"

#include "IndexBuffer.h"
#include "Renderer.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	IndexBuffer::IndexBuffer(const int* indices, uint32_t size)
		: m_Size(size), m_Handle(0)
	{
		m_LocalData = new int[size];
		memcpy(m_LocalData, indices, size);

		Renderer::SubmitCreate([this, size]() 
		{
			glCreateBuffers(1, &m_Handle);
			glNamedBufferData(m_Handle, size, m_LocalData, GL_STATIC_DRAW);
		});
	}

	IndexBuffer::~IndexBuffer()
	{
		Release();
	}

	Shared<IndexBuffer> IndexBuffer::Create(const int* indices, uint32_t size)
	{
		return CreateShared<IndexBuffer>(indices, size);
	}

	void IndexBuffer::Release()
	{
		Renderer::SubmitFree([this]()
		{
			glDeleteBuffers(1, &m_Handle);
		});
	}
}
