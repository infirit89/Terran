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
#include <LibCore/RefPtr.h>
#include <LibCore/UUID.h>
#include <LibCore/Result.h>

#include <cstdint>
#include <deque>
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

enum class RemoveAssetImmediately : uint8_t {
    No = 0,
    Yes = 1,
};

enum class AssetRemoveError : uint8_t {
    AssetNotFound,
    MetadatNotFound
};

class AssetManager final {
    using AssetChangeCallbackFn = std::function<void(std::vector<Core::FileSystemChangeEvent> const&)>;
    using asset_container_type = std::unordered_map<AssetHandle, Core::RefPtr<Asset>>;
    using free_queue_type = std::deque<AssetHandle>;

public:
    AssetManager(Core::EventDispatcher& event_dispatcher);
    ~AssetManager();

    std::filesystem::path filesystem_path(std::filesystem::path const& path);

    AssetHandle import_asset(std::filesystem::path const& asset_path) const;
    void reload_asset_by_handle(AssetHandle const& handle);

    void SetAssetChangedCallback(AssetChangeCallbackFn const& callback) { m_asset_change_callback = callback; }

    template<typename TAsset>
    requires(std::is_base_of_v<Asset, TAsset>)
    Core::RefPtr<TAsset> asset_by_handle(AssetHandle const& assetHandle)
    {
        if (m_loaded_assets.contains(assetHandle))
            return Core::dynamic_pointer_cast<TAsset>(m_loaded_assets.at(assetHandle));

        AssetMetadata& info = AssetMetadataRegistry::asset_metadata_by_handle__internal(assetHandle);

        if (!info)
            return nullptr;

        AssetLoadResult assetResult = AssetImporterRegistry::load(info);

        if (!assetResult) {
            TR_CORE_ERROR(TR_LOG_ASSET, "Failed to load asset with path: {0}", info.Path);
            return nullptr;
        }

        Core::RefPtr<Asset> const& asset = assetResult.value();
        asset->m_handle = assetHandle;
        m_loaded_assets[assetHandle] = asset;
        return Core::DynamicCast<TAsset>(m_loaded_assets[assetHandle]);
    }

    template<typename TAsset>
    requires(std::is_base_of_v<Asset, TAsset>)
    Core::RefPtr<TAsset> asset_by_metadata(AssetMetadata const& assetMetadata)
    {
        if (m_loaded_assets.contains(assetMetadata.Handle))
            return Core::dynamic_pointer_cast<TAsset>(m_loaded_assets.at(assetMetadata.Handle));

        AssetLoadResult assetResult = AssetImporterRegistry::load(assetMetadata);

        if (!assetResult) {
            TR_CORE_ERROR(TR_LOG_ASSET, "Failed to load asset with path: {0}", assetMetadata.Path);
            return nullptr;
        }

        Core::RefPtr<Asset> const& asset = assetResult.value();
        asset->m_handle = assetMetadata.Handle;
        m_loaded_assets[assetMetadata.Handle] = asset;
        return Core::DynamicCast<TAsset>(m_loaded_assets[assetMetadata.Handle]);
    }

    template<typename TAsset, typename... TArgs>
    requires(
        std::is_base_of_v<Asset, TAsset>,
        HasStaticType<TAsset>)
    Core::RefPtr<TAsset> create_asset(std::filesystem::path const& file_path, TArgs&&... args)
    {
        AssetMetadata metadata;
        metadata.Handle = AssetHandle();
        metadata.Path = file_path;
        metadata.Type = TAsset::static_type();

        int file_occurrence_count = 2;
        while (std::filesystem::exists(metadata.Path)) {
            // Format: parent_directory/file_name (occurrence_count).extension
            metadata.Path = std::format("{} ({}){}", (file_path.parent_path() / file_path.stem()).string(), std::to_string(file_occurrence_count), file_path.extension().string());

            file_occurrence_count++;
        }

        AssetMetadataRegistry::add_asset_metadata(metadata);

        Core::RefPtr<TAsset> asset = Core::RefPtr<TAsset>::create(std::forward<TArgs>(args)...);

        m_loaded_assets[metadata.Handle] = asset;
        AssetImporterRegistry::save(metadata, asset);

        return Core::dynamic_pointer_cast<TAsset>(m_loaded_assets[metadata.Handle]);
    }

    // NOTE: maybe we should take in a parameter that signifies 
    // whether to create metadata for this asset
    // something like create_memory_asset(CreateAssetMetadata::Yes)
    template<typename TAsset, typename... TArgs>
    requires(std::is_base_of_v<Asset, TAsset>)
    Core::RefPtr<TAsset> create_memory_asset(TArgs&&... args)
    {
        Core::RefPtr<Asset> asset = Core::RefPtr<TAsset>::create(std::forward<TArgs>(args)...);
        m_loaded_assets[asset->m_handle] = asset;

        return Core::dynamic_pointer_cast<TAsset>(m_loaded_assets[asset->m_handle]);
    }

    Core::Result<void, AssetRemoveError> remove_asset(Core::UUID const& handle, RemoveAssetImmediately remove_immediately = RemoveAssetImmediately::Yes, RemoveAssetMetadata remove_metadata = RemoveAssetMetadata::Yes);

private:
    void on_filesystem_changed(std::vector<Core::FileSystemChangeEvent> const& file_system_events);
    void on_asset_removed(AssetHandle const& handle);
    void on_asset_renamed(AssetHandle const& handle, std::filesystem::path const& new_file_name);
    void enqueue_asset_for_deletion(AssetHandle const& handle);

private:
    asset_container_type m_loaded_assets;
    AssetChangeCallbackFn m_asset_change_callback;
    Core::EventDispatcher& m_event_dispatcher;
    //NOTE: this needs to be a multithreaded queue, either with or without a lock!!!
    //This works for now because the engine is not multithreaded
    //Will cause a bunch of problems if not changed and the engine goes multithreaded!!!
    free_queue_type m_free_queue;
    friend class Asset;
};

}
