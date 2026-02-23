#pragma once

#include <LibCore/UUID.h>

#include <cstdint>

namespace Terran {
namespace Asset {

using AssetId = Terran::Core::UUID;

using AssetTypeId = uint64_t;
constexpr char const* const TR_LOG_ASSET = "Asset";

}
}
