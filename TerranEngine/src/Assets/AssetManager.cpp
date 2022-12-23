#include "trpch.h"
#include "AssetManager.h"

#include "Loaders/TextureAssetLoader.h"

namespace TerranEngine 
{
    std::unordered_map<UUID, Shared<Asset>> AssetManager::s_LoadedAssets;
    std::unordered_map<UUID, AssetInfo> AssetManager::s_AssetsInfos;
    std::unordered_map<AssetType, Shared<AssetLoader>> AssetManager::s_Loaders;

    static AssetInfo s_EmptyAssetInfo;

    void AssetManager::Init()
    {
        s_LoadedAssets.clear();
        s_AssetsInfos.clear();
    }

    void AssetManager::Shutdown()
    {
        s_LoadedAssets.clear();
        s_AssetsInfos.clear();
    }

    AssetInfo& AssetManager::GetAssetInfo(const UUID& assetID)
    {
        if (s_AssetsInfos.find(assetID) != s_AssetsInfos.end())
            return s_AssetsInfos.at(assetID);

        return s_EmptyAssetInfo;
    }

    const AssetInfo& AssetManager::GetAssetInfo(const std::filesystem::path& assetPath)
    {
        for (auto [id, assetInfo] : s_AssetsInfos)
        {
            if (assetInfo.Path == assetPath)
                return assetInfo;
        }

        return { };
    }

    UUID AssetManager::ImportAsset(const std::filesystem::path& assetPath)
    {
        if (assetPath.empty()) return UUID::Invalid();

        std::filesystem::path absouluteAssetPath = std::filesystem::absolute(assetPath);

        AssetType type = AssetType::None;

        if (!std::filesystem::is_directory(absouluteAssetPath))
            type = AssetUtility::GetAssetTypeFromFileExtenstion(absouluteAssetPath.extension());
        else
            type = AssetType::Directory;

        if (type == AssetType::None) return UUID::Invalid();

        UUID assetID;

        AssetInfo assetInfo;
        assetInfo.Path = absouluteAssetPath;
        assetInfo.Type = type;

        s_AssetsInfos[assetID] = assetInfo;

        return assetID;
    }

    void AssetManager::WriteAssetInfos()
    {
    }

    void AssetManager::RegisterAssetLoaders()
    {
        s_Loaders[AssetType::Texture] = CreateShared<TextureAssetLoader>();
    }
}

