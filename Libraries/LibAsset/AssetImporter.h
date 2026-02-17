#pragma once

#include "AssetMetadata.h"
#include "Asset.h"
#include "AssetTypes.h"
#include "AssetError.h"

#include <LibCore/Base.h>
#include <LibCore/Result.h>
#include <LibCore/RefPtr.h>

#include <filesystem>

namespace Terran::Asset {

using AssetLoadResult = Core::Result<Core::RefPtr<Asset>, Core::Shared<AssetError>>;

class AssetImporter {
public:
    virtual ~AssetImporter() = default;
    [[nodiscard]] virtual AssetLoadResult load(AssetMetadata const& assetMetadata) = 0;
    virtual bool save(AssetMetadata const& assetMetadata, Core::RefPtr<Asset> const& asset) = 0;
    [[nodiscard]] virtual bool can_handle(std::filesystem::path const& assetPath) = 0;
    [[nodiscard]] virtual AssetTypeId asset_type() = 0;
};

}
