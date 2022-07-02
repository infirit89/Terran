#include "trpch.h"
#include "Utils.h"

#include "Scene/SceneManager.h"

namespace TerranEngine 
{
    namespace Utils
    {
        
    glm::vec4 NormalizeColor4(const glm::vec4& color)
    {
        return { color.x / 255, color.y / 255, color.z / 255, color.w / 255 };
    }

    Variant::Variant()
    {
        m_Type = Type::None;
        m_Data.iValue = 0;
    }

    Variant::Variant(const Variant& other)
    {
        Clear();

        m_Type = other.m_Type;

        switch (m_Type)
        {
        case Type::Bool:
        {
            m_Data.bValue = other.m_Data.bValue;
            break;
        }
        case Type::Char:
        case Type::Int32:
        case Type::Int8:
        case Type::Int16:
        case Type::Int64:
        case Type::UInt8:
        case Type::UInt16:
        case Type::UInt32:
        case Type::UInt64:
        {
            m_Data.iValue = other.m_Data.iValue;
            break;
        }
        case Type::Float:
        case Type::Double:
        {
            m_Data.dValue = other.m_Data.dValue;
            break;
        }
        case Type::String:
        {
            const char* str = other;
            const size_t strLen = strlen(str);
            m_Data.ptr = new char[strLen + 1];
            strcpy((char*)m_Data.ptr, str);
            ((char*)m_Data.ptr)[strLen] = '\0';
            break;
        }
        case Type::Vector2:
        {
            glm::vec2 vec2 = other;
            m_Data.ptr = new glm::vec2;
            ((glm::vec2*)m_Data.ptr)->x = vec2.x;
            ((glm::vec2*)m_Data.ptr)->y = vec2.y;
            break;
        }
        case Type::Vector3:
        {
            glm::vec3 vec3 = other;
            m_Data.ptr = new glm::vec3;
            ((glm::vec3*)m_Data.ptr)->x = vec3.x;
            ((glm::vec3*)m_Data.ptr)->y = vec3.y;
            ((glm::vec3*)m_Data.ptr)->z = vec3.z;
            break;
        }
        case Type::Vector4:
        {
            glm::vec4 vec4 = other;
            m_Data.ptr = new glm::vec4;
            ((glm::vec4*)m_Data.ptr)->x = vec4.x;
            ((glm::vec4*)m_Data.ptr)->y = vec4.y;
            ((glm::vec4*)m_Data.ptr)->z = vec4.z;
            ((glm::vec4*)m_Data.ptr)->w = vec4.w;
            break;
        }
        case Type::UUID:
        {
            UUID id = other;
            m_Data.ptr = new UUID(id.GetData());
            break;
        }
        }
    }

    Variant::Variant(int8_t i8)
    {
        m_Type = Type::Int8;
        m_Data.iValue = i8;
    }

    Variant::Variant(int16_t i16)
    {
        m_Type = Type::Int16;
        m_Data.iValue = i16;
    }

    Variant::Variant(int32_t i32)
    {
        m_Type = Type::Int32;
        m_Data.iValue = i32;
    }

    Variant::Variant(int64_t i64)
    {
        m_Type = Type::Int64;
        m_Data.iValue = i64;
    }

    Variant::Variant(uint8_t u8)
    {
        m_Type = Type::UInt8;
        m_Data.iValue = u8;
    }

    Variant::Variant(uint16_t u16)
    {
        m_Type = Type::UInt16;
        m_Data.iValue = u16;
    }

    Variant::Variant(uint32_t u32)
    {
        m_Type = Type::UInt32;
        m_Data.iValue = u32;
    }

    Variant::Variant(uint64_t u64)
    {
        m_Type = Type::UInt64;
        m_Data.iValue = u64;
    }

    Variant::Variant(bool b)
    {
        m_Type = Type::Bool;
        m_Data.bValue = b;
    }

    Variant::Variant(char c)
    {
        m_Type = Type::Char;
        m_Data.iValue = c;
    }

    Variant::Variant(float f)
    {
        m_Type = Type::Float;
        m_Data.dValue = f;
    }

    Variant::Variant(double d)
    {
        m_Type = Type::Double;
        m_Data.dValue = d;
    }

    Variant::Variant(const std::string& str)
    {
        m_Type = Type::String;
        const size_t strLen = str.size();
        m_Data.ptr = new char[strLen + 1];
        strcpy((char*)m_Data.ptr, str.c_str());
        ((char*)m_Data.ptr)[strLen] = '\0';
    }

