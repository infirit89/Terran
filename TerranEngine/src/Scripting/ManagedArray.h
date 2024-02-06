#pragma once

#include <cstdint>
#include <stdlib.h>
#include <memory.h>
#include <algorithm>

namespace TerranEngine
{
	template<typename TValue>
	class ManagedArray 
	{
	public:
		static ManagedArray New(int32_t length) 
		{
			ManagedArray<TValue> result;
			if (length > 0)
			{
				result.m_Ptr = static_cast<TValue*>(malloc(length * sizeof(TValue)));
				result.m_Length = length;
			}

			return result;
		}

		static void Free(ManagedArray& array) 
		{
			if (!array.m_Ptr || array.m_Length == 0)
				return;

			delete array.m_Ptr;
			array.m_Ptr = nullptr;
			array.m_Length = 0;
		}

		static void Resize(ManagedArray& array, int32_t length)
		{
			if (!array.m_Ptr || array.m_Length == 0 || length <= 0)
				return;

			ManagedArray<TValue> temp = ManagedArray<TValue>::New(length);
			memcpy(temp.m_Ptr, array.m_Ptr, std::min(length, array.m_Length) * sizeof(TValue));
			array.m_Ptr = temp.m_Ptr;
			array.m_Length = temp.m_Length;
		}

		bool IsEmpty() const { return m_Length == 0 || m_Ptr == nullptr; }

		TValue& operator[](size_t index) { return m_Ptr[index]; }
		const TValue& operator[](size_t index) const { return m_Ptr[index]; }

		size_t Length() const { return m_Length; }
		TValue* Data() { return m_Ptr; }
		const TValue* Data() const { return m_Ptr; }

		TValue* begin() { return m_Ptr; }
		TValue* end() { return m_Ptr + m_Length; }

	private:
		TValue* m_Ptr = nullptr;
		int32_t m_Length = 0;
	};
}
