#include "VertexBuffer.h"
#include "trpch.h"

#include "Renderer.h"

#include <glad/glad.h>

namespace TerranEngine {

/* ---- Vertex Buffer Element --- */
uint8_t VertexBufferElement::GetSize() const
{
    switch (Type) {
    case VertexBufferElementType::Float:
    case VertexBufferElementType::Int:
        return 4;
    default:
        TR_ASSERT(false, "No other type supported!");
    }

    return 0;
}

/* ------------------------------*/

/* ---- Vertex Buffer ---- */
VertexBuffer::VertexBuffer(uint32_t size)
    : m_Handle(0)
    , m_LocalData(size)
{
    Renderer::SubmitCreate([this, size]() {
        glCreateBuffers(1, &m_Handle);
        glNamedBufferStorage(m_Handle, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
    });
}

VertexBuffer::VertexBuffer(float const* vertices, uint32_t size)
    : m_Handle(0)
{
    m_LocalData = Terran::Core::Buffer::Copy(vertices, size);
    Renderer::SubmitCreate([this]() {
        glCreateBuffers(1, &m_Handle);
        glNamedBufferData(m_Handle, m_LocalData.GetSize(), m_LocalData.GetData(), GL_STATIC_DRAW);
    });
}

Terran::Core::Shared<VertexBuffer> VertexBuffer::Create(uint32_t size)
{
    return Terran::Core::CreateShared<VertexBuffer>(size);
}

Terran::Core::Shared<VertexBuffer> VertexBuffer::Create(float const* vertices, uint32_t size)
{
    return Terran::Core::CreateShared<VertexBuffer>(vertices, size);
}

VertexBuffer::~VertexBuffer()
{
    if (m_LocalData)
        m_LocalData.Free();

    Release();
}

void VertexBuffer::SetData(void const* vertices, uint32_t size)
{
    m_LocalData.Write(vertices, 0, size);
    Renderer::Submit([this]() {
        glNamedBufferSubData(m_Handle, 0, m_LocalData.GetSize(), m_LocalData.GetData());
    });
}

void VertexBuffer::Release()
{
    Renderer::SubmitFree([this]() {
        glDeleteBuffers(1, &m_Handle);
    });
}

/* ----------------------- */

}
