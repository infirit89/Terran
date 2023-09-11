#include "trpch.h"
#include "Buffer.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	/* ---- Vertex Buffer Element --- */
	uint8_t VertexBufferElement::GetSize()
	{
		switch (Type)
		{
		case GL_FLOAT:
		case GL_INT:		return 4;
		case GL_BOOL:		return 1;
		default:			TR_ASSERT(false, "No other type supported!");
		}

		return 0;
	}
	/* ------------------------------*/

	/* ---- Vertex Buffer ---- */
	VertexBuffer::VertexBuffer(uint32_t size)
	{
		glCreateBuffers(1, &m_Handle);
		glNamedBufferStorage(m_Handle, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
	}

	VertexBuffer::VertexBuffer(const float* vertices, uint32_t size)
	{
		glCreateBuffers(1, &m_Handle);
		glNamedBufferData(m_Handle, size, vertices, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_Handle);
	}

	void VertexBuffer::SetData(const void* vertices, uint32_t size)
	{
		glNamedBufferSubData(m_Handle, 0, size, vertices);
	}

	/* ----------------------- */


	/* ---- Index Buffer ---- */
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

	/* ----------------------*/
}
