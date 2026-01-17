#pragma once

#include "Asset.h"
#include "AssetLoader.h"
#include "AssetMetadata.h"
#include "AssetTypes.h"

#include <LibCore/Base.h>

#include <filesystem>
#include <type_traits>
#include <unordered_map>

namespace Terran {
namespace Asset {

class AssetImporter final {
public:
    template<typename TAsset>
    requires(
        std::is_base_of_v<Asset, TAsset>,
        HasStaticType<TAsset>)
    static void register_asset(Terran::Core::Shared<AssetLoader> loader)
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

    static Terran::Core::Shared<AssetLoader> find_for_path(std::filesystem::path const& path)
    {
        for (auto const& [type, loader] : s_loaders) {
            if (loader->can_handle(path))
                return loader;
        }

        return nullptr;
    }

private:
    static std::unordered_map<AssetTypeId, Terran::Core::Shared<AssetLoader>> s_loaders;
};

}

}
