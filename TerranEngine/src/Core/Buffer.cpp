#include "trpch.h"
#include "Buffer.h"

namespace TerranEngine
{
	Buffer::Buffer()
		: m_Data(nullptr), m_Size(0) { }

	void Buffer::Allocate(size_t size)
	{
		if (m_Data)
			Free();

		m_Data = new uint8_t[size];
		m_Size = size;
	}

	void Buffer::Free()
	{
		delete m_Data;
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

	Buffer Buffer::Copy(void* data, size_t size)
	{
		Buffer buffer;
		buffer.Allocate(size);
		memcpy(buffer.m_Data, data, size);
		return buffer;
	}
}
