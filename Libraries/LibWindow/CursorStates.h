#pragma once

#include <cstdint>

namespace Terran {
namespace Window {

enum class CursorMode : uint32_t {
    Normal = 0x00034001,
    Hidden = 0x00034002,
    Disabled = 0x00034003,
};

}

}

