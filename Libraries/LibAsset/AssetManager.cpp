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
#include <LibCore/UUID.h>
#include <LibCore/Result.h>

#include <filesystem>
#include <unordered_map>
#include <vector>

namespace Terran::Asset {

AssetManager::AssetManager(Core::EventDispatcher& event_dispatcher)
    : m_event_dispatcher(event_dispatcher)
{
    m_loaded_assets.clear();
    TR_CORE_INFO(TR_LOG_ASSET, "Initialized asset manager");
}

AssetManager::~AssetManager()
{
    m_loaded_assets.clear();

    TR_CORE_INFO(TR_LOG_ASSET, "Shutdown asset manager");
}

AssetHandle AssetManager::import_asset(std::filesystem::path const& asset_path) const
{
    AssetHandle asset_handle = AssetMetadataRegistry::asset_handle_from_path(asset_path);

    if (asset_handle)
        return asset_handle;

    asset_handle = AssetHandle();

    if (asset_path.empty())
        return AssetHandle::invalid();

    auto const asset_loader = AssetImporterRegistry::find_for_path(asset_path);
    if (asset_loader)
        return AssetHandle::invalid();

    AssetMetadata asset_metadata;
    asset_metadata.Path = asset_path;
    asset_metadata.Type = asset_loader->asset_type();
    asset_metadata.Handle = asset_handle;

    AssetMetadataRegistry::add_asset_metadata(asset_metadata);

    return asset_handle;
}

void AssetManager::reload_asset_by_handle(AssetHandle const& handle)
{
    AssetMetadata const& metadata = AssetMetadataRegistry::asset_metadata_by_handle(handle);

    if (!m_loaded_assets.contains(handle))
        TR_CORE_WARN(TR_LOG_ASSET, "Trying to reload an asset that was never loaded");

    AssetLoadResult asset_result = AssetImporterRegistry::load(metadata);
    if (!asset_result)
        return;

    m_loaded_assets[handle] = asset_result.value();
}

Core::Result<void, AssetRemoveError> AssetManager::remove_asset(Core::UUID const& handle, RemoveAssetImmediately remove_immediately, RemoveAssetMetadata remove_metadata)
{
    if(!m_loaded_assets.contains(handle))
        return { AssetRemoveError::AssetNotFound };

    if (remove_metadata == RemoveAssetMetadata::Yes && !AssetMetadataRegistry::contains(handle)) {
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

            if (metadata)
                reload_asset_by_handle(metadata.Handle);

            break;
        }
        }
    }
}

void AssetManager::on_asset_removed(AssetHandle const& handle)
{
    remove_asset(handle);

    AssetRemovedEvent removed_event(handle);
    m_event_dispatcher.trigger(removed_event);
}

void AssetManager::on_asset_renamed(AssetHandle const& handle, std::filesystem::path const& new_file_name)
{
    AssetMetadata& metadata = AssetMetadataRegistry::asset_metadata_by_handle__internal(handle);

    AssetRenamedEvent renamed_event(handle, new_file_name, metadata.Path);

    if (metadata)
        metadata.Path = new_file_name;

    m_event_dispatcher.trigger(renamed_event);
}

void AssetManager::enqueue_asset_for_deletion(AssetHandle const& handle) {
    m_free_queue.emplace_back(handle);
}

}
