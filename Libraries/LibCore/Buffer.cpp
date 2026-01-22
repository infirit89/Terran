#include "Buffer.h"
#include "Result.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace Terran::Core {

constexpr ByteBuffer::ByteBuffer() noexcept
    : m_data(nullptr)
    , m_size(0)
{
}

ByteBuffer::ByteBuffer(size_t size)
{
    allocate(size);
}

ByteBuffer::ByteBuffer(void const* data, size_t size)
    : m_data((uint8_t*)data)
    , m_size(size)
{
}

void ByteBuffer::allocate(size_t size)
{
    if (m_data)
        free();

    m_data = new uint8_t[size];
    m_size = size;
}

void ByteBuffer::free()
{
    if (!m_data)
        return;

    delete[] m_data;
    m_data = nullptr;
    m_size = 0;
}

ByteBuffer ByteBuffer::Copy(ByteBuffer const& other)
{
    ByteBuffer buffer;
    buffer.allocate(other.m_size);
    memcpy(buffer.m_data, other.m_data, other.m_size);
    return buffer;
}

ByteBuffer ByteBuffer::Copy(void const* data, size_t size)
{
    ByteBuffer buffer;
    buffer.allocate(size);
    memcpy(buffer.m_data, data, size);
    return buffer;
}

Result<void, ByteBufferError> ByteBuffer::write(void const* data, size_t offset, size_t size)
{
    uint8_t* writeLocation = m_data + offset;
    if(offset + size > m_size)
        return { ByteBufferError::Overflow };

    memcpy(writeLocation, data, size);

    return {};
}

}