    Variant::Variant(const char* str)
    {
        m_Type = Type::String;
        const size_t strLen = strlen(str);
        m_Data.ptr = new char[strLen + 1];
        strcpy((char*)m_Data.ptr, str);
        ((char*)m_Data.ptr)[strLen] = '\0';
    }

    Variant::Variant(const glm::vec2& vec2)
    {
        m_Type = Type::Vector2;
        m_Data.ptr = new glm::vec2;
        ((glm::vec2*)m_Data.ptr)->x = vec2.x;
        ((glm::vec2*)m_Data.ptr)->y = vec2.y;
    }

    Variant::Variant(const glm::vec3& vec3)
    {
        m_Type = Type::Vector3;
        m_Data.ptr = new glm::vec3;
        ((glm::vec3*)m_Data.ptr)->x = vec3.x;
        ((glm::vec3*)m_Data.ptr)->y = vec3.y;
        ((glm::vec3*)m_Data.ptr)->z = vec3.z;
    }

    Variant::Variant(const glm::vec4& vec4)
    {
        m_Type = Type::Vector4;
        m_Data.ptr = new glm::vec4;
        ((glm::vec4*)m_Data.ptr)->x = vec4.x;
        ((glm::vec4*)m_Data.ptr)->y = vec4.y;
        ((glm::vec4*)m_Data.ptr)->z = vec4.z;
        ((glm::vec4*)m_Data.ptr)->w = vec4.w;
    }

    Variant::Variant(const UUID& id)
    {
        m_Type = Type::UUID;
        m_Data.ptr = new UUID(id.GetData());
    }

    Variant::Variant(Entity entity)
        : Variant(entity.GetID()) { }

    Variant::~Variant() { Clear(); }

    void Variant::Clear()
    {
        switch (m_Type)
        {
        case Type::String:
        {
            if(m_Data.ptr)
            { 
                delete[] (char*)m_Data.ptr;
                m_Data.ptr = nullptr;
            }
            break;
        }
        case Type::Vector2:
        {
            if(m_Data.ptr)
            {
                delete (glm::vec2*)m_Data.ptr;
                m_Data.ptr = nullptr;
            }
            break;
        }
        case Type::Vector3:
        {
            if(m_Data.ptr)
            {
                delete (glm::vec3*)m_Data.ptr;
                m_Data.ptr = nullptr;
            }
            break;
        }
        case Type::Vector4:
        {
            if(m_Data.ptr)
            {
                delete (glm::vec4*)m_Data.ptr;
                m_Data.ptr = nullptr;
            }
            break;
        }
        case Type::UUID:
        {
            if(m_Data.ptr)
            {
                delete (UUID*)m_Data.ptr;
                m_Data.ptr = nullptr;
            }

            break;
        }
        }
    }
        
    Variant::operator bool() const          { return m_Type == Type::Bool ?  m_Data.bValue : false; }
    Variant::operator int8_t() const        { return static_cast<int8_t>(m_Data.iValue); }
    Variant::operator int16_t() const       { return static_cast<int16_t>(m_Data.iValue); }
    Variant::operator int32_t() const       { return static_cast<int32_t>(m_Data.iValue); }
    Variant::operator int64_t() const       { return m_Data.iValue; }
    Variant::operator uint8_t() const       { return static_cast<uint8_t>(m_Data.iValue); }
    Variant::operator uint16_t() const      { return static_cast<uint16_t> (m_Data.iValue); }
    Variant::operator uint32_t() const      { return static_cast<uint32_t>(m_Data.iValue); }
    Variant::operator uint64_t() const      { return static_cast<uint64_t>(m_Data.iValue); }
    Variant::operator char() const          { return static_cast<char>(m_Data.iValue); }
    Variant::operator float() const         { return static_cast<float>(m_Data.dValue); }
    Variant::operator double() const        { return m_Data.dValue; }
    Variant::operator const char*() const   { return static_cast<const char*>(m_Data.ptr); }
    Variant::operator std::string() const   { return static_cast<const char*>(m_Data.ptr); }
    Variant::operator glm::vec2() const     { return *static_cast<glm::vec2*>(m_Data.ptr); }
    Variant::operator glm::vec3() const     { return *static_cast<glm::vec3*>(m_Data.ptr); }
    Variant::operator glm::vec4() const     { return *static_cast<glm::vec4*>(m_Data.ptr); }
    Variant::operator UUID() const          { return *static_cast<UUID*>(m_Data.ptr); }
    Variant::operator Entity() const        { return SceneManager::GetCurrentScene()->FindEntityWithUUID(operator UUID()); }
    }
}
