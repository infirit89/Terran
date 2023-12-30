#include "trpch.h"

#include "ScriptArray.h"
#include "ScriptCache.h"
#include "ManagedObject.h"
#include "ScriptType.h"
#include "ManagedClass.h"
#include "ScriptMarshal.h"

#include "Utils/Debug/OptickProfiler.h"

#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>

namespace TerranEngine
{
    namespace detail 
    {
        template<>
        MonoClass* ScriptArrayCreate<uint8_t>(uint32_t size)
        {
            return ScriptCache::GetCachedClassFromName("System.Byte")->GetMonoClass();
        }
        template<>
        MonoClass* ScriptArrayCreate<uint16_t>(uint32_t size) 
        {
            return ScriptCache::GetCachedClassFromName("System.UInt16")->GetMonoClass();
        }
        template<>
        MonoClass* ScriptArrayCreate<uint32_t>(uint32_t size)
        {
            return ScriptCache::GetCachedClassFromName("System.UInt32")->GetMonoClass();
        }
        template<>
        MonoClass* ScriptArrayCreate<uint64_t>(uint32_t size)
        {
            return ScriptCache::GetCachedClassFromName("System.UInt64")->GetMonoClass();
        }

        template<>
        MonoClass* ScriptArrayCreate<int8_t>(uint32_t size)
        {
            return ScriptCache::GetCachedClassFromName("System.SByte")->GetMonoClass();
        }
        template<>
        MonoClass* ScriptArrayCreate<int16_t>(uint32_t size)
        {
            return ScriptCache::GetCachedClassFromName("System.Int16")->GetMonoClass();
        }
        template<>
        MonoClass* ScriptArrayCreate<int32_t>(uint32_t size)
        {
            return ScriptCache::GetCachedClassFromName("System.Int32")->GetMonoClass();
        }
        template<>
        MonoClass* ScriptArrayCreate<int64_t>(uint32_t size)
        {
            return ScriptCache::GetCachedClassFromName("System.Int64")->GetMonoClass();
        }

        template<>
        MonoClass* ScriptArrayCreate<bool>(uint32_t size)
        {
            return ScriptCache::GetCachedClassFromName("System.Bool")->GetMonoClass();
        }

        template<>
        MonoClass* ScriptArrayCreate<float>(uint32_t size)
        {
            return ScriptCache::GetCachedClassFromName("System.Single")->GetMonoClass();
        }
        template<>
        MonoClass* ScriptArrayCreate<double>(uint32_t size)
        {
            return ScriptCache::GetCachedClassFromName("System.Double")->GetMonoClass();
        }

        template<>
        MonoClass* ScriptArrayCreate<std::string>(uint32_t size)
        {
            return ScriptCache::GetCachedClassFromName("System.String")->GetMonoClass();
        }

        template<>
        MonoClass* ScriptArrayCreate<MonoObject*>(uint32_t size)
        {
            return ScriptCache::GetCachedClassFromName("System.Object")->GetMonoClass();
        }

        template<>
        MonoClass* ScriptArrayCreate<glm::vec2>(uint32_t size)
        {
            return ScriptCache::GetCachedClassFromName("Terran.Vector2")->GetMonoClass();
        }

        template<>
        MonoClass* ScriptArrayCreate<UUID>(uint32_t size)
        {
            return ScriptCache::GetCachedClassFromName("Terran.UUID")->GetMonoClass();
        }
    }

    ScriptArray::ScriptArray(MonoClass* arrayClass, uint32_t size)
    {
        TR_PROFILE_FUNCTION();
        m_MonoArray = mono_array_new(mono_domain_get(), arrayClass, size);
        m_Length = size;
        m_ElementType = ManagedType(arrayClass);
    }
    
    ScriptArray ScriptArray::Create(MonoArray* monoArray)
    {
        TR_PROFILE_FUNCTION();
        MonoClass* arrayMonoClass = mono_object_get_class(reinterpret_cast<MonoObject*>(monoArray));
        //const MonoArrayType* arrayElementType = mono_type_get_array_type(arrayType);
        ScriptArray array;
        array.m_MonoArray = monoArray;
        array.m_Length = mono_array_length(monoArray);
        array.m_ElementType = ManagedType(mono_class_get_element_class(arrayMonoClass));
        //int size = mono_class_array_element_size(arrayElementType->eklass);

        return array;
    }

