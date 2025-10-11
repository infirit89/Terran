#pragma once

#include <cstddef>
#include <cstdint>

namespace Terran {
namespace Core {

class Buffer final {
public:
    Buffer();
    Buffer(size_t size);
    Buffer(void const* data, size_t size);

    void Allocate(size_t size);
    void Free();

    static Buffer Copy(Buffer const& other);
    static Buffer Copy(void const* data, size_t size);
    uint8_t const* GetData() const { return m_Data; }
    size_t GetSize() const { return m_Size; }
    void Write(void const* data, uint32_t offset, size_t size);
    uint8_t* Read(uint32_t offset) const { return m_Data + offset; }

    operator bool() const { return m_Data != nullptr && m_Size != 0; }

private:
    uint8_t* m_Data = nullptr;
    size_t m_Size;
};

}

}
