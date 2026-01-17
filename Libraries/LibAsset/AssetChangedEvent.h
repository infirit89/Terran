#pragma once

#include <cstdint>
namespace Terran {
namespace Asset {
enum class AssetChangeType : uint8_t {
    Added,
    Removed,
    Renamed,
    Modified,
};

struct AssetChangeEvent {

};
}
}
