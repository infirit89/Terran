#include "trpch.h"
#include "ScriptType.h"
#include "ScriptCache.h"
#include "ManagedClass.h"

#include <mono/metadata/class.h>

namespace TerranEngine
{
#define TR_NATIVE_TYPE_DEF(typeName) if(monoTypeClass == *TR_API_CACHED_CLASS(typeName)) { return NativeType::typeName; }

    static NativeType GetNativeTypeFromMonoType(MonoType* monoType, MonoClass* monoTypeClass)
    {
        MonoTypeEnum type = (MonoTypeEnum)mono_type_get_type(monoType);

        switch (type)
        {
        case MONO_TYPE_BOOLEAN:		return NativeType::Bool;

        // signed integer types
        case MONO_TYPE_I1:			return NativeType::Int8;
        case MONO_TYPE_I2:			return NativeType::Int16;
        case MONO_TYPE_I4:			return NativeType::Int32;
        case MONO_TYPE_I8:			return NativeType::Int64;

        // unsigned integer types
        case MONO_TYPE_U1:			return NativeType::UInt8;
        case MONO_TYPE_U2:			return NativeType::UInt16;
        case MONO_TYPE_U4:			return NativeType::UInt32;
        case MONO_TYPE_U8:			return NativeType::UInt64;

        case MONO_TYPE_R4:			return NativeType::Float;
        case MONO_TYPE_R8:			return NativeType::Double;

        case MONO_TYPE_CHAR:		return NativeType::Char;
			
        case MONO_TYPE_STRING:		return NativeType::String;
        case MONO_TYPE_VALUETYPE: 
        {
            TR_NATIVE_TYPE_DEF(Vector2)
            TR_NATIVE_TYPE_DEF(Vector3)
            TR_NATIVE_TYPE_DEF(Color)
            TR_NATIVE_TYPE_DEF(LayerMask)

            if (mono_class_is_enum(monoTypeClass)) 
            {
                MonoType* enumType = mono_class_enum_basetype(monoTypeClass);
                type = (MonoTypeEnum)mono_type_get_type(enumType);

                switch (type)
                {
                case MONO_TYPE_BOOLEAN:		return NativeType::Bool;

                // signed integer types
                case MONO_TYPE_I1:			return NativeType::Int8;
                case MONO_TYPE_I2:			return NativeType::Int16;
                case MONO_TYPE_I4:			return NativeType::Int32;
                case MONO_TYPE_I8:			return NativeType::Int64;

                // unsigned integer types
                case MONO_TYPE_U1:			return NativeType::UInt8;
                case MONO_TYPE_U2:			return NativeType::UInt16;
                case MONO_TYPE_U4:			return NativeType::UInt32;
                case MONO_TYPE_U8:			return NativeType::UInt64;

                case MONO_TYPE_R4:			return NativeType::Float;
                case MONO_TYPE_R8:			return NativeType::Double;

                case MONO_TYPE_CHAR:		return NativeType::Char;
                }
            }
                
            break;
        }
        case MONO_TYPE_CLASS:
        {
            TR_NATIVE_TYPE_DEF(UUID)
            TR_NATIVE_TYPE_DEF(Entity)
        }
        }

        return NativeType::None;
    }
    
    ManagedType::ManagedType(const ManagedType& other)
        : m_NativeType(other.m_NativeType),
        m_MonoType(other.m_MonoType),
        m_TypeClass(other.m_TypeClass),
        m_TypeEncoding(other.m_TypeEncoding),
        m_Size(other.m_Size) { }

    ManagedType::ManagedType(MonoClass* monoClass) 
    {
        MonoType* monoType = mono_class_get_type(monoClass);
        m_MonoType = monoType;
        m_TypeEncoding = mono_type_get_type(monoType);
        m_TypeClass = monoClass;
        m_NativeType = GetNativeTypeFromMonoType(monoType, m_TypeClass);
        int alignment = 0;
        m_Size = mono_type_size(monoType, &alignment);
    }

    ManagedType::ManagedType(MonoType* monoType) 
    {
        m_TypeEncoding = mono_type_get_type(monoType);

        MonoClass* monoTypeClass = nullptr;

        if (m_TypeEncoding == MONO_TYPE_CLASS || m_TypeEncoding == MONO_TYPE_VALUETYPE)
            monoTypeClass = mono_type_get_class(monoType);
        else
            monoTypeClass = mono_class_from_mono_type(monoType);

        m_TypeClass = monoTypeClass;
        m_NativeType = GetNativeTypeFromMonoType(monoType, m_TypeClass);
        m_MonoType = monoType;
        int alignment = 0;
        m_Size = mono_type_size(monoType, &alignment);

    }

    bool ManagedType::IsArray() const       { return m_TypeEncoding == MONO_TYPE_ARRAY || m_TypeEncoding == MONO_TYPE_SZARRAY; }
    bool ManagedType::IsPointer() const     { return mono_type_is_pointer(m_MonoType); }
    bool ManagedType::IsVoid() const        { return mono_type_is_void(m_MonoType); }
    bool ManagedType::IsObject() const      { return m_TypeEncoding == MONO_TYPE_OBJECT; }
    bool ManagedType::IsEnum() const        { return mono_class_is_enum(m_TypeClass); }

    ManagedType& ManagedType::operator=(const ManagedType& other)
    {
        m_NativeType = other.m_NativeType;
        m_MonoType = other.m_MonoType;
        m_TypeClass = other.m_TypeClass;
        m_TypeEncoding = other.m_TypeEncoding;
        m_Size = other.m_Size;
        return *this;
    }
}