    void ScriptArray::SetData(const Utils::Variant& value, uint32_t index)
    {
        TR_PROFILE_FUNCTION();
        switch (value.GetType())
        {
        case Utils::Variant::Type::Bool:        mono_array_set(m_MonoArray, bool, index, (bool)value); return;
        case Utils::Variant::Type::Char:        mono_array_set(m_MonoArray, char, index, (char)value); return;
        case Utils::Variant::Type::Int8:        mono_array_set(m_MonoArray, int8_t, index, (int8_t)value); return;
        case Utils::Variant::Type::Int16:       mono_array_set(m_MonoArray, int16_t, index, (int16_t)value); return;
        case Utils::Variant::Type::Int32:       mono_array_set(m_MonoArray, int32_t, index, (int32_t)value); return;
        case Utils::Variant::Type::Int64:       mono_array_set(m_MonoArray, int64_t, index, (int64_t)value); return;
        case Utils::Variant::Type::UInt8:       mono_array_set(m_MonoArray, uint8_t, index, (uint8_t)value); return;
        case Utils::Variant::Type::UInt16:      mono_array_set(m_MonoArray, uint16_t, index, (uint16_t)value); return;
        case Utils::Variant::Type::UInt32:      mono_array_set(m_MonoArray, uint32_t, index, (uint32_t)value); return;
        case Utils::Variant::Type::UInt64:      mono_array_set(m_MonoArray, uint64_t, index, (uint64_t)value); return;
        case Utils::Variant::Type::Float:       mono_array_set(m_MonoArray, float, index, (float)value); return;
        case Utils::Variant::Type::Double:      mono_array_set(m_MonoArray, double, index, (double)value); return;
        case Utils::Variant::Type::String:      mono_array_set(m_MonoArray, MonoString*,
                                                index, ScriptMarshal::UTF8ToMonoString(value)); return;
        case Utils::Variant::Type::Vector2:     mono_array_set(m_MonoArray, glm::vec2, index, (glm::vec2)value); return;
        case Utils::Variant::Type::Vector3:     mono_array_set(m_MonoArray, glm::vec3, index, (glm::vec3)value); return;
        case Utils::Variant::Type::Vector4:     mono_array_set(m_MonoArray, glm::vec4, index, (glm::vec4)value); return;
        case Utils::Variant::Type::UUID:        mono_array_set(m_MonoArray, UUID, index, (UUID)value); return;
        }

        TR_ERROR("Unknown Variant Type!");
    }

    char* ScriptArray::GetElementAddress(uint32_t index, int dataSize) const
    {
        TR_PROFILE_FUNCTION();
        return mono_array_addr_with_size(m_MonoArray, dataSize, index);
    }

    Utils::Variant ScriptArray::At(uint32_t index) const
    {
        TR_PROFILE_FUNCTION();
        if(m_ElementType.IsObject() || m_ElementType.GetNativeType() == NativeType::String)
        {
            MonoObject* obj = mono_array_get(m_MonoArray, MonoObject*, index);
            return ScriptMarshal::MonoObjectToVariant(obj, m_ElementType);
        }
        else
        {
            char* val = mono_array_addr_with_size(m_MonoArray, m_ElementType.GetSize(), index);
            return Utils::Variant(val, ScriptMarshal::ScriptTypeToVariantType(m_ElementType));
        }

        return { };
    }

    void ScriptArray::Resize(size_t size)
    {
        MonoArray* tempArr = mono_array_new(mono_domain_get(), m_ElementType.GetTypeClass(), size);

        const size_t copyCount = m_Length > size ? size : m_Length;
        char* tempArrAddr = mono_array_addr_with_size(tempArr, m_ElementType.GetSize(), 0);
        char* arrAddr = GetElementAddress(0, m_ElementType.GetSize());
        memcpy(tempArrAddr, arrAddr, copyCount * m_ElementType.GetSize());

        m_Length = size;
        m_MonoArray = tempArr;
    }
}
