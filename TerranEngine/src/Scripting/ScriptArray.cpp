#include "trpch.h"
#include "ScriptArray.h"

#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>

namespace TerranEngine
{
    ScriptArray::ScriptArray(MonoClass* arrayClass, uint32_t size)
    {
        m_MonoArray = mono_array_new(mono_domain_get(), arrayClass, size);
    }

    ScriptArray::ScriptArray(const ScriptArray& other)
    {
        mono_arra
    }

    char* ScriptArray::GetElementAddress(uint32_t index, int dataSize) const
    {
        return mono_array_addr_with_size(m_MonoArray, dataSize, index);
    }

    uintptr_t ScriptArray::Length() const
    {
        return mono_array_length(m_MonoArray);
    }
}

