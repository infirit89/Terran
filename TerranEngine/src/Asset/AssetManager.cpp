#include "trpch.h"
#include "AssetManager.h"
#include "Asset.h"
#include "LibCore/Base.h"
#include "LibCore/FileUtils.h"
#include "LibCore/Log.h"

#include "AssetImporter.h"
#include "AssetUtils.h"

#include "Project/Project.h"

#include <yaml-cpp/yaml.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace TerranEngine {

std::unordered_map<AssetHandle, Terran::Core::Shared<Asset>> AssetManager::s_LoadedAssets;
std::map<AssetHandle, AssetInfo> AssetManager::s_AssetsInfos;
AssetManager::AssetChangeCallbackFn AssetManager::s_ChangeCallback;

static AssetInfo s_EmptyAssetInfo;

void AssetManager::Initialize()
{
    Terran::Core::FileSystem::SetFileSystemChangeCallback(OnFileSystemChanged);

    s_LoadedAssets.clear();
    s_AssetsInfos.clear();
    TR_CORE_INFO(TR_LOG_ASSET, "Initialized asset manager");
}

void AssetManager::Shutdown()
{
    s_LoadedAssets.clear();
    s_AssetsInfos.clear();

    TR_CORE_INFO(TR_LOG_ASSET, "Shutdown asset manager");
}

AssetInfo& AssetManager::GetAssetInfoByHandle_Internal(AssetHandle const& assetID)
{
    if (s_AssetsInfos.find(assetID) != s_AssetsInfos.end())
        return s_AssetsInfos.at(assetID);

    return s_EmptyAssetInfo;
}

std::filesystem::path AssetManager::GetRelativePath(std::filesystem::path const& path)
{
    return std::filesystem::relative(path, Project::GetAssetPath());
}

AssetInfo const& AssetManager::GetAssetInfoByHandle(AssetHandle assetID)
{
    if (s_AssetsInfos.find(assetID) != s_AssetsInfos.end())
        return s_AssetsInfos[assetID];

    return s_EmptyAssetInfo;
}

AssetInfo const& AssetManager::GetAssetInfoByPath(std::filesystem::path const& assetPath)
{
    for (auto const& [id, assetInfo] : s_AssetsInfos) {
        if (assetInfo.Path == assetPath)
            return assetInfo;
    }

    return s_EmptyAssetInfo;
}

AssetHandle AssetManager::ImportAsset(std::filesystem::path const& assetPath)
{
    std::filesystem::path path = GetRelativePath(assetPath);
    AssetHandle assetHandle = GetAssetHandleFromPath(path);

    if (assetHandle)
        return assetHandle;

    assetHandle = AssetHandle();

    if (assetPath.empty())
        return AssetHandle::Invalid();

    AssetType type = AssetType::None;

    type = AssetUtility::GetAssetTypeFromFileExtenstion(assetPath.extension());

    if (type == AssetType::None)
        return AssetHandle::Invalid();

    AssetInfo assetInfo;
    assetInfo.Path = path;
    assetInfo.Type = type;
    assetInfo.Handle = assetHandle;

    s_AssetsInfos[assetHandle] = assetInfo;

    return assetHandle;
}

void AssetManager::WriteAssetInfosToFile()
{
    YAML::Emitter out;

    out << YAML::BeginMap;
    out << YAML::Key << "AssetInfos" << YAML::Value << YAML::BeginSeq;

    for (auto const& [assetID, assetInfo] : s_AssetsInfos) {
        out << YAML::BeginMap;
        out << YAML::Key << "Asset" << YAML::Value << std::to_string(assetInfo.Handle);
        out << YAML::Key << "Type" << YAML::Value << (uint32_t)assetInfo.Type;
        out << YAML::Key << "Path" << YAML::Value << assetInfo.Path.string();
        out << YAML::EndMap;
    }

    out << YAML::EndSeq;

    out << YAML::EndMap;

    std::ofstream ofs(Project::GetAssetInfoDBPath());
    ofs << out.c_str();
}

