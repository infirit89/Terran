#pragma once

#include "Scene.h"

#include <LibCore/Base.h>
#include <LibCore/Result.h>

#include <LibAsset/AssetImporter.h>
#include <LibAsset/AssetMetadata.h>
#include <LibAsset/Asset.h>
#include <LibAsset/AssetTypes.h>

#include <filesystem>

namespace Terran::World {

class SceneSerializer final : Asset::AssetImporter {
public:
    SceneSerializer() = default;
    ~SceneSerializer() override = default;
    bool save(Asset::AssetMetadata const& assetMetadata, Core::Shared<Asset::Asset> const& asset) override;
    Asset::AssetLoadResult load(Asset::AssetMetadata const& assetMetadata) override;
    [[nodiscard]] bool can_handle(std::filesystem::path const& assetPath) override {
        return assetPath.extension() == ".terran";
    }
    [[nodiscard]] Asset::AssetTypeId asset_type() override {
        return Scene::static_type();
    }

};

}
