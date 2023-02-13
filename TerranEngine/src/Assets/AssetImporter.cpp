#include "trpch.h"
#include "AssetImporter.h"

namespace TerranEngine 
{
    std::unordered_map<AssetType, Shared<AssetLoader>> AssetImporter::s_Loaders;

    void AssetImporter::RegisterLoaders()
    {
        s_Loaders[AssetType::Texture] = CreateShared<TextureAssetLoader>();
        s_Loaders[AssetType::Text] = CreateShared<TextAssetLoader>();
        s_Loaders[AssetType::Scene] = CreateShared<SceneAssetLoader>();
        s_Loaders[AssetType::PhysicsMaterial2D] = CreateShared<PhysicsMaterial2DAssetLoader>();
    }

    void AssetImporter::Load(const AssetInfo& assetInfo, Shared<Asset>& asset)
    {
        if (s_Loaders.find(assetInfo.Type) != s_Loaders.end()) 
        {
            s_Loaders[assetInfo.Type]->Load(assetInfo, asset);
            return;
        }

        TR_ERROR("Invalid asset type for asset: {0}", assetInfo.Path);
    }

    bool AssetImporter::Save(const AssetInfo& assetInfo, const Shared<Asset>& asset)
    {
        if (s_Loaders.find(assetInfo.Type) != s_Loaders.end())
            return s_Loaders[assetInfo.Type]->Save(assetInfo, asset);

        TR_ERROR("Invalid asset type for asset: {0}", assetInfo.Path);
        return false;
    }
}

