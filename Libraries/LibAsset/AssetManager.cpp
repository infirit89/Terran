#include "AssetManager.h"
#include "Asset.h"
#include "AssetEvents.h"
#include "AssetImporter.h"
#include "AssetImporterRegistry.h"
#include "AssetMetadata.h"
#include "AssetMetadataRegistry.h"
#include "AssetTypes.h"

#include <LibCore/Event.h>
#include <LibCore/FileUtils.h>
#include <LibCore/Log.h>
#include <LibCore/RefPtr.h>
#include <LibCore/Result.h>
#include <LibCore/UUID.h>

#include <filesystem>
#include <unordered_map>
#include <vector>

namespace Terran::Asset {

AssetManager::AssetManager(Core::EventDispatcher& event_dispatcher)
    : m_event_dispatcher(event_dispatcher)
{
    m_loaded_assets.clear();
    TR_INFO(TR_LOG_ASSET, "Initialized asset manager");
}

AssetManager::~AssetManager()
{
    m_loaded_assets.clear();

    TR_INFO(TR_LOG_ASSET, "Shutdown asset manager");
}

Core::RefPtr<AssetHandle> AssetManager::import_asset(std::filesystem::path const& asset_path) const
{
    AssetId id = AssetMetadataRegistry::asset_handle_from_path(asset_path);

    if (id) {
        return create_asset_handle(id);
    }

    id = AssetId();

    if (asset_path.empty()) {
        TR_ERROR(TR_LOG_ASSET, "Failed to import asset, due to empty asset path!");
        return nullptr;
    }

    auto const asset_loader = AssetImporterRegistry::find_for_path(asset_path);
    if (asset_loader) {
        TR_ERROR(TR_LOG_ASSET, "No registered loaders for asset with path {}", asset_path);
        return nullptr;
    }

    AssetMetadata asset_metadata;
    asset_metadata.Path = asset_path;
    asset_metadata.Type = asset_loader->asset_type();
    asset_metadata.AssetId = id;

    AssetMetadataRegistry::add_asset_metadata(asset_metadata);

    return create_asset_handle(id);
}

void AssetManager::reload_asset_by_id(AssetId const& asset_id)
{
    AssetMetadata const& metadata = AssetMetadataRegistry::asset_metadata_by_handle(asset_id);

    if (!m_loaded_assets.contains(asset_id))
        TR_WARN(TR_LOG_ASSET, "Trying to reload an asset that was never loaded");

    AssetLoadResult asset_result = AssetImporterRegistry::load(metadata);
    if (!asset_result) {
        TR_ERROR(TR_LOG_ASSET, "Failed to load asset with Id: {} and Path: {}", asset_id, metadata.AssetId);
        return;
    }

    m_loaded_assets[asset_id] = asset_result.value();
}

Core::Result<void, AssetRemoveError> AssetManager::remove_asset(Core::UUID const& handle, RemoveAssetImmediately remove_immediately, RemoveAssetMetadata remove_metadata)
{
    if (!m_loaded_assets.contains(handle)) {
        TR_ERROR(TR_LOG_ASSET, "Asset with id {} wasn't found!", handle);
        return { AssetRemoveError::AssetNotFound };
    }

    if (remove_metadata == RemoveAssetMetadata::Yes && !AssetMetadataRegistry::contains(handle)) {
        TR_ERROR(TR_LOG_ASSET, "Asset metadata with id {} wasn't found!", handle);
        return { AssetRemoveError::MetadatNotFound };
    }

    if (remove_immediately == RemoveAssetImmediately::Yes) {
        m_loaded_assets.erase(handle);
    } else {
        m_free_queue.emplace_back(handle);
    }

    if (remove_metadata == RemoveAssetMetadata::Yes) {
        AssetMetadataRegistry::erase(handle);
    }

    return {};
}

void AssetManager::on_filesystem_changed(std::vector<Terran::Core::FileSystemChangeEvent> const& file_system_events)
{
    if (m_asset_change_callback)
        m_asset_change_callback(file_system_events);

    for (auto const& event : file_system_events) {
        if (std::filesystem::is_directory(event.FileName))
            continue;

        switch (event.Action) {
        case Core::FileAction::Removed: {
            on_asset_removed(AssetMetadataRegistry::asset_handle_from_path(event.FileName));
            break;
        }
        case Core::FileAction::Renamed: {
            bool was_asset = AssetImporterRegistry::exists_for_path(event.OldFileName);
            bool is_asset = AssetImporterRegistry::exists_for_path(event.FileName);

            if (!was_asset && is_asset) {
                import_asset(event.FileName);
            } else if (was_asset && !is_asset) {
                on_asset_removed(
                    AssetMetadataRegistry::asset_handle_from_path(event.OldFileName));
            } else {
                on_asset_renamed(
                    AssetMetadataRegistry::asset_handle_from_path(event.OldFileName), event.FileName);
            }
            break;
        }
        case Core::FileAction::Modified: {
            AssetMetadata metadata = AssetMetadataRegistry::asset_metadata_by_path(event.FileName);

            if (!metadata) {
                TR_WARN(TR_LOG_ASSET, "Asset metadata wasn't found for asset with path: {}", event.FileName);
                break;
            }

            reload_asset_by_id(metadata.AssetId);

            break;
        }
        }
    }
}

void AssetManager::on_asset_removed(AssetId const& handle)
{
    remove_asset(handle);

    AssetRemovedEvent removed_event(handle);
    m_event_dispatcher.trigger(removed_event);
}

void AssetManager::on_asset_renamed(AssetId const& handle, std::filesystem::path const& new_file_name)
{
    AssetMetadata& metadata = AssetMetadataRegistry::asset_metadata_by_handle__internal(handle);

    AssetRenamedEvent renamed_event(handle, new_file_name, metadata.Path);

    if (metadata)
        metadata.Path = new_file_name;

    m_event_dispatcher.trigger(renamed_event);
}

void AssetManager::enqueue_asset_for_deletion(AssetId const& handle) const
{
    m_free_queue.emplace_back(handle);
}

void AssetManager::purge_stale()
{
    while (!m_free_queue.empty()) {
        AssetId const& id = m_free_queue.front();
        if (m_loaded_assets.contains(id)) {
            m_loaded_assets.erase(id);
        } else {
            TR_ERROR(TR_LOG_ASSET, "Trying to purge an asset {} which is not loaded!", id);
        }

        m_free_queue.pop_front();
    }
}

}
