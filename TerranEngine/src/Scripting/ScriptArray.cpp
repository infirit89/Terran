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
        m_Type = ScriptType::FromClass(arrayClass);
    }
    
    ScriptArray ScriptArray::Create(MonoArray* monoArray)
    {
        ScriptObject arrayObject((MonoObject*)monoArray);
        const ScriptType arrayType = ScriptType::FromClass(arrayObject.GetClass());
        ScriptType elementType = arrayType.GetElementType();
        ScriptArray array;
        array.m_MonoArray = monoArray;
        array.m_Length = mono_array_length(monoArray);
        array.m_Type = ScriptType::FromClass(*elementType.GetTypeClass());
        return array;
    }
    
    char* ScriptArray::GetElementAddress(uint32_t index, int dataSize) const
    {
        return mono_array_addr_with_size(m_MonoArray, dataSize, index);
    }

    void ScriptArray::Resize(uint32_t size)
    {
        MonoArray* tempArr = mono_array_new(mono_domain_get(), m_Type.GetTypeClass()->GetMonoClass(), m_Length);

        char* tempArrAddr = mono_array_addr_with_size(tempArr, m_Type.GetSize(), 0);
        char* arrAddr = GetElementAddress(m_Type.GetSize(), 0);
        memcpy(tempArrAddr, arrAddr, m_Length);
        
        const uint32_t copyCount = m_Length > size ? size : m_Length;
        m_MonoArray = mono_array_new(mono_domain_get(), m_Type.GetTypeClass()->GetMonoClass(), size);
        
        arrAddr = GetElementAddress(m_Type.GetSize(), 0);
        memcpy(arrAddr, tempArrAddr, copyCount);

        m_Length = size;
    }
}

