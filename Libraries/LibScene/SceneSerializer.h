#pragma once

#include "Scene.h"
#include "SceneManager.h"

#include <LibCore/Base.h>
#include <LibCore/Result.h>
#include <LibCore/RefPtr.h>

#include <LibAsset/Asset.h>
#include <LibAsset/AssetImporter.h>
#include <LibAsset/AssetMetadata.h>
#include <LibAsset/AssetTypes.h>

#include <filesystem>

namespace Terran::World {

class SceneSerializer final : public Asset::AssetImporter {
public:
    explicit SceneSerializer(SceneManager& scene_system)
        : m_scene_system(scene_system)
    {
    }

    ~SceneSerializer() override = default;
    bool save(Asset::AssetMetadata const& assetMetadata, Core::RefPtr<Asset::Asset> const& asset) override;
    Asset::AssetLoadResult load(Asset::AssetMetadata const& assetMetadata) override;
    [[nodiscard]] bool can_handle(std::filesystem::path const& assetPath) override
    {
        return assetPath.extension() == ".terran";
    }
    [[nodiscard]] Asset::AssetTypeId asset_type() override
    {
        return Scene::static_type();
    }

private:
    SceneManager& m_scene_system;
};

}
