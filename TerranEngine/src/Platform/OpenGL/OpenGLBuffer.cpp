#include "trpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace TerranEngine 
{

	/* ---- VertexBuffer ---- */
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
		: m_Size(size)
	{
		glCreateBuffers(1, &m_BufferID);
		glNamedBufferStorage(m_BufferID, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const float* vertices, uint32_t size)
	{
		glCreateBuffers(1, &m_BufferID);
		glNamedBufferData(m_BufferID, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	void OpenGLVertexBuffer::SetData(const void* vertices, uint32_t size)
	{
		glNamedBufferSubData(m_BufferID, 0, size, vertices);
	}

	void OpenGLVertexBuffer::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	}

	void OpenGLVertexBuffer::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0	);
	}
	/* ---------------------- */

	/* ---- Index Buffer ---- */
	OpenGLIndexBuffer::OpenGLIndexBuffer(const int* indices, uint32_t size)
	{
		m_Size = size;
		glCreateBuffers(1, &m_BufferID);
		glNamedBufferData(m_BufferID, size, indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}

	void OpenGLIndexBuffer::Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	}

	// NOTE: do we need unbind
	void OpenGLIndexBuffer::Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	/* ---------------------- */
}
