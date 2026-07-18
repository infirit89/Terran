#pragma once

#include <LibCore/UUID.h>

#include <cstdint>

namespace Terran {
namespace Asset {

using AssetId = Terran::Core::UUID;

using AssetTypeId = uint64_t;
constexpr char const* const ASSET_SYSTEM = "Asset";

}
}
