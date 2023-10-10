#include "trpch.h"
#include "VertexBuffer.h"

#include "Renderer.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	/* ---- Vertex Buffer Element --- */
	uint8_t VertexBufferElement::GetSize()
	{
		switch (Type)
		{
		case VertexBufferElementType::Float:
		case VertexBufferElementType::Int:		return 4;
		default:			TR_ASSERT(false, "No other type supported!");
		}

		return 0;
	}
	
	/* ------------------------------*/

	/* ---- Vertex Buffer ---- */
	VertexBuffer::VertexBuffer(uint32_t size)
		: m_Handle(0)
	{
		Renderer::SubmitCreate([this, size]()
		{
			glCreateBuffers(1, &m_Handle);
			glNamedBufferStorage(m_Handle, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
		});
	}

	VertexBuffer::VertexBuffer(const float* vertices, uint32_t size)
		: m_Handle(0)
	{
		Renderer::SubmitCreate([this, size, vertices]()
		{
			glCreateBuffers(1, &m_Handle);
			glNamedBufferData(m_Handle, size, vertices, GL_STATIC_DRAW);
		});
	}

	Shared<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		return CreateShared<VertexBuffer>(size);
	}

	Shared<VertexBuffer> VertexBuffer::Create(const float* vertices, uint32_t size)
	{
		return CreateShared<VertexBuffer>(vertices, size);
	}

	VertexBuffer::~VertexBuffer()
	{
		Release();
	}

	void VertexBuffer::SetData(const void* vertices, uint32_t size)
	{
		Renderer::Submit([this, vertices, size]() 
		{
			glNamedBufferSubData(m_Handle, 0, size, vertices);
		});
	}

	void VertexBuffer::Release()
	{
		Renderer::SubmitFree([this]() 
		{
			glDeleteBuffers(1, &m_Handle);
		});
	}

	/* ----------------------- */
}
