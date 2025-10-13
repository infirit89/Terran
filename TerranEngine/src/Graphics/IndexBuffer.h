#pragma once

#include "LibCore/Base.h"
#include "LibCore/Buffer.h"

#include <cstdint>

namespace TerranEngine {

class IndexBuffer {
public:
    IndexBuffer()
        : m_Handle(0)
        , m_Size(0)
    {
    }

    IndexBuffer(int const* indices, uint32_t size);
    ~IndexBuffer();

    static Terran::Core::Shared<IndexBuffer> Create(int const* indices, uint32_t size);

    uint32_t GetCount() const { return m_Size / sizeof(uint32_t); }

private:
    void Release();

private:
    uint32_t m_Handle;
    int m_Size;
    Terran::Core::Buffer m_LocalData;

    friend class VertexArray;
};

}
