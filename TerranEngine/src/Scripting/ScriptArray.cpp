#include "trpch.h"
#include "ScriptArray.h"
#include "ScriptObject.h"
#include "ScriptType.h"

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

    ScriptArray::ScriptArray(MonoArray* monoArray)
        : m_MonoArray(monoArray)
    {
        m_Length = mono_array_length(monoArray);
        ScriptObject arrayObject((MonoObject*)m_MonoArray);
        
        const ScriptClass arrayKlass = arrayObject.GetClass();
        const ScriptType arrayType = ScriptType::FromClass(arrayKlass);
        const MonoArrayType* arrayElementType = mono_type_get_array_type(arrayType.GetMonoType());
        m_ElementClass = arrayElementType->eklass;
    }

    ScriptArray::ScriptArray(const ScriptArray& other)
    {
        m_MonoArray = mono_array_new(mono_domain_get(), other.m_ElementClass, other.m_Length);
        m_Length = other.m_Length;
        mono_value_copy_array(m_MonoArray, 0, other.m_MonoArray, m_Length);
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

