#include "trpch.h"
#include "UniformBuffer.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	UniformBuffer::UniformBuffer(uint32_t bufferSize, uint16_t bindingPoint)
		: m_Handle(0)
	{
		glCreateBuffers(1, &m_Handle);
		glNamedBufferStorage(m_Handle, bufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_Handle);
	}

	UniformBuffer::~UniformBuffer() { glDeleteBuffers(1, &m_Handle); }

	void UniformBuffer::SetData(const void* data, uint32_t offset, uint32_t size)
	{
		glNamedBufferSubData(m_Handle, offset, size, data);
	}

	/*void UniformBuffer::SetData(const void* newData, uint32_t size)
	{
		void* data = glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE);

		memcpy(data, newData, size);
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}*/
}
