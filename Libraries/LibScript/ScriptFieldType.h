#pragma once

#include <cstdint>

namespace Terran::Script {

enum class ScriptFieldType : uint8_t {
    None = 0,

    Int8,
    UInt8,
    Int16,
    UInt16,
    Int32,
    UInt32,
    Int64,
    UInt64,

    Float,
    Double,

    Bool,
    Char,

    String,

    Vector2,
    Vector3,
    Color,

    Entity
};

}
