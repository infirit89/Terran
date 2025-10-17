#pragma once

#include <cstddef>
#include <cstdint>

namespace Terran {
namespace Core {

// NOTE: this is an owning byte buffer
class ByteBuffer final {
public:
    constexpr ByteBuffer() noexcept;
    constexpr ~ByteBuffer() noexcept
    {
        free();
    }

    ByteBuffer(size_t size);
    ByteBuffer(void const* data, size_t size);

    void allocate(size_t size);
    void free();

    static ByteBuffer Copy(ByteBuffer const& other);
    static ByteBuffer Copy(void const* data, size_t size);

    constexpr uint8_t const* data() const noexcept
    {
        return m_data;
    }

    constexpr size_t size() const noexcept
    {
        return m_size;
    }

    void write(void const* data, uint32_t offset, size_t size);

    constexpr uint8_t* read(uint32_t offset) const noexcept
    {
        return m_data + offset;
    }

    constexpr operator bool() const noexcept
    {
        return m_data != nullptr && m_size != 0;
    }

private:
    uint8_t* m_data = nullptr;
    size_t m_size;
};

}

}
