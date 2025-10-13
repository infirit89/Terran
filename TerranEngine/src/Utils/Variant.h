#pragma once

#include "LibCore/UUID.h"
#include "Scene/Entity.h"

#include <glm/glm.hpp>

#include <cstdint>
#include <string>

namespace TerranEngine {
namespace Utils {

class Variant final {
public:
    enum class Type {
        None = 0,
        Bool,
        Char,
        Int8,
        Int16,
        Int32,
        Int64,
        UInt8,
        UInt16,
        UInt32,
        UInt64,
        Float,
        Double,
        String,
        Vector2,
        Vector3,
        Vector4,
        UUID
    };

    Variant();
    Variant(Variant const& other);

    Variant(int8_t i8);
    Variant(int16_t i16);
    Variant(int32_t i32);
    Variant(int64_t i64);

    Variant(uint8_t u8);
    Variant(uint16_t u16);
    Variant(uint32_t u32);
    Variant(uint64_t u64);

    Variant(bool b);

    Variant(char c);

    Variant(float f);
    Variant(double d);

    Variant(std::string const& str);
    Variant(char const* str);

    Variant(glm::vec2 const& vec2);
    Variant(glm::vec3 const& vec3);
    Variant(glm::vec4 const& vec4);

    Variant(Terran::Core::UUID const& id);
    Variant(Entity entity);

    Variant(char* data, Type const& type);

    ~Variant();

    void Clear();

    Variant& operator=(Variant const& other)
    {
        Copy(*this, other);
        return *this;
    }

    operator bool() const;

    operator int8_t() const;
    operator int16_t() const;
    operator int32_t() const;
    operator int64_t() const;

    operator uint8_t() const;
    operator uint16_t() const;
    operator uint32_t() const;
    operator uint64_t() const;

    operator char() const;

    operator float() const;
    operator double() const;

    operator char const*() const;
    operator std::string() const;

    operator glm::vec2() const;
    operator glm::vec3() const;
    operator glm::vec4() const;

    operator Terran::Core::UUID() const;

    Type GetType() const { return m_Type; }

private:
    static void Copy(Variant& result, Variant const& in);

private:
    union {
        double dValue;
        int64_t iValue;
        bool bValue;
        void* ptr;
    } m_Data {};

    Type m_Type = Type::None;
};

}
}
