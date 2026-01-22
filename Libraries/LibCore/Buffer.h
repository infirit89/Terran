/**
 * @file Buffer.h
 * @brief Byte buffer.
 * @ingroup LibCore
 */
#pragma once

#include "Result.h"

#include <cstddef>
#include <cstdint>

namespace Terran::Core {

/**
 * @class ByteBuffer
 * @brief Owning, contiguous byte buffer.
 *
 * @details
 * `ByteBuffer` represents a simple owning buffer of raw bytes.
 * It is intended for low-level memory operations such as serialization,
 * file I/O, and binary data manipulation.
 *
 * The buffer:
 * - Owns its memory
 * - Uses manual allocation and deallocation
 * - Stores bytes contiguously
 *
 * @note
 * This class performs no bounds checking. The caller is responsible
 * for ensuring valid offsets and sizes when reading or writing.
 */
enum class ByteBufferError : uint8_t {
    OutOfBounds = 0,
    Overflow
};

class ByteBuffer final {
public:
    /**
     * @brief Constructs an empty buffer.
     */
    constexpr ByteBuffer() noexcept;

    /**
     * @brief Destroys the buffer and frees owned memory.
     */
    constexpr ~ByteBuffer() noexcept
    {
        free();
    }

    /**
     * @brief Constructs a buffer with the specified size.
     *
     * @param size Number of bytes to allocate
     */
    ByteBuffer(size_t size);

    /**
     * @brief Constructs a buffer from raw data, DOES NOT COPY.
     *
     * @param data Source data to copy from
     * @param size Number of bytes to copy
     */
    ByteBuffer(void const* data, size_t size);

    /**
     * @brief Allocates memory for the buffer.
     *
     * @details
     * Any previously allocated memory is released before allocation.
     *
     * @param size Number of bytes to allocate
     */
    void allocate(size_t size);

    /**
     * @brief Frees the allocated buffer memory.
     *
     * @details
     * After calling this function, the buffer becomes empty.
     */
    void free();

    /**
     * @brief Creates a copy of another ByteBuffer.
     *
     * @param other Buffer to copy from
     * @return New ByteBuffer containing copied data
     */
    static ByteBuffer Copy(ByteBuffer const& other);

    /**
     * @brief Creates a ByteBuffer by copying raw data.
     *
     * @param data Source data to copy from
     * @param size Number of bytes to copy
     * @return New ByteBuffer containing copied data
     */
    static ByteBuffer Copy(void const* data, size_t size);

    /**
     * @brief Returns a pointer to the buffer data.
     *
     * @return Pointer to the internal byte data
     */
    constexpr uint8_t const* data() const noexcept
    {
        return m_data;
    }

    /**
     * @brief Returns the size of the buffer in bytes.
     *
     * @return Buffer size in bytes.
     */
    constexpr size_t size() const noexcept
    {
        return m_size;
    }

    /**
     * @brief Writes data into the buffer at the specified offset.
     *
     * @param data   Source data to write.
     * @param offset Offset into the buffer.
     * @param size   Number of bytes to write.
     *
     * @retval ByteBuffer::Overflow If the write would result in an overflow.
     * @return Empty result on success.
     */
    Result<void, ByteBufferError> write(void const* data, size_t offset, size_t size);

    /**
     * @brief Returns a pointer to the buffer at the given offset.
     *
     * @param offset Offset into the buffer.
     * @retval uint8_t* Pointer to the buffer at @p offset on success.
     * @retval ByteBufferError::OutOfBounds when the read would result in an access violation.
     */
    Result<uint8_t*, ByteBufferError> read(size_t offset) const noexcept
    {
        if (offset >= m_size)
            return { ByteBufferError::OutOfBounds };

        return { m_data + offset };
    }

    /**
     * @brief Checks whether the buffer contains valid data.
     *
     * @return `true` if the buffer is allocated and non-empty.
     */
    constexpr operator bool() const noexcept
    {
        return m_data != nullptr && m_size != 0;
    }

private:
    uint8_t* m_data = nullptr;
    size_t m_size;
};

}
