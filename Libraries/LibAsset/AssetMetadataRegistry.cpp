#include "AssetMetadataRegistry.h"
#include "AssetMetadata.h"
#include "AssetTypes.h"

#include <LibCore/Log.h>
#include <yaml-cpp/emitter.h>
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <fstream>
#include <map>
#include <string>

namespace Terran {
namespace Asset {

std::map<AssetHandle, AssetMetadata> AssetMetadataRegistry::s_asset_metadata;
static AssetMetadata s_invalid_asset_info;

AssetMetadata& AssetMetadataRegistry::asset_metadata_by_handle__internal(AssetHandle const& handle)
{
    if (s_asset_metadata.contains(handle))
        return s_asset_metadata.at(handle);

    return s_invalid_asset_info;
}

// std::filesystem::path AssetManager::relative_path(std::filesystem::path const& path)
// {
//     return std::filesystem::relative(path, Project::GetAssetPath());
// }

AssetMetadata const& AssetMetadataRegistry::asset_metadata_by_handle(AssetHandle const& handle)
{
    if (s_asset_metadata.contains(handle))
        return s_asset_metadata.at(handle);

    return s_invalid_asset_info;
}

AssetMetadata const& AssetMetadataRegistry::asset_metadata_by_path(std::filesystem::path const& assetPath)
{
    for (auto const& [handle, asset_metadata] : s_asset_metadata) {
        if (asset_metadata.Path == assetPath)
            return asset_metadata;
    }

    return s_invalid_asset_info;
}

AssetHandle AssetMetadataRegistry::asset_handle_from_path(std::filesystem::path const& assetPath)
{
    for (auto const& [handle, asset_metadata] : s_asset_metadata) {
        if (asset_metadata.Path == assetPath)
            return handle;
    }

    return AssetHandle::invalid();
}

void AssetMetadataRegistry::serialize_asset_metadata(YAML::Emitter& out)
{
    out << YAML::BeginMap;
    out << YAML::Key << "AssetInfos" << YAML::Value << YAML::BeginSeq;

    for (auto const& [assetID, assetInfo] : s_asset_metadata) {
        out << YAML::BeginMap;
        out << YAML::Key << "Asset" << YAML::Value << std::to_string(assetInfo.Handle);
        out << YAML::Key << "Type" << YAML::Value << assetInfo.Type;
        out << YAML::Key << "Path" << YAML::Value << assetInfo.Path.string();
        out << YAML::EndMap;
    }

    out << YAML::EndSeq;

    out << YAML::EndMap;
}

void AssetMetadataRegistry::deserialize_asset_metadata(YAML::Node const& node)
{
    try {
        auto assetInfos = node["AssetInfos"];
        if (assetInfos) {
            for (auto const& assetInfo : assetInfos) {
                AssetMetadata info;

                AssetHandle assetHandle = AssetHandle::from_string(assetInfo["Asset"].as<std::string>());

                info.Type = assetInfo["Type"].as<AssetTypeId>();
                info.Path = assetInfo["Path"].as<std::string>();
                info.Handle = assetHandle;

                if (info.Path.empty())
                    continue;

                s_asset_metadata[assetHandle] = info;
            }
        }
    } catch (YAML::BadSubscript const& e) {
        TR_CORE_ERROR(TR_LOG_ASSET, e.what());
        return;
    }
}

void AssetMetadataRegistry::write_asset_metadata_to_file(std::filesystem::path const& path)
{
    YAML::Emitter out;
    serialize_asset_metadata(out);
    std::ofstream ofs(path);
    ofs << out.c_str();
}

void AssetMetadataRegistry::load_asset_metadata_from_file(std::filesystem::path const& path)
{
    YAML::Node node;

    try {
        node = YAML::LoadFile(path.string());
    } catch (YAML::Exception const& e) {
        TR_CORE_ERROR(TR_LOG_ASSET, e.what());
        return;
    }

    deserialize_asset_metadata(node);
}

void AssetMetadataRegistry::erase(AssetHandle const& handle) {
    s_asset_metadata.erase(handle);
}

bool AssetMetadataRegistry::contains(AssetHandle const& handle) {
    return s_asset_metadata.contains(handle);
}

}
}
