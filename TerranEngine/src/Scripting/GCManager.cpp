#include "trpch.h"
#include "GCManager.h"

#include "ScriptObject.h"

#include "Utils/Debug/OptickProfiler.h"

#include <mono/metadata/mono-gc.h>
#include <mono/metadata/object.h>

namespace TerranEngine 
{
    GCHandle::GCHandle(uint32_t handle, GCHandleType handleType)
        : m_Handle(handle), m_HandleType(handleType)
    { }

    GCHandle GCManager::CreateWeakHandle(const ScriptObject& object)
    {
        uint32_t handle = mono_gchandle_new_weakref(object.GetMonoObject(), false);
        return { handle, GCHandleType::Weak };
    }

    GCHandle GCManager::CreateStrongHadle(const ScriptObject& object)
    {
        uint32_t handle = mono_gchandle_new(object.GetMonoObject(), true);
        return { handle, GCHandleType::Strong };
    }

    void GCManager::FreeHandle(GCHandle& handle)
    {
        mono_gchandle_free(handle.GetHandle());
        handle.m_Handle = 0;
    }

    MonoObject* GCManager::GetManagedObject(const GCHandle& handle)
    {
        TR_PROFILE_FUNCTION();
        if (!handle.IsValid())
            return nullptr;

        MonoObject* object = mono_gchandle_get_target(handle.GetHandle());

        return object;
    }

    void GCManager::CollectAll()
    {
        mono_gc_collect(mono_gc_max_generation());
    }
}
