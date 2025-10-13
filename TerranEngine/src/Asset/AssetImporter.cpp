#include "trpch.h"
#include "AssetImporter.h"
#include "LibCore/Base.h"

namespace TerranEngine {

std::unordered_map<AssetType, Terran::Core::Shared<AssetLoader>> AssetImporter::s_Loaders;

void AssetImporter::RegisterLoaders()
{
    s_Loaders[AssetType::Texture2D] = Terran::Core::CreateShared<TextureAssetLoader>();
    s_Loaders[AssetType::Text] = Terran::Core::CreateShared<TextAssetLoader>();
    s_Loaders[AssetType::Scene] = Terran::Core::CreateShared<SceneAssetLoader>();
    s_Loaders[AssetType::PhysicsMaterial2D] = Terran::Core::CreateShared<PhysicsMaterial2DAssetLoader>();
}

void AssetImporter::Load(AssetInfo const& assetInfo, Terran::Core::Shared<Asset>& asset)
{
    if (s_Loaders.find(assetInfo.Type) != s_Loaders.end()) {
        s_Loaders[assetInfo.Type]->Load(assetInfo, asset);
        return;
    }

    TR_CORE_ERROR(TR_LOG_ASSET, "Invalid asset type for asset: {0}", assetInfo.Path);
}

bool AssetImporter::Save(AssetInfo const& assetInfo, Terran::Core::Shared<Asset> const& asset)
{
    if (s_Loaders.find(assetInfo.Type) != s_Loaders.end())
        return s_Loaders[assetInfo.Type]->Save(assetInfo, asset);

    TR_CORE_ERROR(TR_LOG_ASSET, "Invalid asset type for asset: {0}", assetInfo.Path);
    return false;
}

}
