#include "trpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	namespace Utils 
	{
		void CreateBuffer(uint32_t* outID, uint32_t bufferType, uint32_t size, uint32_t usage, const void* data = nullptr)
		{
			glGenBuffers(1, outID);
			glBindBuffer(bufferType, *outID);
			glBufferData(bufferType, size, data, usage);
		}

		void ReleaseBuffer(uint32_t* inID) 
		{
			glDeleteBuffers(1, inID);
		}
	}

	/* ---- VertexBuffer ---- */
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		Utils::CreateBuffer(&m_BufferID, GL_ARRAY_BUFFER, size, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const float* vertices, uint32_t size)
	{
		Utils::CreateBuffer(&m_BufferID, GL_ARRAY_BUFFER, size, GL_STATIC_DRAW, vertices);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		Utils::ReleaseBuffer(&m_BufferID);
	}

	void OpenGLVertexBuffer::SetData(const void* vertices, uint32_t size)
	{
		Bind();
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);
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
		Utils::CreateBuffer(&m_BufferID, GL_ELEMENT_ARRAY_BUFFER, size, GL_STATIC_DRAW, indices);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		Utils::ReleaseBuffer(&m_BufferID);
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
