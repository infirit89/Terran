#pragma once

#include "ScriptObject.h"

#include <stdint.h>

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
		static GCHandle CreateWeakHandle(const ScriptObject& monoObject);
		static GCHandle CreateStrongHadle(const ScriptObject& monoObject);
		static void FreeHandle(GCHandle& handle);
		static MonoObject* GetManagedObject(const GCHandle& handle);
		static void CollectAll();
	};
}
