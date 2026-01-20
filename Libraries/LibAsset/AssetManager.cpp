#include "AssetManager.h"
#include "Asset.h"
#include "AssetImporterRegistry.h"
#include "AssetMetadata.h"
#include "AssetMetadataRegistry.h"
#include "AssetTypes.h"

#include "LibCore/Base.h"
#include "LibCore/FileUtils.h"
#include "LibCore/Log.h"

#include <LibAsset/AssetEvents.h>
#include <LibCore/Event.h>
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

AssetHandle AssetManager::import_asset(std::filesystem::path const& assetPath)
{
    AssetHandle assetHandle = AssetMetadataRegistry::asset_handle_from_path(assetPath);

    if (assetHandle)
        return assetHandle;

    assetHandle = AssetHandle();

    if (assetPath.empty())
        return AssetHandle::invalid();

    auto const assetLoader = AssetImporterRegistry::find_for_path(assetPath);
    if (assetLoader)
        return AssetHandle::invalid();

    AssetMetadata assetMetadata;
    assetMetadata.Path = assetPath;
    assetMetadata.Type = assetLoader->asset_type();
    assetMetadata.Handle = assetHandle;

    AssetMetadataRegistry::add_asset_metadata(assetMetadata);

    return assetHandle;
}

void AssetManager::reload_asset_by_handle(AssetHandle const& handle)
{
    AssetMetadata const& info = AssetMetadataRegistry::asset_metadata_by_handle(handle);
    if (!m_loaded_assets.contains(handle)) {
        TR_CORE_WARN(TR_LOG_ASSET, "Trying to reload an asset that was never loaded");
        Terran::Core::Shared<Asset> asset;
        AssetImporterRegistry::load(info, asset);
        return;
    }
    Terran::Core::Shared<Asset>& asset = m_loaded_assets.at(handle);
    AssetImporterRegistry::load(info, asset);
}

void AssetManager::on_filesystem_changed(std::vector<Terran::Core::FileSystemChangeEvent> const& fileSystemEvents)
{
    if (m_asset_change_callback)
        m_asset_change_callback(fileSystemEvents);

    for (auto const& e : fileSystemEvents) {
        if (std::filesystem::is_directory(e.FileName))
            continue;

        switch (e.Action) {
        case Terran::Core::FileAction::Removed: {
            on_asset_removed(AssetMetadataRegistry::asset_handle_from_path(e.FileName));
            break;
        }
        case Terran::Core::FileAction::Renamed: {
            bool was_asset = AssetImporterRegistry::exists_for_path(e.OldFileName);
            bool is_asset = AssetImporterRegistry::exists_for_path(e.FileName);

            if (!was_asset && is_asset) {
                import_asset(e.FileName);
            } else if (was_asset && !is_asset) {
                on_asset_removed(
                    AssetMetadataRegistry::asset_handle_from_path(e.OldFileName));
            } else {
                on_asset_renamed(
                    AssetMetadataRegistry::asset_handle_from_path(e.OldFileName), e.FileName);
            }
            break;
        }
        case Terran::Core::FileAction::Modified: {
            AssetMetadata info = AssetMetadataRegistry::asset_metadata_by_path(e.FileName);

            if (info)
                reload_asset_by_handle(info.Handle);

            break;
        }
        }
    }
}

void AssetManager::on_asset_removed(AssetHandle const& handle)
{
    if (AssetMetadataRegistry::contains(handle))
        AssetMetadataRegistry::erase(handle);

    if (m_loaded_assets.contains(handle))
        m_loaded_assets.erase(handle);

    AssetRemovedEvent removed_event(handle);
    m_event_dispatcher.trigger(removed_event);
}

void AssetManager::on_asset_renamed(AssetHandle const& handle, std::filesystem::path const& newFileName)
{
    AssetMetadata& info = AssetMetadataRegistry::asset_metadata_by_handle__internal(handle);

    AssetRenamedEvent renamed_event(handle, newFileName, info.Path);

    if (info)
        info.Path = newFileName;

    m_event_dispatcher.trigger(renamed_event);
}

}
