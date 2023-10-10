#include "trpch.h"
#include "Buffer.h"

namespace TerranEngine
{
	Buffer::Buffer()
		: m_Data(nullptr), m_Size(0) { }

	Buffer::Buffer(size_t size)
	{
		Allocate(size);
	}

	Buffer::Buffer(const void* data, size_t size)
		: m_Data((uint8_t*)data), m_Size(size) { }

	void Buffer::Allocate(size_t size)
	{
		if (m_Data)
			Free();

		m_Data = new uint8_t[size];
		m_Size = size;
	}

	void Buffer::Free()
	{
		delete[] m_Data;
		m_Data = nullptr;
		m_Size = 0;
	}

	Buffer Buffer::Copy(const Buffer& other)
	{
		Buffer buffer;
		buffer.Allocate(other.m_Size);
		memcpy(buffer.m_Data, other.m_Data, other.m_Size);
		return buffer;
	}

	Buffer Buffer::Copy(const void* data, size_t size)
	{
		Buffer buffer;
		buffer.Allocate(size);
		memcpy(buffer.m_Data, data, size);
		return buffer;
	}

	void Buffer::Write(const void* data, uint32_t offset, size_t size)
	{
		uint8_t* writeLocation = m_Data + offset;
		size_t dataSize = (size_t)((writeLocation + size) - m_Data);
		TR_ASSERT(dataSize <= m_Size, "Data size is too big");

		memcpy(writeLocation, data, size);
	}
}
