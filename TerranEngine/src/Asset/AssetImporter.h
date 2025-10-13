#pragma once

#include "Asset.h"
#include "AssetLoaders.h"
#include "LibCore/Base.h"

#include <unordered_map>

namespace TerranEngine {

class AssetImporter final {
public:
    static void RegisterLoaders();

    static void Load(AssetInfo const& assetInfo, Terran::Core::Shared<Asset>& asset);
    static bool Save(AssetInfo const& assetInfo, Terran::Core::Shared<Asset> const& asset);

private:
    static std::unordered_map<AssetType, Terran::Core::Shared<AssetLoader>> s_Loaders;
};

}

