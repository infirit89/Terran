#include "trpch.h"

#include "IndexBuffer.h"
#include "Renderer.h"

#include <glad/glad.h>

namespace TerranEngine {

IndexBuffer::IndexBuffer(int const* indices, uint32_t size)
    : m_Handle(0)
    , m_Size(size)
{
    m_LocalData = Terran::Core::Buffer::Copy(indices, size);
    Renderer::SubmitCreate([this, size]() {
        glCreateBuffers(1, &m_Handle);
        glNamedBufferData(m_Handle, size, m_LocalData.GetData(), GL_STATIC_DRAW);
    });
}

IndexBuffer::~IndexBuffer()
{
    if (m_LocalData)
        m_LocalData.Free();

    Release();
}

Terran::Core::Shared<IndexBuffer> IndexBuffer::Create(int const* indices, uint32_t size)
{
    return Terran::Core::CreateShared<IndexBuffer>(indices, size);
}

void IndexBuffer::Release()
{
    Renderer::SubmitFree([this]() {
        glDeleteBuffers(1, &m_Handle);
    });
}

}
