#pragma once

#include <cstddef>
#include <cstdint>

namespace Terran::Core {

/**
 * @ingroup LibCore
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
     * @brief Constructs a buffer by copying raw data.
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
     * @return Buffer size in bytes
     */
    constexpr size_t size() const noexcept
    {
        return m_size;
    }

    /**
     * @brief Writes data into the buffer at the specified offset.
     *
     * @param data   Source data to write
     * @param offset Offset into the buffer
     * @param size   Number of bytes to write
     *
     */
    void write(void const* data, uint32_t offset, size_t size);

    /**
     * @brief Returns a pointer to the buffer at the given offset.
     *
     * @param offset Offset into the buffer
     * @return Pointer to the requested position in the buffer
     *
     * @warning
     * No bounds checking is performed.
     */
    constexpr uint8_t* read(uint32_t offset) const noexcept
    {
        return m_data + offset;
    }

    /**
     * @brief Checks whether the buffer contains valid data.
     *
     * @return `true` if the buffer is allocated and non-empty
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
