#include "Buffer.h"

#include "Assert.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace Terran {
namespace Core {

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
    if(!m_data)
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

void ByteBuffer::write(void const* data, uint32_t offset, size_t size)
{
    uint8_t* writeLocation = m_data + offset;
    size_t dataSize = (size_t)((writeLocation + size) - m_data);
    TR_ASSERT(dataSize <= m_size, "Data size is too big");

    memcpy(writeLocation, data, size);
}

}

}
