#pragma once

#include <cstdint>

namespace Terran::Graphics {

enum class Format : uint8_t {
    UNDEFINED,
    RGB32_FLOAT,
    RG32_FLOAT,
    RGBA8,
    BGRA8,
    D32,
    D32S8,
    D24S8,
};

enum class ImageTiling : uint8_t {
    Optimal,
    Linear
};

}
