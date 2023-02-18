#pragma once

#include "Core/Base.h"
#include "Core/FileUtils.h"
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
		std::vector<Shared<DirectoryInfo>> Subdirectories;
		Shared<DirectoryInfo> Parent;
		std::vector<UUID> AssetHandles;
	};

	enum class ItemAction
	{
		None =  0,
		NavigateTo,
		Activate,
		Select,
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
		bool m_IsClicked = false;
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

		virtual void ImGuiRender() override;

		virtual void OnProjectChanged(const std::filesystem::path& projectPath) override;
	private:
		template<typename T>
		Shared<T> CreateAssetInDirectory(const std::string& name, const std::filesystem::path& directory = "")
		{
			std::filesystem::path filepath = directory / name;
			return AssetManager::CreateAsset<T>(filepath);
		}

		void Reload();
		UUID ProcessDirectory(const std::filesystem::path& directoryPath, const Shared<DirectoryInfo>& parent = nullptr);
		const Shared<DirectoryInfo>& GetDirectory(const std::filesystem::path& directoryPath);
		const Shared<DirectoryInfo>& GetDirectory(const UUID& id);
		void ChangeDirectory(const Shared<DirectoryInfo>& directory);
		void OnFileSystemChanged(const std::vector<TerranEngine::FileSystemChangeEvent>& events);

		Shared<DirectoryInfo> m_CurrentDirectory;
		Shared<DirectoryInfo> m_PreviousDirectory;
		std::stack<Shared<DirectoryInfo>> m_NextDirectoryStack;
		std::unordered_map<UUID, Shared<DirectoryInfo>> m_DirectoryInfoMap;
		std::vector <TerranEngine::Shared<ContentBrowserItem>> m_CurrentItems;
		//std::queue<std::function<void()>> m_PostRenderActions;
		
	};
}
