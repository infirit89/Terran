#pragma once

#include "AssetTypes.h"

#include <filesystem>

namespace Terran {
namespace Asset {

struct AssetMetadata final {

    AssetMetadata() noexcept = default;

    bool operator==(AssetMetadata const& other) const
    {
        return Path == other.Path && Type == other.Type;
    }

    bool operator!=(AssetMetadata const& other) const
    {
        return !((*this) == other);
    }

    operator bool() const
    {
        return Path != "" && AssetId.is_valid();
    }

    std::filesystem::path Path = "";
    AssetTypeId Type;
    AssetId AssetId = AssetId::invalid();
};

}
}
