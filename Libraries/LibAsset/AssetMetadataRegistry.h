#pragma once

#include "AssetMetadata.h"
#include "AssetTypes.h"

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <map>

namespace Terran {
namespace Asset {

class AssetMetadataRegistry final {
    using AssetMetadataMap = std::map<AssetHandle, AssetMetadata>;

public:
    static AssetMetadata const& asset_metadata_by_handle(AssetHandle const& assetHandle);

    static AssetMetadata const& asset_metadata_by_path(std::filesystem::path const& assetPath);

    static AssetHandle asset_handle_from_path(std::filesystem::path const& assetPath);

    static void add_asset_metadata(AssetMetadata const& metadata);

    static AssetMetadataMap& asset_metadata_map()
    {
        return s_asset_metadata;
    }

    static void load_asset_metadata_from_file(std::filesystem::path const& path);

    static void write_asset_metadata_to_file(std::filesystem::path const& path);

    static void serialize_asset_metadata(YAML::Emitter& out);

    static void deserialize_asset_metadata(YAML::Node const& node);

    static void erase(AssetHandle const& handle);

    static bool contains(AssetHandle const& handle);

    static void clear()
    {
        s_asset_metadata.clear();
    }

private:
    static AssetMetadata& asset_metadata_by_handle__internal(AssetHandle const& handle);

    static AssetMetadataMap s_asset_metadata;

    friend class AssetManager;
};

}
}
