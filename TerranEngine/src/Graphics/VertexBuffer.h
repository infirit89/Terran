#pragma once

#include "LibCore/Base.h"
#include "LibCore/Buffer.h"

#include <cstdint>
#include <initializer_list>
#include <vector>

namespace TerranEngine {

enum class VertexBufferElementType : std::uint8_t {
    Float,
    Int
};

struct VertexBufferElement final {
    VertexBufferElementType Type;
    bool Normalised;
    uint8_t Count;
    uint32_t Offset;

    VertexBufferElement(VertexBufferElementType type, uint8_t count, bool normalised = false)
        : Type(type)
        , Normalised(normalised)
        , Count(count)
        , Offset(0)
    {
    }

    uint8_t GetSize() const;
};

class VertexBufferLayout final {
public:
    VertexBufferLayout()
        : m_Stride(0)
    {
    }

    VertexBufferLayout(std::initializer_list<VertexBufferElement> initList)
        : m_Elements(initList)
        , m_Stride(0)
    {
        CalculateStrideAndOffset();
    }

    uint32_t GetStride() const { return m_Stride; }
    std::vector<VertexBufferElement> GetElements() const { return m_Elements; }

private:
    void CalculateStrideAndOffset()
    {
        uint32_t offset = 0;
        for (auto& element : m_Elements) {
            element.Offset = offset;
            offset += element.GetSize() * element.Count;
            m_Stride += element.GetSize() * element.Count;
        }
    }

    std::vector<VertexBufferElement> m_Elements;
    uint32_t m_Stride;
};

class VertexBuffer final {
public:
    VertexBuffer()
        : m_Handle(0)
    {
    }

    VertexBuffer(uint32_t size);
    VertexBuffer(float const* vertices, uint32_t size);

    static Terran::Core::Shared<VertexBuffer> Create(uint32_t size);
    static Terran::Core::Shared<VertexBuffer> Create(float const* vertices, uint32_t size);

    ~VertexBuffer();

    void SetData(void const* vertices, uint32_t size);
    void SetLayout(VertexBufferLayout const& layout) { m_Layout = layout; }
    VertexBufferLayout const& GetLayout() const { return m_Layout; }

private:
    void Release();

    uint32_t m_Handle;
    VertexBufferLayout m_Layout;
    Terran::Core::Buffer m_LocalData;

    friend class VertexArray;
};

}
