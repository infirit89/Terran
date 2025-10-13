#include "Variant.h"
#include "trpch.h"

namespace TerranEngine {
namespace Utils {

Variant::Variant()
{
    m_Type = Type::None;
    m_Data.iValue = 0;
}

Variant::Variant(Variant const& other)
{
    Copy(*this, other);
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

Variant::Variant(std::string const& str)
{
    m_Type = Type::String;
    size_t const strLen = str.size();
    m_Data.ptr = new char[strLen + 1];
    strcpy(static_cast<char*>(m_Data.ptr), str.c_str());
    static_cast<char*>(m_Data.ptr)[strLen] = '\0';
}

Variant::Variant(char const* str)
{
    m_Type = Type::String;
    size_t const strLen = strlen(str);
    m_Data.ptr = new char[strLen + 1];
    strcpy(static_cast<char*>(m_Data.ptr), str);
    static_cast<char*>(m_Data.ptr)[strLen] = '\0';
}

Variant::Variant(glm::vec2 const& vec2)
{
    m_Type = Type::Vector2;
    m_Data.ptr = new glm::vec2;
    static_cast<glm::vec2*>(m_Data.ptr)->x = vec2.x;
    static_cast<glm::vec2*>(m_Data.ptr)->y = vec2.y;
}

Variant::Variant(glm::vec3 const& vec3)
{
    m_Type = Type::Vector3;
    m_Data.ptr = new glm::vec3;
    static_cast<glm::vec3*>(m_Data.ptr)->x = vec3.x;
    static_cast<glm::vec3*>(m_Data.ptr)->y = vec3.y;
    static_cast<glm::vec3*>(m_Data.ptr)->z = vec3.z;
}

Variant::Variant(glm::vec4 const& vec4)
{
    m_Type = Type::Vector4;
    m_Data.ptr = new glm::vec4;
    static_cast<glm::vec4*>(m_Data.ptr)->x = vec4.x;
    static_cast<glm::vec4*>(m_Data.ptr)->y = vec4.y;
    static_cast<glm::vec4*>(m_Data.ptr)->z = vec4.z;
    static_cast<glm::vec4*>(m_Data.ptr)->w = vec4.w;
}

Variant::Variant(Terran::Core::UUID const& id)
{
    m_Type = Type::UUID;
    m_Data.ptr = new Terran::Core::UUID();
    static_cast<Terran::Core::UUID*>(m_Data.ptr)->SetData(id.GetData());
}

Variant::Variant(Entity entity)
    : Variant(entity.GetID())
{
}

Variant::Variant(char* data, Type const& type)
{
    m_Type = type;
    switch (type) {
    case Type::Bool:
        m_Data.bValue = *reinterpret_cast<bool*>(data);
        break;
    case Type::Char:
        m_Data.iValue = *data;
        break;
    case Type::Int8:
        m_Data.iValue = *reinterpret_cast<int8_t*>(data);
        break;
    case Type::Int16:
        m_Data.iValue = *reinterpret_cast<int16_t*>(data);
        break;
    case Type::Int32:
        m_Data.iValue = *reinterpret_cast<int32_t*>(data);
        break;
    case Type::Int64:
        m_Data.iValue = *reinterpret_cast<int64_t*>(data);
        break;
    case Type::UInt8:
        m_Data.iValue = *reinterpret_cast<uint8_t*>(data);
        break;
    case Type::UInt16:
        m_Data.iValue = *reinterpret_cast<uint16_t*>(data);
        break;
    case Type::UInt32:
        m_Data.iValue = *reinterpret_cast<uint32_t*>(data);
        break;
    case Type::UInt64:
        m_Data.iValue = *reinterpret_cast<uint64_t*>(data);
        break;
    case Type::Float:
        m_Data.dValue = *reinterpret_cast<float*>(data);
        break;
    case Type::Double:
        m_Data.dValue = *reinterpret_cast<double*>(data);
        break;
    case Type::Vector2: {
        glm::vec2 v2Data = *reinterpret_cast<glm::vec2*>(data);
        m_Data.ptr = new glm::vec2;
        static_cast<glm::vec2*>(m_Data.ptr)->x = v2Data.x;
        static_cast<glm::vec2*>(m_Data.ptr)->y = v2Data.y;
        break;
    }
    case Type::Vector3: {
        glm::vec3 v3Data = *reinterpret_cast<glm::vec3*>(data);
        m_Data.ptr = new glm::vec3;
        static_cast<glm::vec3*>(m_Data.ptr)->x = v3Data.x;
        static_cast<glm::vec3*>(m_Data.ptr)->y = v3Data.y;
        static_cast<glm::vec3*>(m_Data.ptr)->z = v3Data.z;
        break;
    }
    case Type::Vector4: {
        glm::vec4 v4Data = *reinterpret_cast<glm::vec4*>(data);
        m_Data.ptr = new glm::vec4;
        static_cast<glm::vec4*>(m_Data.ptr)->x = v4Data.x;
        static_cast<glm::vec4*>(m_Data.ptr)->y = v4Data.y;
        static_cast<glm::vec4*>(m_Data.ptr)->z = v4Data.z;
        static_cast<glm::vec4*>(m_Data.ptr)->w = v4Data.w;
        break;
    }
    case Type::UUID: {
        Terran::Core::UUID id = *reinterpret_cast<Terran::Core::UUID*>(data);
        m_Data.ptr = new Terran::Core::UUID();
        static_cast<Terran::Core::UUID*>(m_Data.ptr)->SetData(id.GetData());
        break;
    }
    default:;
    }
}

Variant::~Variant() { Clear(); }

void Variant::Clear()
{
    switch (m_Type) {
    case Type::String: {
        if (m_Data.ptr) {
            delete[] static_cast<char*>(m_Data.ptr);
            m_Data.ptr = nullptr;
        }
        break;
    }
    case Type::Vector2: {
        if (m_Data.ptr) {
            delete static_cast<glm::vec2*>(m_Data.ptr);
            m_Data.ptr = nullptr;
        }
        break;
    }
    case Type::Vector3: {
        if (m_Data.ptr) {
            delete static_cast<glm::vec3*>(m_Data.ptr);
            m_Data.ptr = nullptr;
        }
        break;
    }
    case Type::Vector4: {
        if (m_Data.ptr) {
            delete static_cast<glm::vec4*>(m_Data.ptr);
            m_Data.ptr = nullptr;
        }
        break;
    }
    case Type::UUID: {
        if (m_Data.ptr) {
            delete static_cast<Terran::Core::UUID*>(m_Data.ptr);
            m_Data.ptr = nullptr;
        }

        break;
    }
    default:;
    }
}

void Variant::Copy(Variant& result, Variant const& in)
{
    result.Clear();

    result.m_Type = in.m_Type;

    switch (result.m_Type) {
    case Type::Bool: {
        result.m_Data.bValue = in.m_Data.bValue;
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
    case Type::UInt64: {
        result.m_Data.iValue = in.m_Data.iValue;
        break;
    }
    case Type::Float:
    case Type::Double: {
        result.m_Data.dValue = in.m_Data.dValue;
        break;
    }
    case Type::String: {
        char const* str = in;
        size_t const strLen = strlen(str);
        result.m_Data.ptr = new char[strLen + 1];
        strcpy(static_cast<char*>(result.m_Data.ptr), str);
        static_cast<char*>(result.m_Data.ptr)[strLen] = '\0';
        break;
    }
    case Type::Vector2: {
        glm::vec2 vec2 = in;
        result.m_Data.ptr = new glm::vec2;
        static_cast<glm::vec2*>(result.m_Data.ptr)->x = vec2.x;
        static_cast<glm::vec2*>(result.m_Data.ptr)->y = vec2.y;
        break;
    }
    case Type::Vector3: {
        glm::vec3 vec3 = in;
        result.m_Data.ptr = new glm::vec3;
        static_cast<glm::vec3*>(result.m_Data.ptr)->x = vec3.x;
        static_cast<glm::vec3*>(result.m_Data.ptr)->y = vec3.y;
        static_cast<glm::vec3*>(result.m_Data.ptr)->z = vec3.z;
        break;
    }
    case Type::Vector4: {
        glm::vec4 vec4 = in;
        result.m_Data.ptr = new glm::vec4;
        static_cast<glm::vec4*>(result.m_Data.ptr)->x = vec4.x;
        static_cast<glm::vec4*>(result.m_Data.ptr)->y = vec4.y;
        static_cast<glm::vec4*>(result.m_Data.ptr)->z = vec4.z;
        static_cast<glm::vec4*>(result.m_Data.ptr)->w = vec4.w;
        break;
    }
    case Type::UUID: {
        Terran::Core::UUID id = in;
        result.m_Data.ptr = new Terran::Core::UUID();
        static_cast<Terran::Core::UUID*>(result.m_Data.ptr)->SetData(id.GetData());
        break;
    }
    default:;
    }
}

Variant::operator bool() const { return m_Type == Type::Bool ? m_Data.bValue : false; }
Variant::operator int8_t() const { return static_cast<int8_t>(m_Data.iValue); }
Variant::operator int16_t() const { return static_cast<int16_t>(m_Data.iValue); }
Variant::operator int32_t() const { return static_cast<int32_t>(m_Data.iValue); }
Variant::operator int64_t() const { return m_Data.iValue; }
Variant::operator uint8_t() const { return static_cast<uint8_t>(m_Data.iValue); }
Variant::operator uint16_t() const { return static_cast<uint16_t>(m_Data.iValue); }
Variant::operator uint32_t() const { return static_cast<uint32_t>(m_Data.iValue); }
Variant::operator uint64_t() const { return static_cast<uint64_t>(m_Data.iValue); }
Variant::operator char() const { return static_cast<char>(m_Data.iValue); }
Variant::operator float() const { return static_cast<float>(m_Data.dValue); }
Variant::operator double() const { return m_Data.dValue; }
Variant::operator char const*() const { return static_cast<char const*>(m_Data.ptr); }
Variant::operator std::string() const { return static_cast<char const*>(m_Data.ptr); }
Variant::operator glm::vec2() const { return *static_cast<glm::vec2*>(m_Data.ptr); }
Variant::operator glm::vec3() const { return *static_cast<glm::vec3*>(m_Data.ptr); }
Variant::operator glm::vec4() const { return *static_cast<glm::vec4*>(m_Data.ptr); }
Variant::operator Terran::Core::UUID() const { return *static_cast<Terran::Core::UUID*>(m_Data.ptr); }

}
}
