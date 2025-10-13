#pragma once

#include "LibCore/Base.h"
#include "LibCore/UUID.h"

#include "Graphics/Texture.h"

#include "Asset/AssetManager.h"

#include <filesystem>
#include <string>

namespace TerranEditor {

enum class ItemAction {
    None = 0,
    NavigateTo,
    Activate,
    Select,
    StartRename,
    Renamed,
    MoveTo
};

enum class ItemType {
    Directory = 0,
    File
};

class ContentBrowserItem {
public:
    ContentBrowserItem(std::string const& name, Terran::Core::UUID const& handle, Terran::Core::Shared<TerranEngine::Texture> icon, ItemType type);
    virtual ~ContentBrowserItem() = default;

    void BeginRender();
    void EndRender();
    ItemAction OnRender();
    virtual void Move(std::filesystem::path const& newPath) = 0;

    Terran::Core::UUID GetHandle() { return m_Handle; }
    std::string const& GetName() { return m_Name; }
    ItemType GetType() { return m_Type; }

    void StartRename();
    void StopRename();

private:
    virtual ItemAction OnActivate() = 0;
    virtual ItemAction OnRename(std::string const& newName) = 0;

private:
    std::string m_Name;
    Terran::Core::Shared<TerranEngine::Texture> m_Icon;
    Terran::Core::UUID m_Handle;
    ItemType m_Type;
    bool m_IsRenaming = false;

    friend class ContentPanel;
};

struct DirectoryInfo;
class ContentBrowserDirectory : public ContentBrowserItem {
public:
    ContentBrowserDirectory(Terran::Core::Shared<DirectoryInfo> const& directoryInfo);
    virtual ~ContentBrowserDirectory() override = default;

    virtual void Move(std::filesystem::path const& newPath) override;
    Terran::Core::Shared<DirectoryInfo> const& GetDirectoryInfo() { return m_DirectoryInfo; }

private:
    virtual ItemAction OnActivate() override;
    virtual ItemAction OnRename(std::string const& newName) override;

private:
    Terran::Core::Shared<DirectoryInfo> m_DirectoryInfo;
};

class ContentBrowserAsset : public ContentBrowserItem {
public:
    ContentBrowserAsset(TerranEngine::AssetInfo const& assetInfo, Terran::Core::Shared<TerranEngine::Texture> const& icon);
    virtual ~ContentBrowserAsset() override = default;

    virtual void Move(std::filesystem::path const& newPath) override;
    TerranEngine::AssetInfo const& GetAssetInfo() { return m_AssetInfo; }

private:
    virtual ItemAction OnActivate() override;
    virtual ItemAction OnRename(std::string const& newName) override;

private:
    TerranEngine::AssetInfo m_AssetInfo;
};

}
