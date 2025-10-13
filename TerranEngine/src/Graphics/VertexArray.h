#pragma once

#include "IndexBuffer.h"
#include "LibCore/Base.h"
#include "VertexBuffer.h"

#include <cstdint>

namespace TerranEngine {

class VertexArray final {
public:
    VertexArray();
    ~VertexArray();

    void Bind();
    void Unbind();

    void AddVertexBuffer(Terran::Core::Shared<VertexBuffer> const& buffer);
    Terran::Core::Shared<VertexBuffer> const& GetVertexBuffer() const { return m_VertexBuffer; }

    void AddIndexBuffer(Terran::Core::Shared<IndexBuffer> const& buffer);
    Terran::Core::Shared<IndexBuffer> const& GetIndexBuffer() const { return m_IndexBuffer; }

private:
    void Release();

    uint32_t m_Handle;

    Terran::Core::Shared<VertexBuffer> m_VertexBuffer;
    Terran::Core::Shared<IndexBuffer> m_IndexBuffer;

    uint32_t m_AttributeIndex;
};

}
