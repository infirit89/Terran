#pragma once

#include "Asset.h"
#include "AssetImporter.h"
#include "AssetMetadata.h"
#include "AssetTypes.h"

#include <LibCore/Base.h>

// #include "Graphics/Texture.h"
#include <filesystem>

namespace Terran::Asset {

// class TextureAssetLoader final : public AssetLoader {
// public:
//     void Load(AssetInfo const& assetInfo, Terran::Core::Shared<Asset>& asset) override;
//     bool Save(AssetInfo const& assetInfo, Terran::Core::Shared<Asset> const& asset) override;
//     static Terran::Core::Shared<Texture2D> CreateTextureFromFile(std::filesystem::path const& textureSourcePath);
// };

// class TextAssetImporter final : public AssetImporter {
// public:
//     virtual void load(AssetMetadata const& assetInfo, Terran::Core::Shared<Asset>& asset) override;
//     virtual bool save(AssetMetadata const& assetInfo, Terran::Core::Shared<Asset> const& asset) override;
//     virtual bool can_handle(std::filesystem::path const& assetPath) override;
//     virtual AssetTypeId asset_type() override;
// };

// class SceneAssetLoader final : public AssetLoader {
// public:
//     void Load(AssetInfo const& assetInfo, Terran::Core::Shared<Asset>& asset) override;
//     bool Save(AssetInfo const& assetInfo, Terran::Core::Shared<Asset> const& asset) override;
// };

// class PhysicsMaterial2DAssetLoader final : public AssetLoader {
// public:
//     void Load(AssetInfo const& assetInfo, Terran::Core::Shared<Asset>& asset) override;
//     bool Save(AssetInfo const& assetInfo, Terran::Core::Shared<Asset> const& asset) override;
// };

}

