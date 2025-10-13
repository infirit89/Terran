#pragma once

#include "Asset.h"
#include "AssetImporter.h"

#include "LibCore/Base.h"
#include "LibCore/FileUtils.h"
#include "LibCore/Log.h"
#include "LibCore/UUID.h"

#include <filesystem>
#include <functional>
#include <map>
#include <unordered_map>
#include <vector>

namespace TerranEngine {

class AssetManager final {
    using AssetInfoMap = std::map<Terran::Core::UUID, AssetInfo>;
    using AssetChangeCallbackFn = std::function<void(std::vector<Terran::Core::FileSystemChangeEvent> const&)>;

public:
    static void Initialize();
    static void Shutdown();

    static AssetInfo const& GetAssetInfoByHandle(AssetHandle assetHandle);
    static AssetInfo const& GetAssetInfoByPath(std::filesystem::path const& assetPath);
    static AssetHandle GetAssetHandleFromPath(std::filesystem::path const& assetPath);

    static std::filesystem::path GetFileSystemPath(std::filesystem::path const& path);

    static AssetInfoMap& GetAssetInfoMap() { return s_AssetsInfos; }

    static AssetHandle ImportAsset(std::filesystem::path const& assetPath);
    static void ReloadAssetByHandle(AssetHandle assetHandle);

    static void SetAssetChangedCallback(AssetChangeCallbackFn const& callback) { s_ChangeCallback = callback; }

    template<typename T>
    static Terran::Core::Shared<T> GetAssetByHandle(AssetHandle const& assetHandle)
    {
        if (s_LoadedAssets.find(assetHandle) != s_LoadedAssets.end())
            return Terran::Core::DynamicCast<T>(s_LoadedAssets.at(assetHandle));

        AssetInfo& info = GetAssetInfoByHandle_Internal(assetHandle);

        if (!info)
            return nullptr;

        // NOTE: poc code
        Terran::Core::Shared<Asset> asset = nullptr;
        AssetImporter::Load(info, asset);

        if (!asset) {
            TR_CORE_ERROR(TR_LOG_ASSET, "Failed to load asset with path: {0}", info.Path);
            return nullptr;
        }

        asset->m_Handle = assetHandle;
        s_LoadedAssets[assetHandle] = asset;
        return Terran::Core::DynamicCast<T>(s_LoadedAssets[assetHandle]);
    }

    template<typename T>
    static Terran::Core::Shared<T> GetAssetByAssetInfo(AssetInfo const& assetInfo)
    {
        if (s_LoadedAssets.contains(assetInfo.Handle))
            return Terran::Core::DynamicCast<T>(s_LoadedAssets.at(assetInfo.Handle));

        // NOTE: poc code
        Terran::Core::Shared<Asset> asset = nullptr;
        AssetImporter::Load(assetInfo, asset);

        if (!asset) {
            TR_CORE_ERROR(TR_LOG_ASSET, "Failed to load asset with path: {0}", assetInfo.Path);
            return nullptr;
        }

        asset->m_Handle = assetInfo.Handle;
        s_LoadedAssets[assetInfo.Handle] = asset;
        return Terran::Core::DynamicCast<T>(s_LoadedAssets[assetInfo.Handle]);
    }

    template<typename T>
    static Terran::Core::Shared<T> CreateNewAsset(std::filesystem::path const& filePath)
    {
        AssetInfo info;
        info.Handle = AssetHandle();
        info.Path = filePath;
        info.Type = T::GetStaticType();

        int currentFileNumber = 2;
        while (FileExists(info.Path)) {
            info.Path = filePath.parent_path() / filePath.stem();

            info.Path = info.Path.string() + " (" + std::to_string(currentFileNumber) + ")" + filePath.extension().string();

            currentFileNumber++;
        }

        s_AssetsInfos[info.Handle] = info;

        // TODO: add parameter options
        Terran::Core::Shared<T> asset = Terran::Core::CreateShared<T>();

        s_LoadedAssets[info.Handle] = asset;
        AssetImporter::Save(info, asset);

        return Terran::Core::DynamicCast<T>(s_LoadedAssets[info.Handle]);
    }

    template<typename T>
    static Terran::Core::Shared<T> CreateMemoryAsset()
    {
        Terran::Core::Shared<Asset> asset = Terran::Core::CreateShared<T>();
        s_LoadedAssets[asset->m_Handle] = asset;

        return Terran::Core::DynamicCast<T>(s_LoadedAssets[asset->m_Handle]);
    }

    static void LoadAssetInfos();
    static void WriteAssetInfosToFile();

    static bool FileExists(std::filesystem::path const& path);

private:
    static void OnFileSystemChanged(std::vector<Terran::Core::FileSystemChangeEvent> const& fileSystemEvents);
    static void OnAssetRemoved(AssetHandle assetID);
    static void OnAssetRenamed(AssetHandle assetID, std::filesystem::path const& newFileName);
    static AssetInfo& GetAssetInfoByHandle_Internal(AssetHandle const& assetID);
    static std::filesystem::path GetRelativePath(std::filesystem::path const& path);

private:
    static std::unordered_map<AssetHandle, Terran::Core::Shared<Asset>> s_LoadedAssets;
    static AssetInfoMap s_AssetsInfos;
    static AssetChangeCallbackFn s_ChangeCallback;
};

}
