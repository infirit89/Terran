#pragma once

#include "Asset.h"
#include "AssetImporterRegistry.h"
#include "AssetMetadata.h"
#include "AssetTypes.h"
#include "AssetImporter.h"
#include "AssetMetadataRegistry.h"

#include <LibCore/Base.h>
#include <LibCore/Event.h>
#include <LibCore/FileUtils.h>
#include <LibCore/Log.h>
#include <LibCore/UUID.h>

#include <cstdint>
#include <filesystem>
#include <format>
#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace Terran::Asset {

enum class RemoveAssetMetadata : uint8_t {
    No = 0,
    Yes = 1
};

class AssetManager final {
    using AssetChangeCallbackFn = std::function<void(std::vector<Core::FileSystemChangeEvent> const&)>;
    using asset_container = std::unordered_map<AssetHandle, Core::Shared<Asset>>;

public:
    AssetManager(Core::EventDispatcher& event_dispatcher);
    ~AssetManager();

    std::filesystem::path filesystem_path(std::filesystem::path const& path);

    AssetHandle import_asset(std::filesystem::path const& asset_path);
    void reload_asset_by_handle(AssetHandle const& handle);

    void SetAssetChangedCallback(AssetChangeCallbackFn const& callback) { m_asset_change_callback = callback; }

    template<typename TAsset>
    requires(std::is_base_of_v<Asset, TAsset>)
    Core::Shared<TAsset> asset_by_handle(AssetHandle const& assetHandle)
    {
        if (m_loaded_assets.contains(assetHandle))
            return Core::DynamicCast<TAsset>(m_loaded_assets.at(assetHandle));

        AssetMetadata& info = AssetMetadataRegistry::asset_metadata_by_handle__internal(assetHandle);

        if (!info)
            return nullptr;

        AssetLoadResult assetResult = AssetImporterRegistry::load(info);

        if (!assetResult) {
            TR_CORE_ERROR(TR_LOG_ASSET, "Failed to load asset with path: {0}", info.Path);
            return nullptr;
        }

        Core::Shared<Asset> const& asset = assetResult.value();
        asset->m_handle = assetHandle;
        m_loaded_assets[assetHandle] = asset;
        return Core::DynamicCast<TAsset>(m_loaded_assets[assetHandle]);
    }

    template<typename TAsset>
    requires(std::is_base_of_v<Asset, TAsset>)
    Core::Shared<TAsset> asset_by_metadata(AssetMetadata const& assetMetadata)
    {
        if (m_loaded_assets.contains(assetMetadata.Handle))
            return Core::DynamicCast<TAsset>(m_loaded_assets.at(assetMetadata.Handle));

        AssetLoadResult assetResult = AssetImporterRegistry::load(assetMetadata);

        if (!assetResult) {
            TR_CORE_ERROR(TR_LOG_ASSET, "Failed to load asset with path: {0}", assetMetadata.Path);
            return nullptr;
        }

        Core::Shared<Asset> const& asset = assetResult.value();
        asset->m_handle = assetMetadata.Handle;
        m_loaded_assets[assetMetadata.Handle] = asset;
        return Core::DynamicCast<TAsset>(m_loaded_assets[assetMetadata.Handle]);
    }

    template<typename TAsset, typename... TArgs>
    requires(
        std::is_base_of_v<Asset, TAsset>,
        HasStaticType<TAsset>)
    Core::Shared<TAsset> create_asset(std::filesystem::path const& file_path, TArgs&&... args)
    {
        AssetMetadata metadata;
        metadata.Handle = AssetHandle();
        metadata.Path = file_path;
        metadata.Type = TAsset::static_type();

        int file_occurrence_count = 2;
        while (file_exists(metadata.Path)) {
            // Format: parent_directory/file_name (occurrence_count).extension
            metadata.Path = std::format("{0} ({1}){2}", (file_path.parent_path() / file_path.stem()).string(), std::to_string(file_occurrence_count), file_path.extension().string());

            file_occurrence_count++;
        }

        AssetMetadataRegistry::add_asset_metadata(metadata);

        Core::Shared<TAsset> asset = Core::CreateShared<TAsset>(std::forward<TArgs>(args)...);

        m_loaded_assets[metadata.Handle] = asset;
        AssetImporterRegistry::save(metadata, asset);

        return Core::DynamicCast<TAsset>(m_loaded_assets[metadata.Handle]);
    }

    // NOTE: maybe we should take in a parameter that signifies 
    // whether to create metadata for this asset
    // something like create_memory_asset(CreateAssetMetadata::Yes)
    template<typename TAsset, typename... TArgs>
    requires(std::is_base_of_v<Asset, TAsset>)
    Core::Shared<TAsset> create_memory_asset(TArgs&&... args)
    {
        Core::Shared<Asset> asset = Core::CreateShared<TAsset>(std::forward<TArgs>(args)...);
        m_loaded_assets[asset->m_handle] = asset;

        return Core::DynamicCast<TAsset>(m_loaded_assets[asset->m_handle]);
    }

    void remove_asset(Core::UUID const& handle, RemoveAssetMetadata remove_metadata = RemoveAssetMetadata::Yes);

    static bool file_exists(std::filesystem::path const& path);

private:
    void on_filesystem_changed(std::vector<Core::FileSystemChangeEvent> const& file_system_events);
    void on_asset_removed(AssetHandle const& handle);
    void on_asset_renamed(AssetHandle const& handle, std::filesystem::path const& new_file_name);
    std::filesystem::path relative_path(std::filesystem::path const& path);

private:
    asset_container m_loaded_assets;
    AssetChangeCallbackFn m_asset_change_callback;
    Core::EventDispatcher& m_event_dispatcher;
};

}
