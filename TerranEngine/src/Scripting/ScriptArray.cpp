#include "trpch.h"
#include "ScriptArray.h"

#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>

namespace TerranEngine
{
    ScriptArray::ScriptArray(MonoClass* arrayClass, uint32_t size)
    {
        m_MonoArray = mono_array_new(mono_domain_get(), arrayClass, size);
        m_Length = size;
        m_ElementClass = arrayClass;
    }

    ScriptArray::ScriptArray(const ScriptArray& other)
    {
        uint32_t copyCount = m_Length > other.m_Length ? other.m_Length : m_Length;
        m_Length = other.m_Length;
        mono_value_copy_array(m_MonoArray, 0, other.m_MonoArray, copyCount);
    }

    char* ScriptArray::GetElementAddress(uint32_t index, int dataSize) const
    {
        return mono_array_addr_with_size(m_MonoArray, dataSize, index);
    }

    void ScriptArray::Resize(uint32_t size)
    {
        MonoArray* tempArr = mono_array_new(mono_domain_get(), m_ElementClass, m_Length);
        mono_value_copy_array(tempArr, 0, m_MonoArray, m_Length);
        uint32_t copyCount = m_Length > size ? size : m_Length;
        m_MonoArray = mono_array_new(mono_domain_get(), m_ElementClass, size);
        mono_value_copy_array(m_MonoArray, 0, tempArr, copyCount);
        m_Length = size;
    }

}

