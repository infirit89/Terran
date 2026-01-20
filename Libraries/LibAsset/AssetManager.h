#pragma once

#include "Asset.h"
#include "AssetImporterRegistry.h"
#include "AssetMetadata.h"
#include "AssetTypes.h"

#include <LibAsset/AssetMetadataRegistry.h>
#include <LibCore/Base.h>
#include <LibCore/Event.h>
#include <LibCore/FileUtils.h>
#include <LibCore/Log.h>

#include <filesystem>
#include <functional>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace Terran::Asset {

class AssetManager final {
    using AssetChangeCallbackFn = std::function<void(std::vector<Terran::Core::FileSystemChangeEvent> const&)>;

public:
    AssetManager(Core::EventDispatcher& event_dispatcher);
    ~AssetManager();

     std::filesystem::path filesystem_path(std::filesystem::path const& path);

     AssetHandle import_asset(std::filesystem::path const& assetPath);
     void reload_asset_by_handle(AssetHandle const& handle);

     void SetAssetChangedCallback(AssetChangeCallbackFn const& callback) { m_asset_change_callback = callback; }

    template<typename TAsset>
    requires(std::is_base_of_v<Asset, TAsset>)
    Terran::Core::Shared<TAsset> asset_by_handle(AssetHandle const& assetHandle)
    {
        if (m_loaded_assets.contains(assetHandle))
            return Terran::Core::DynamicCast<TAsset>(m_loaded_assets.at(assetHandle));

        AssetMetadata& info = AssetMetadataRegistry::asset_metadata_by_handle__internal(assetHandle);

        if (!info)
            return nullptr;

        // NOTE: poc code
        Terran::Core::Shared<Asset> asset = nullptr;
        AssetImporterRegistry::load(info, asset);

        if (!asset) {
            TR_CORE_ERROR(TR_LOG_ASSET, "Failed to load asset with path: {0}", info.Path);
            return nullptr;
        }

        asset->m_handle = assetHandle;
        m_loaded_assets[assetHandle] = asset;
        return Terran::Core::DynamicCast<TAsset>(m_loaded_assets[assetHandle]);
    }

    template<typename TAsset>
    requires(std::is_base_of_v<Asset, TAsset>)
    Terran::Core::Shared<TAsset> asset_by_metadata(AssetMetadata const& assetMetadata)
    {
        if (m_loaded_assets.contains(assetMetadata.Handle))
            return Terran::Core::DynamicCast<TAsset>(m_loaded_assets.at(assetMetadata.Handle));

        // NOTE: poc code
        Terran::Core::Shared<Asset> asset = nullptr;
        AssetImporterRegistry::load(assetMetadata, asset);

        if (!asset) {
            TR_CORE_ERROR(TR_LOG_ASSET, "Failed to load asset with path: {0}", assetMetadata.Path);
            return nullptr;
        }

        asset->m_handle = assetMetadata.Handle;
        m_loaded_assets[assetMetadata.Handle] = asset;
        return Terran::Core::DynamicCast<TAsset>(m_loaded_assets[assetMetadata.Handle]);
    }

    template<typename TAsset>
    requires(
        std::is_base_of_v<Asset, TAsset>,
        HasStaticType<TAsset>)
    Terran::Core::Shared<TAsset> create_asset(std::filesystem::path const& filePath)
    {
        AssetMetadata metadata;
        metadata.Handle = AssetHandle();
        metadata.Path = filePath;
        metadata.Type = TAsset::static_type();

        int currentFileNumber = 2;
        while (file_exists(metadata.Path)) {
            metadata.Path = filePath.parent_path() / filePath.stem();

            metadata.Path = metadata.Path.string() + " (" + std::to_string(currentFileNumber) + ")" + filePath.extension().string();

            currentFileNumber++;
        }

        AssetMetadataRegistry::add_asset_metadata(metadata);

        // TODO: add parameter options
        Terran::Core::Shared<TAsset> asset = Terran::Core::CreateShared<TAsset>();

        m_loaded_assets[metadata.Handle] = asset;
        AssetImporterRegistry::save(metadata, asset);

        return Terran::Core::DynamicCast<TAsset>(m_loaded_assets[metadata.Handle]);
    }

    template<typename TAsset>
    requires(std::is_base_of_v<Asset, TAsset>)
    Terran::Core::Shared<TAsset> create_memory_asset()
    {
        Terran::Core::Shared<Asset> asset = Terran::Core::CreateShared<TAsset>();
        m_loaded_assets[asset->m_handle] = asset;

        return Terran::Core::DynamicCast<TAsset>(m_loaded_assets[asset->m_handle]);
    }

    static bool file_exists(std::filesystem::path const& path);

private:
    void on_filesystem_changed(std::vector<Terran::Core::FileSystemChangeEvent> const& fileSystemEvents);
    void on_asset_removed(AssetHandle const& handle);
    void on_asset_renamed(AssetHandle const& handle, std::filesystem::path const& newFileName);
    std::filesystem::path relative_path(std::filesystem::path const& path);

private:
    std::unordered_map<AssetHandle, Terran::Core::Shared<Asset>> m_loaded_assets;
    AssetChangeCallbackFn m_asset_change_callback;
    Core::EventDispatcher& m_event_dispatcher;
};

}
