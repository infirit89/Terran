#pragma once

namespace TerranEngine 
{
	class Buffer 
	{
	public:
		Buffer();
		Buffer(size_t size);
		Buffer(const void* data, size_t size);

		void Allocate(size_t size);
		void Free();

		static Buffer Copy(const Buffer& other);
		static Buffer Copy(const void* data, size_t size);
		const uint8_t* GetData() const { return m_Data; }
		size_t GetSize() const { return m_Size; }
		void Write(const void* data, uint32_t offset, size_t size);
		uint8_t* Read(uint32_t offset) { return m_Data + offset; }

		operator bool() const { return m_Data != nullptr && m_Size != 0; }
	private:
		uint8_t* m_Data = nullptr;
		size_t m_Size;
	};
}
