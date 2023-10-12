#pragma once

#include "Core/Base.h"
#include "Core/FileUtils.h"
#include "Events/KeyboardEvent.h"

#include "Graphics/Texture.h"

#include "EditorPanel.h"

#include <filesystem>
#include <queue>
#include <stack>

namespace TerranEditor 
{
	using namespace TerranEngine;

	struct DirectoryInfo 
	{
		UUID ID;
		std::filesystem::path Path;
		std::unordered_map<UUID, Shared<DirectoryInfo>> Subdirectories;
		Shared<DirectoryInfo> Parent;
		std::vector<UUID> Assets;
		std::filesystem::file_time_type ModifiedTime;
	};

	enum class ItemAction
	{
		None =  0,
		NavigateTo,
		Activate,
		Select,
		StartRename,
		Renamed,
		MoveTo
	};

	enum class ItemType
	{
		Directory = 0,
		File
	};

	class ContentBrowserItem
	{
	public:
		ContentBrowserItem(const std::string& name, const UUID& id, TerranEngine::Shared<TerranEngine::Texture> icon, ItemType type);
		virtual ~ContentBrowserItem() = default;

		void BeginRender();
		void EndRender();
		ItemAction OnRender();
		virtual void Move(const std::filesystem::path& newPath) = 0;

		UUID GetID() { return m_ID; }
		const std::string& GetName() { return m_Name; }
		ItemType GetType() { return m_Type; }

		void StartRename();
		void StopRename();

	private:
		virtual ItemAction OnActivate() = 0;
		virtual ItemAction OnRename(const std::string& newName) = 0;

	private:
		std::string m_Name;
		TerranEngine::Shared<TerranEngine::Texture> m_Icon;
		UUID m_ID;
		ItemType m_Type;
		bool m_IsRenaming = false;
	};

	class ContentBrowserItemList 
	{
	using Iterator = std::vector<TerranEngine::Shared<ContentBrowserItem>>::iterator;
	public:
		Iterator Find(const UUID& id) 
		{
			return std::find_if(Items.begin(), Items.end(), [&id](Shared<ContentBrowserItem> item)
			{
				return item->GetID() == id;
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

		Iterator begin() { return Items.begin(); }
		Iterator end() { return Items.end(); }

		std::vector<TerranEngine::Shared<ContentBrowserItem>> Items;
	};

	class ContentBrowserDirectory : public ContentBrowserItem
	{
	public:
		ContentBrowserDirectory(const Shared<DirectoryInfo>& directoryInfo);
		virtual ~ContentBrowserDirectory() override = default;

		virtual void Move(const std::filesystem::path& newPath) override;
		const Shared<DirectoryInfo>& GetDirectoryInfo() { return m_DirectoryInfo; }

	private:
		virtual ItemAction OnActivate() override;
		virtual ItemAction OnRename(const std::string& newName) override;

	private:
		Shared<DirectoryInfo> m_DirectoryInfo;
	};

	class ContentBrowserAsset : public ContentBrowserItem 
	{
	public:
		ContentBrowserAsset(const AssetInfo& assetInfo, const Shared<Texture>& icon);
		virtual ~ContentBrowserAsset() override = default;

		virtual void Move(const std::filesystem::path& newPath) override;
		const AssetInfo& GetAssetInfo() { return m_AssetInfo; }

	private:
		virtual ItemAction OnActivate() override;
		virtual ItemAction OnRename(const std::string& newName) override;

	private:
		AssetInfo m_AssetInfo;
	};

	class ContentPanel : public EditorPanel
	{
	public:
		ContentPanel();
		~ContentPanel() = default;

		virtual void OnRender() override;
		virtual void OnEvent(TerranEngine::Event& event) override;

		virtual void OnProjectChanged(const std::filesystem::path& projectPath) override;

	private:
		bool OnKeyPressedEvent(KeyPressedEvent& kEvent);
		bool DirectoryExists(const Shared<DirectoryInfo>& directory);
		
		template<typename T>
		Shared<T> CreateAsset(const std::string& name) 
		{
			return CreateAssetInDirectory(name, m_CurrentDirectory->Path);
		}

		template<typename T>
		Shared<T> CreateAssetInDirectory(const std::string& name, const std::filesystem::path& directory)
		{
			std::filesystem::path filepath = directory / name;
			return AssetManager::CreateNewAsset<T>(filepath);
		}

		void Refresh();
		UUID ProcessDirectory(const std::filesystem::path& directoryPath, const Shared<DirectoryInfo>& parent = nullptr);
		Shared<DirectoryInfo> GetDirectory(const std::filesystem::path& directoryPath);
		Shared<DirectoryInfo> GetDirectory(const UUID& id);
		void ChangeDirectory(const Shared<DirectoryInfo>& directory);
		void OnFileSystemChanged(const std::vector<TerranEngine::FileSystemChangeEvent>& events);

		void RefreshDirectory(const Shared<DirectoryInfo>& directory);
		void RemoveDirectoryInfo(const Shared<DirectoryInfo>& directory);
		void UpdateCurrentItems();
		void SortItems();

		void ChangeForwardDirectory();
		void ChangeBackwardDirectory();

	private:
		Shared<DirectoryInfo> m_CurrentDirectory;
		Shared<DirectoryInfo> m_PreviousDirectory;
		std::stack<Shared<DirectoryInfo>> m_NextDirectoryStack;
		std::unordered_map<UUID, Shared<DirectoryInfo>> m_Directories;
		ContentBrowserItemList m_CurrentItems;
		std::function<void(const std::string&)> m_CreateAsset;
		std::filesystem::path m_NewAssetName;

		template<typename AssetType>
		friend void DrawNewAssetMenu(ContentPanel* panel, const char* name, const char* fileName, bool& openRenamePopup);
	};
}