void AssetManager::LoadAssetInfos()
{
    YAML::Node node;

    try {
        std::filesystem::path assetInfoPath = Project::GetAssetInfoDBPath();
        node = YAML::LoadFile(assetInfoPath.string());
    } catch (YAML::Exception const& e) {
        TR_CORE_ERROR(TR_LOG_ASSET, e.what());
        return;
    }

    try {
        auto assetInfos = node["AssetInfos"];
        if (assetInfos) {
            for (auto const& assetInfo : assetInfos) {
                AssetInfo info;

                AssetHandle assetHandle = AssetHandle::FromString(assetInfo["Asset"].as<std::string>());

                info.Type = (AssetType)assetInfo["Type"].as<uint32_t>();
                info.Path = assetInfo["Path"].as<std::string>();
                info.Handle = assetHandle;

                if (!FileExists(info.Path) || info.Path.empty())
                    continue;

                s_AssetsInfos[assetHandle] = info;
            }
        }
    } catch (YAML::BadSubscript const& e) {
        TR_CORE_ERROR(TR_LOG_ASSET, e.what());
        return;
    }
}

void AssetManager::ReloadAssetByHandle(AssetHandle assetID)
{
    AssetInfo const& info = GetAssetInfoByHandle(assetID);
    if (s_LoadedAssets.find(assetID) == s_LoadedAssets.end()) {
        TR_CORE_WARN(TR_LOG_ASSET, "Trying to reload an asset that was never loaded");
        Terran::Core::Shared<Asset> asset;
        AssetImporter::Load(info, asset);
        return;
    }
    Terran::Core::Shared<Asset>& asset = s_LoadedAssets.at(assetID);
    AssetImporter::Load(info, asset);
}

void AssetManager::OnFileSystemChanged(std::vector<Terran::Core::FileSystemChangeEvent> const& fileSystemEvents)
{
    if (s_ChangeCallback)
        s_ChangeCallback(fileSystemEvents);

    for (auto e : fileSystemEvents) {
        if (std::filesystem::is_directory(e.FileName))
            continue;

        switch (e.Action) {
        case Terran::Core::FileAction::Removed: {
            OnAssetRemoved(GetAssetHandleFromPath(e.FileName));
            break;
        }
        case Terran::Core::FileAction::Renamed: {
            AssetType oldType = AssetUtility::GetAssetTypeFromFileExtenstion(e.OldFileName.extension());
            AssetType newType = AssetUtility::GetAssetTypeFromFileExtenstion(e.FileName.extension());

            if ((oldType == AssetType::None && newType != AssetType::None) || (oldType != AssetType::None && newType != AssetType::None && newType != oldType))
                ImportAsset(e.FileName);
            else
                OnAssetRenamed(GetAssetHandleFromPath(e.OldFileName), e.FileName);
            break;
        }
        case Terran::Core::FileAction::Modified: {
            AssetInfo info = GetAssetInfoByPath(e.FileName);

            if (info)
                ReloadAssetByHandle(info.Handle);

            break;
        }
        }
    }
}

AssetHandle AssetManager::GetAssetHandleFromPath(std::filesystem::path const& assetPath)
{
    for (auto const& [assetID, assetInfo] : s_AssetsInfos) {
        if (assetInfo.Path == assetPath)
            return assetID;
    }

    return AssetHandle::Invalid();
}

bool AssetManager::FileExists(std::filesystem::path const& path)
{
    return std::filesystem::exists(GetFileSystemPath(path));
}

std::filesystem::path AssetManager::GetFileSystemPath(std::filesystem::path const& path)
{
    return Project::GetAssetPath() / path;
}

void AssetManager::OnAssetRemoved(AssetHandle assetID)
{
    if (s_AssetsInfos.find(assetID) != s_AssetsInfos.end())
        s_AssetsInfos.erase(assetID);

    if (s_LoadedAssets.find(assetID) != s_LoadedAssets.end())
        s_LoadedAssets.erase(assetID);

    WriteAssetInfosToFile();
}

void AssetManager::OnAssetRenamed(AssetHandle assetID, std::filesystem::path const& newFileName)
{
    AssetInfo& info = GetAssetInfoByHandle_Internal(assetID);

    if (info != s_EmptyAssetInfo)
        info.Path = newFileName;

    WriteAssetInfosToFile();
}

}
