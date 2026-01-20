#pragma once

#include <LibCore/UUID.h>

#include <cstdint>

namespace Terran {
namespace Asset {

using AssetHandle = Terran::Core::UUID;
using AssetTypeId = uint64_t;
#define TR_LOG_ASSET "Asset"

}
}
