﻿#include "trpch.h"
#include "ScriptType.h"
#include "ScriptCache.h"
#include "ScriptClass.h"

#include <mono/metadata/class.h>

namespace TerranEngine
{
    void ScriptType::GetUnmanagedType(MonoType* monoType)
    {
        const MonoTypeEnum type = (MonoTypeEnum)mono_type_get_type(monoType);

        switch (type)
        {
        case MONO_TYPE_BOOLEAN:		TypeEnum = Bool; return;

        // signed integer types
        case MONO_TYPE_I1:			TypeEnum = Int8; return;
        case MONO_TYPE_I2:			TypeEnum = Int16; return;
        case MONO_TYPE_I4:			TypeEnum = Int32; return;
        case MONO_TYPE_I8:			TypeEnum = Int64; return;

        // unsigned integer types
        case MONO_TYPE_U1:			TypeEnum = UInt8; return;
        case MONO_TYPE_U2:			TypeEnum = UInt16; return;
        case MONO_TYPE_U4:			TypeEnum = UInt32; return;
        case MONO_TYPE_U8:			TypeEnum = UInt64; return;

        case MONO_TYPE_R4:			TypeEnum = Float; return;
        case MONO_TYPE_R8:			TypeEnum = Double; return;

        case MONO_TYPE_CHAR:		TypeEnum = Char; return;
			
        case MONO_TYPE_STRING:		TypeEnum = String; return;
        case MONO_TYPE_VALUETYPE: 
        {
            const ScriptClass typeClass = mono_class_from_mono_type(monoType);
            if(typeClass == *TR_API_CACHED_CLASS(Vector2))  { TypeEnum = Vector2; return; }
            if(typeClass == *TR_API_CACHED_CLASS(Vector3))  { TypeEnum = Vector3; return; }
            if(typeClass == *TR_API_CACHED_CLASS(Color))    { TypeEnum = Color; return; }
                
            break;
        }
        case MONO_TYPE_CLASS:
        {
            const ScriptClass typeClass = mono_class_from_mono_type(monoType);
            if(typeClass == *TR_API_CACHED_CLASS(Entity))   { TypeEnum = Entity; return; }
        }
        }

        TypeEnum = None;
    }
    
    ScriptType::ScriptType(MonoType* monoType)
        : m_MonoType(monoType)
    {
        m_TypeEncoding = mono_type_get_type(monoType);
        GetUnmanagedType(m_MonoType);

        MonoClass* monoTypeClass = nullptr;
        if(m_TypeEncoding == MONO_TYPE_CLASS || m_TypeEncoding == MONO_TYPE_VALUETYPE)
             monoTypeClass = mono_type_get_class(monoType);
        else
            monoTypeClass =  mono_class_from_mono_type(monoType);
        m_TypeClass = new ScriptClass(monoTypeClass);
        
        int alignment = 0;
        m_Size = mono_type_size(m_MonoType, &alignment);
    }

    ScriptType::ScriptType(const ScriptType& other)
        :TypeEnum(other.TypeEnum),  m_MonoType(other.m_MonoType), m_TypeEncoding(other.m_TypeEncoding),
            m_Size(other.m_Size)
    {
        m_TypeClass = new ScriptClass(*other.m_TypeClass);
    }

    ScriptType::~ScriptType()
    {
        delete m_TypeClass;
    }

    ScriptType ScriptType::FromClass(const ScriptClass& klass)
    {
        MonoType* type = mono_class_get_type(klass.GetMonoClass());
        return { type };
    }

    ScriptType ScriptType::GetElementType() const
    {
        const MonoArrayType* monoElementType = mono_type_get_array_type(m_MonoType);
        return FromClass(monoElementType->eklass);
    }

    bool ScriptType::IsArray() const
    { return m_TypeEncoding == MONO_TYPE_ARRAY || m_TypeEncoding == MONO_TYPE_SZARRAY; }
    bool ScriptType::IsPointer() const  { return mono_type_is_pointer(m_MonoType); }
    bool ScriptType::IsVoid() const     { return mono_type_is_void(m_MonoType); }

    ScriptType& ScriptType::operator=(const ScriptType& other)
    {
        TypeEnum = other.TypeEnum;
        m_MonoType = other.m_MonoType;
        m_TypeClass = new ScriptClass(*other.m_TypeClass);
        m_TypeEncoding = other.m_TypeEncoding;
        m_Size = other.m_Size;
        return *this;
    }
}

