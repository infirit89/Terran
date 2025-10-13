#pragma once

#include "LibCore/Base.h"

#include "Asset.h"

#include "Graphics/Texture.h"
#include <filesystem>

namespace TerranEngine {

class AssetLoader {
public:
    virtual ~AssetLoader() = default;
    virtual void Load(AssetInfo const& assetInfo, Terran::Core::Shared<Asset>& asset) = 0;
    virtual bool Save(AssetInfo const& assetInfo, Terran::Core::Shared<Asset> const& asset) = 0;
};

class TextureAssetLoader final : public AssetLoader {
public:
    void Load(AssetInfo const& assetInfo, Terran::Core::Shared<Asset>& asset) override;
    bool Save(AssetInfo const& assetInfo, Terran::Core::Shared<Asset> const& asset) override;
    static Terran::Core::Shared<Texture2D> CreateTextureFromFile(std::filesystem::path const& textureSourcePath);
};

class TextAssetLoader final : public AssetLoader {
public:
    void Load(AssetInfo const& assetInfo, Terran::Core::Shared<Asset>& asset) override;
    bool Save(AssetInfo const& assetInfo, Terran::Core::Shared<Asset> const& asset) override;
};

class SceneAssetLoader final : public AssetLoader {
public:
    void Load(AssetInfo const& assetInfo, Terran::Core::Shared<Asset>& asset) override;
    bool Save(AssetInfo const& assetInfo, Terran::Core::Shared<Asset> const& asset) override;
};

class PhysicsMaterial2DAssetLoader final : public AssetLoader {
public:
    void Load(AssetInfo const& assetInfo, Terran::Core::Shared<Asset>& asset) override;
    bool Save(AssetInfo const& assetInfo, Terran::Core::Shared<Asset> const& asset) override;
};

}

