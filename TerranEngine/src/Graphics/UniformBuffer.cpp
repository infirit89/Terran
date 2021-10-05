#include "trpch.h"
#include "UniformBuffer.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	UniformBuffer::UniformBuffer(uint32_t bufferSize, uint16_t bindingPoint)
		: m_Buffer(0)
	{
		glGenBuffers(1, &m_Buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer);
		glBufferData(GL_UNIFORM_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_Buffer);
	}

	UniformBuffer::UniformBuffer(uint32_t bufferSize, uint16_t bindingPoint, uint32_t offset, uint32_t size)
		: m_Buffer(0)
	{
		glGenBuffers(1, &m_Buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer);
		glBufferData(GL_UNIFORM_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, m_Buffer, offset, size);
	}

	UniformBuffer::~UniformBuffer()
	{
		glDeleteBuffers(1, &m_Buffer);
	}

	void UniformBuffer::SetData(const void* data, uint32_t offset, uint32_t size)
	{	 
		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	}

	void UniformBuffer::SetData(const void* newData, uint32_t size)
	{
		Bind();

		void* data = glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE);

		memcpy(data, newData, size);
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}
		 
	void UniformBuffer::Bind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer);
	}	 
		 
	void UniformBuffer::Unbind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}

