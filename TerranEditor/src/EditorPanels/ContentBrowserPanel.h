#pragma once

#include "Events/KeyboardEvent.h"
#include "LibCore/Base.h"
#include "LibCore/FileUtils.h"

#include "Graphics/Texture.h"

#include "EditorPanel.h"

#include "ContentBrowserItem.h"

#include <filesystem>
#include <queue>
#include <stack>

namespace TerranEditor {

using namespace TerranEngine;

struct DirectoryInfo {
    Terran::Core::UUID Handle;
    std::filesystem::path Path;
    std::unordered_map<Terran::Core::UUID, Terran::Core::Shared<DirectoryInfo>> Subdirectories;
    Terran::Core::Shared<DirectoryInfo> Parent;

    // TODO: change assets to be a hash set
    std::vector<Terran::Core::UUID> Assets;
};

class ContentBrowserItemList {
    using Iterator = std::vector<Terran::Core::Shared<ContentBrowserItem>>::iterator;

public:
    Iterator Find(Terran::Core::UUID const& handle)
    {
        return std::find_if(Items.begin(), Items.end(), [&handle](Terran::Core::Shared<ContentBrowserItem> item) {
            return item->GetHandle() == handle;
        });
    }

    Iterator Find(Terran::Core::Shared<ContentBrowserItem> item)
    {
        return std::find(Items.begin(), Items.end(), item);
    }

    void Erase(Terran::Core::Shared<ContentBrowserItem> item)
    {
        auto itemIt = Find(item);
        if (itemIt != Items.end())
            Items.erase(itemIt);
    }

    void Erase(Terran::Core::UUID const& id)
    {
        auto itemIt = Find(id);
        if (itemIt != Items.end())
            Items.erase(itemIt);
    }

    void AddDirectory(Terran::Core::Shared<DirectoryInfo> directory)
    {
        if (Find(directory->Handle) == Items.end())
            Items.push_back(Terran::Core::CreateShared<ContentBrowserDirectory>(directory));
    }

    void AddAsset(AssetInfo const& assetInfo, Terran::Core::Shared<Texture> texture)
    {
        if (Find(assetInfo.Handle) == Items.end())
            Items.push_back(Terran::Core::CreateShared<ContentBrowserAsset>(assetInfo, texture));
    }

    Iterator begin() { return Items.begin(); }
    Iterator end() { return Items.end(); }

    std::vector<Terran::Core::Shared<ContentBrowserItem>> Items;
};

class ContentPanel : public EditorPanel {
public:
    using OnItemClickedFn = std::function<void(Terran::Core::Shared<ContentBrowserItem> const&)>;
    ContentPanel();
    ~ContentPanel() = default;

    virtual void OnRender() override;
    virtual void OnEvent(Terran::Core::Event& event) override;

    virtual void OnProjectChanged(std::filesystem::path const& projectPath) override;
    virtual std::string_view GetName() override { return "Content"; }

    void SetOnItemClickCallback(OnItemClickedFn const& itemClickFn) { m_OnItemClickedFn = itemClickFn; }

private:
    bool OnKeyPressedEvent(KeyPressedEvent& kEvent);
    bool DirectoryExists(Terran::Core::Shared<DirectoryInfo> const& directory);

    void RenderTopBar();
    void RenderSideBar();
    void RenderDirectoryTree(Terran::Core::Shared<DirectoryInfo> parent);

    template<typename T>
    Terran::Core::Shared<T> CreateAsset(std::string const& name)
    {
        return CreateAssetInDirectory<T>(name, m_CurrentDirectory->Path);
    }

    template<typename T>
    Terran::Core::Shared<T> CreateAssetInDirectory(std::string const& name, std::filesystem::path const& directory)
    {
        std::filesystem::path filepath = directory / name;
        return AssetManager::CreateNewAsset<T>(filepath);
    }

    void CreateNewDirectory(std::string const& name, std::filesystem::path const parent)
    {
        std::filesystem::path directoryPath = parent / name;

        int currentFileNumber = 2;
        while (AssetManager::FileExists(directoryPath)) {
            directoryPath = (parent / name).string() + " (" + std::to_string(currentFileNumber) + ")";
            currentFileNumber++;
        }

        std::filesystem::create_directory(AssetManager::GetFileSystemPath(directoryPath));
    }

    void Refresh();
    Terran::Core::UUID ProcessDirectory(std::filesystem::path const& directoryPath, Terran::Core::Shared<DirectoryInfo> parent = nullptr);
    Terran::Core::Shared<DirectoryInfo> GetDirectory(std::filesystem::path const& directoryPath);
    Terran::Core::Shared<DirectoryInfo> GetDirectory(Terran::Core::UUID const& handle);
    void MoveSelectedItemsToDirectory(Terran::Core::Shared<DirectoryInfo> const& directory);
    void ChangeDirectory(Terran::Core::Shared<DirectoryInfo> const& directory);
    void OnFileSystemChanged(std::vector<Terran::Core::FileSystemChangeEvent> const& events);
    void FillBreadCrumbs();

    // returns the id of the directory it removed
    Terran::Core::UUID RemoveDirectoryInfo(Terran::Core::Shared<DirectoryInfo> directory);
    void SortItems();

    void ChangeForwardDirectory();
    void ChangeBackwardDirectory();

private:
    Terran::Core::Shared<DirectoryInfo> m_CurrentDirectory;
    Terran::Core::Shared<DirectoryInfo> m_PreviousDirectory;
    Terran::Core::Shared<DirectoryInfo> m_RootDirectory;
    std::stack<Terran::Core::Shared<DirectoryInfo>> m_NextDirectoryStack;
    std::vector<Terran::Core::Shared<DirectoryInfo>> m_BreadCrumbs;
    std::unordered_map<Terran::Core::UUID, Terran::Core::Shared<DirectoryInfo>> m_Directories;
    ContentBrowserItemList m_CurrentItems;
    std::function<void(std::string const&)> m_CreateAsset;
    std::filesystem::path m_NewAssetName;
    OnItemClickedFn m_OnItemClickedFn;

    template<typename AssetType>
    friend void DrawNewAssetMenu(ContentPanel* panel, char const* name, char const* fileName, bool& openRenamePopup);
};

}
