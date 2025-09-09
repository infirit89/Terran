#pragma once

#include "Core/Base.h"
#include "Core/FileUtils.h"
#include "Events/KeyboardEvent.h"

#include "Graphics/Texture.h"

#include "EditorPanel.h"

#include "ContentBrowserItem.h"

#include <filesystem>
#include <queue>
#include <stack>

namespace TerranEditor 
{
	using namespace TerranEngine;

	struct DirectoryInfo 
	{
		UUID Handle;
		std::filesystem::path Path;
		std::unordered_map<UUID, Shared<DirectoryInfo>> Subdirectories;
		Shared<DirectoryInfo> Parent;

		// TODO: change assets to be a hash set
		std::vector<UUID> Assets;
	};

	class ContentBrowserItemList 
	{
	using Iterator = std::vector<TerranEngine::Shared<ContentBrowserItem>>::iterator;
	public:
		Iterator Find(const UUID& handle) 
		{
			return std::find_if(Items.begin(), Items.end(), [&handle](Shared<ContentBrowserItem> item)
			{
				return item->GetHandle() == handle;
			});
		}

		Iterator Find(TerranEngine::Shared<ContentBrowserItem> item) 
		{ 
			return std::find(Items.begin(), Items.end(), item);
		}

		void Erase(TerranEngine::Shared<ContentBrowserItem> item)
		{
			auto itemIt = Find(item);
			if (itemIt != Items.end())
				Items.erase(itemIt);
		}

		void Erase(const UUID& id) 
		{
			auto itemIt = Find(id);
			if (itemIt != Items.end())
				Items.erase(itemIt);
		}

		void AddDirectory(Shared<DirectoryInfo> directory) 
		{
			if(Find(directory->Handle) == Items.end())
				Items.push_back(CreateShared<ContentBrowserDirectory>(directory));
		}

		void AddAsset(const AssetInfo& assetInfo, Shared<Texture> texture) 
		{
			if (Find(assetInfo.Handle) == Items.end())
				Items.push_back(CreateShared<ContentBrowserAsset>(assetInfo, texture));
		}

		Iterator begin() { return Items.begin(); }
		Iterator end() { return Items.end(); }

		std::vector<TerranEngine::Shared<ContentBrowserItem>> Items;
	};

	class ContentPanel : public EditorPanel
	{
	public:
		using OnItemClickedFn = std::function<void(const Shared<ContentBrowserItem>&)>;
		ContentPanel();
		~ContentPanel() = default;

		virtual void OnRender() override;
		virtual void OnEvent(TerranEngine::Event& event) override;

		virtual void OnProjectChanged(const std::filesystem::path& projectPath) override;
		virtual std::string_view GetName() override { return "Content"; }

		void SetOnItemClickCallback(const OnItemClickedFn& itemClickFn) { m_OnItemClickedFn = itemClickFn; }

	private:
		bool OnKeyPressedEvent(KeyPressedEvent& kEvent);
		bool DirectoryExists(const Shared<DirectoryInfo>& directory);
		
		void RenderTopBar();
		void RenderSideBar();
		void RenderDirectoryTree(Shared<DirectoryInfo> parent);

		template<typename T>
		Shared<T> CreateAsset(const std::string& name) 
		{
			return CreateAssetInDirectory<T>(name, m_CurrentDirectory->Path);
		}

		template<typename T>
		Shared<T> CreateAssetInDirectory(const std::string& name, const std::filesystem::path& directory)
		{
			std::filesystem::path filepath = directory / name;
			return AssetManager::CreateNewAsset<T>(filepath);
		}

		void CreateNewDirectory(const std::string& name, const std::filesystem::path parent)
		{
			std::filesystem::path directoryPath = parent / name;
			
			int currentFileNumber = 2;
			while (AssetManager::FileExists(directoryPath)) 
			{
				directoryPath = (parent / name).string() +
										" (" + std::to_string(currentFileNumber) + ")";
				currentFileNumber++;
			}

			std::filesystem::create_directory(AssetManager::GetFileSystemPath(directoryPath));
		}

		void Refresh();
		UUID ProcessDirectory(const std::filesystem::path& directoryPath, Shared<DirectoryInfo> parent = nullptr);
		Shared<DirectoryInfo> GetDirectory(const std::filesystem::path& directoryPath);
		Shared<DirectoryInfo> GetDirectory(const UUID& handle);
		void MoveSelectedItemsToDirectory(const Shared<DirectoryInfo>& directory);
		void ChangeDirectory(const Shared<DirectoryInfo>& directory);
		void OnFileSystemChanged(const std::vector<TerranEngine::FileSystemChangeEvent>& events);
		void FillBreadCrumbs();

		// returns the id of the directory it removed
		UUID RemoveDirectoryInfo(Shared<DirectoryInfo> directory);
		void SortItems();

		void ChangeForwardDirectory();
		void ChangeBackwardDirectory();
		
	private:
		Shared<DirectoryInfo> m_CurrentDirectory;
		Shared<DirectoryInfo> m_PreviousDirectory;
		Shared<DirectoryInfo> m_RootDirectory;
		std::stack<Shared<DirectoryInfo>> m_NextDirectoryStack;
		std::vector<Shared<DirectoryInfo>> m_BreadCrumbs;
		std::unordered_map<UUID, Shared<DirectoryInfo>> m_Directories;
		ContentBrowserItemList m_CurrentItems;
		std::function<void(const std::string&)> m_CreateAsset;
		std::filesystem::path m_NewAssetName;
		OnItemClickedFn m_OnItemClickedFn;

		template<typename AssetType>
		friend void DrawNewAssetMenu(ContentPanel* panel, const char* name, const char* fileName, bool& openRenamePopup);
	};
}
