#include "trpch.h"
#include "AssetManager.h"

namespace TerranEngine 
{
    std::unordered_map<UUID, Shared<Asset>> AssetManager::s_LoadedAssets;
    std::unordered_map<UUID, AssetInfo> AssetManager::s_AssetsInfos;

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
}

