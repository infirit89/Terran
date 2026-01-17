#pragma once

#include "AssetMetadata.h"
#include "Asset.h"

#include <LibAsset/AssetTypes.h>
#include <LibCore/Base.h>
#include <filesystem>

namespace Terran::Asset {

class AssetImporter {
public:
    virtual ~AssetImporter() = default;
    virtual void load(AssetMetadata const& assetInfo, Terran::Core::Shared<Asset>& asset) = 0;
    virtual bool save(AssetMetadata const& assetInfo, Terran::Core::Shared<Asset> const& asset) = 0;
    virtual bool can_handle(std::filesystem::path const& assetPath) = 0;
    virtual AssetTypeId asset_type() = 0;
};

}
