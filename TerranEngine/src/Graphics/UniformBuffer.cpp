#include "UniformBuffer.h"
#include "trpch.h"

#include "Renderer.h"

#include <glad/glad.h>

namespace TerranEngine {

UniformBuffer::UniformBuffer(uint32_t bufferSize, uint32_t bindingPoint)
    : m_LocalData(bufferSize)
    , m_Handle(0)
{
    Renderer::SubmitCreate([this, bufferSize, bindingPoint]() {
        glCreateBuffers(1, &m_Handle);
        glNamedBufferStorage(m_Handle, bufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_Handle);
    });
}

UniformBuffer::~UniformBuffer()
{
    if (m_LocalData)
        m_LocalData.Free();

    Release();
}

Terran::Core::Shared<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t bindingPoint)
{
    return Terran::Core::CreateShared<UniformBuffer>(size, bindingPoint);
}

void UniformBuffer::SetData(void const* data, uint32_t offset, uint32_t size)
{
    m_LocalData.Write(data, offset, size);
    Renderer::Submit([this, offset, size]() {
        glNamedBufferSubData(m_Handle, offset, size, m_LocalData.Read(offset));
    });
}

void UniformBuffer::Release()
{
    Renderer::SubmitFree([this]() {
        glDeleteBuffers(1, &m_Handle);
    });
}

/*void UniformBuffer::SetData(const void* newData, uint32_t size)
{
        void* data = glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE);

        memcpy(data, newData, size);
        glUnmapBuffer(GL_UNIFORM_BUFFER);
}*/

}
