#pragma once

#include <stdint.h>

typedef struct _MonoObject MonoObject;

namespace TerranEngine 
{
	enum class GCHandleType
	{
		None = 0,
		Weak,
		Strong
	};

	class GCHandle
	{
	public:
		GCHandle() = default;
		GCHandle(const GCHandle& other) = default;
		GCHandle(uint32_t handle, GCHandleType handleType);
		~GCHandle() = default;

		inline bool IsValid() const { return m_Handle != 0; }
		inline uint32_t GetHandle() const { return m_Handle; }
		inline GCHandleType GetType() const { return m_HandleType; }

	private:
		uint32_t m_Handle;
		GCHandleType m_HandleType;

		friend class GCManager;
	};

	class GCManager 
	{
	public:
		static GCHandle CreateWeakHandle(MonoObject* monoObject);
		static GCHandle CreateStrongHadle(MonoObject* monoObject);
		static void FreeHandle(GCHandle& handle);
		static MonoObject* GetMonoObject(const GCHandle& handle);
		static void CollectAll();
	};
}
