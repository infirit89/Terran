#pragma once

#include "Asset.h"
#include "AssetImporter.h"
#include "AssetMetadata.h"
#include "AssetTypes.h"

#include <LibCore/Base.h>

#include <filesystem>
#include <type_traits>
#include <unordered_map>

namespace Terran::Asset {

class AssetImporterRegistry final {
public:
    template<typename TAsset>
    requires(
        std::is_base_of_v<Asset, TAsset>,
        HasStaticType<TAsset>)
    static void register_asset(Terran::Core::Shared<AssetImporter> loader)
    {
        s_loaders[TAsset::static_type()] = loader;
    }

    static void load(AssetMetadata const& assetMetadata, Terran::Core::Shared<Asset>& asset);
    static bool save(AssetMetadata const& assetMetadata, Terran::Core::Shared<Asset> const& asset);

    static bool exists_for_path(std::filesystem::path const& path)
    {
        for (auto const& [type, loader] : s_loaders) {
            if (loader->can_handle(path))
                return true;
        }

        return false;
    }

    static Terran::Core::Shared<AssetImporter> find_for_path(std::filesystem::path const& path)
    {
        for (auto const& [type, loader] : s_loaders) {
            if (loader->can_handle(path))
                return loader;
        }

        return nullptr;
    }

private:
    static std::unordered_map<AssetTypeId, Terran::Core::Shared<AssetImporter>> s_loaders;
};

}
