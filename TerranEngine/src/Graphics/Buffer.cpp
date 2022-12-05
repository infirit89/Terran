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
		glGenBuffers(1, &m_Buffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	VertexBuffer::VertexBuffer(const float* vertices, uint32_t size)
	{
		glGenBuffers(1, &m_Buffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_Buffer);
	}

	void VertexBuffer::SetData(const void* vertices, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);
	}

	const void VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
	}

	const void VertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	/* ----------------------- */


	/* ---- Index Buffer ---- */
	IndexBuffer::IndexBuffer(const int* indices, uint32_t size)
		: m_Size(size)
	{
		glGenBuffers(1, &m_Buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &m_Buffer);
	}

	const void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
	}

	const void IndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	/* ----------------------*/
}
