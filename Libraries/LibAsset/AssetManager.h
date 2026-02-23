#pragma once

#include "Asset.h"
#include "AssetImporter.h"
#include "AssetImporterRegistry.h"
#include "AssetMetadata.h"
#include "AssetMetadataRegistry.h"
#include "AssetTypes.h"
#include "AssetHandle.h"

#include <LibCore/Base.h>
#include <LibCore/Event.h>
#include <LibCore/FileUtils.h>
#include <LibCore/Log.h>
#include <LibCore/RefPtr.h>
#include <LibCore/Result.h>
#include <LibCore/UUID.h>

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
    using asset_container_type = std::unordered_map<AssetId, Core::RefPtr<Asset>>;
    using free_queue_type = std::deque<AssetId>;

public:
    AssetManager(Core::EventDispatcher& event_dispatcher);
    ~AssetManager();

    StrongAssetHandle import_asset(std::filesystem::path const& asset_path) const;

    void reload_asset_by_id(AssetId const& asset_id);
    void reload_asset_by_handle(StrongAssetHandle const& asset_handle) {
        reload_asset_by_id(asset_handle->id());
    }

    void SetAssetChangedCallback(AssetChangeCallbackFn const& callback) { m_asset_change_callback = callback; }

    template<typename TAsset>
    requires(std::is_base_of_v<Asset, TAsset>)
    Core::RefPtr<TAsset> asset_by_handle(StrongAssetHandle const& asset_handle)
    {
        if (m_loaded_assets.contains(asset_handle->id()))
            return asset_by_id<TAsset>(asset_handle->id());

        AssetMetadata& info = AssetMetadataRegistry::asset_metadata_by_handle__internal(asset_handle->id());

        if (!info)
            return nullptr;

        AssetLoadResult assetResult = AssetImporterRegistry::load(info);

        if (!assetResult) {
            TR_ERROR(TR_LOG_ASSET, "Failed to load asset with path: {0}", info.Path);
            return nullptr;
        }

        Core::RefPtr<Asset> const& asset = assetResult.value();
        asset->m_id = asset_handle->id();
        m_loaded_assets[asset_handle->id()] = asset;
        return Core::dynamic_pointer_cast<TAsset>(asset);
    }

    template<typename TAsset>
    requires(std::is_base_of_v<Asset, TAsset>)
    Core::RefPtr<TAsset> asset_by_metadata(AssetMetadata const& asset_metadata)
    {
        if (m_loaded_assets.contains(asset_metadata.AssetId))
            return asset_by_id<TAsset>(asset_metadata.AssetId);

        AssetLoadResult asset_result = AssetImporterRegistry::load(asset_metadata);

        if (!asset_result) {
            TR_ERROR(TR_LOG_ASSET, "Failed to load asset with path: {0}", asset_metadata.Path);
            return nullptr;
        }

        Core::RefPtr<Asset> const& asset = asset_result.value();
        asset->m_id = asset_metadata.AssetId;
        m_loaded_assets[asset_metadata.AssetId] = asset;
        return Core::dynamic_pointer_cast<TAsset>(asset);
    }

    template<typename TAsset, typename... TArgs>
    requires(
        std::is_base_of_v<Asset, TAsset>,
        HasStaticType<TAsset>)
    void save_asset(Core::RefPtr<TAsset> asset)
    {
        if (!AssetMetadataRegistry::contains(asset->id())) {
            TR_ERROR(TR_LOG_ASSET, "Failed to save asset {} as there is no metadata associated with it", asset->id());
            return;
        }
        AssetMetadata& metadata = AssetMetadataRegistry::asset_metadata_by_handle__internal(asset->id());

        int file_occurrence_count = 2;
        while (std::filesystem::exists(metadata.Path)) {
            // Format: parent_directory/file_name (occurrence_count).extension
            std::filesystem::path file_path_without_extension = metadata.Path.parent_path() / metadata.Path.stem();
            std::filesystem::path file_extension = metadata.Path.extension();
            metadata.Path = std::format("{} ({}){}",
                file_path_without_extension.string(),
                file_occurrence_count,
                file_extension.string());

            file_occurrence_count++;
        }

        AssetImporterRegistry::save(metadata, asset);
    }

    // NOTE: maybe we should take in a parameter that signifies
    // whether to create metadata for this asset
    // something like create_memory_asset(CreateAssetMetadata::Yes)
    StrongAssetHandle add_asset(Core::RefPtr<Asset> const& asset)
    {
        m_loaded_assets[asset->id()] = asset;
        return create_asset_handle(asset->id());
    }

    template<typename TAsset, typename... TArgs>
    requires(std::is_base_of_v<Asset, TAsset>)
    Core::RefPtr<TAsset> create_asset(TArgs&&... args)
    {
        Core::RefPtr<Asset> asset = Core::RefPtr<TAsset>::create(std::forward<TArgs>(args)...);
        return Core::dynamic_pointer_cast<TAsset>(asset);
    }

    template<typename TAsset, typename... TArgs>
    requires(
        std::is_base_of_v<Asset, TAsset>,
        HasStaticType<TAsset>)
    AssetMetadata create_asset_metadata(AssetId const& asset_id, std::filesystem::path const& file_path)
    {
        AssetMetadata metadata;
        metadata.AssetId = asset_id;
        metadata.Path = file_path;
        metadata.Type = TAsset::static_type();
    }

    Core::Result<void, AssetRemoveError> remove_asset(Core::UUID const& handle, RemoveAssetImmediately remove_immediately = RemoveAssetImmediately::Yes, RemoveAssetMetadata remove_metadata = RemoveAssetMetadata::Yes);

    constexpr bool is_asset_loaded(AssetId const& id) const {
        return m_loaded_assets.contains(id);
    }

    void purge_stale();

private:
    void on_filesystem_changed(std::vector<Core::FileSystemChangeEvent> const& file_system_events);
    void on_asset_removed(AssetId const& handle);
    void on_asset_renamed(AssetId const& handle, std::filesystem::path const& new_file_name);
    void enqueue_asset_for_deletion(AssetId const& handle) const;

    template<typename TAsset>
    requires(std::is_base_of_v<Asset, TAsset>)
    inline void asset_by_id(AssetId const& id)
    {
        return Core::dynamic_pointer_cast<TAsset>(m_loaded_assets.at(id));
    }

    inline StrongAssetHandle create_asset_handle(AssetId const& asset_id) const
    {
        return StrongAssetHandle::create(asset_id, this);
    }

private:
    asset_container_type m_loaded_assets;
    AssetChangeCallbackFn m_asset_change_callback;
    Core::EventDispatcher& m_event_dispatcher;
    // NOTE: this needs to be a multithreaded queue, either with or without a lock!!!
    // This works for now because the engine is not multithreaded
    // Will cause a bunch of problems if not changed and the engine goes multithreaded!!!
    mutable free_queue_type m_free_queue;
    friend class Asset;
    friend class AssetHandle;
};

}
