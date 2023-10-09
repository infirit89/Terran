#pragma once

namespace TerranEngine 
{
	class Buffer 
	{
	public:
		Buffer();

		void Allocate(size_t size);
		void Free();

		static Buffer Copy(const Buffer& other);
		static Buffer Copy(void* data, size_t size);
		const uint8_t* GetData() const { return m_Data; }
		size_t GetSize() const { return m_Size; }

		operator bool() const { return m_Data != nullptr && m_Size != 0; }
	private:
		uint8_t* m_Data = nullptr;
		size_t m_Size;
	};
}
