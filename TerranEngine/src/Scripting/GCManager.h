#pragma once

#include <stdint.h>

extern "C"
{
	typedef struct _MonoObject MonoObject;
}

namespace TerranEngine 
{
	class ScriptObject;
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

		inline uint32_t GetHandle() const { return m_Handle; }
		inline GCHandleType GetType() const { return m_HandleType; }
		inline operator bool() const { return m_Handle != 0; }

	private:
		uint32_t m_Handle = 0;
		GCHandleType m_HandleType = GCHandleType::None;

		friend class GCManager;
	};

	class GCManager 
	{
	public:
		// with a weak ref the gc can still collect the object;
		// if track resurrection then if the object is resurrected during the finalizer
		// (stage of the gc) then the target object will still be active
		static GCHandle CreateWeakHandle(const ScriptObject& monoObject, bool trackResurrection);

		// prevents the gc from deleting the object until free handle is called;
		// pinned prevents the object from being moved
		static GCHandle CreateStrongHandle(const ScriptObject& monoObject, bool pinned = false);
		static void FreeHandle(GCHandle& handle);
		static MonoObject* GetManagedObject(const GCHandle& handle);
		static void CollectAll();
	};
}
