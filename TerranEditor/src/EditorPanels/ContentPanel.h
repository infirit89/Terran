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

	enum class ContentBrowserPanelAction
	{
		None,
		NavigateTo
	};

	class ContentBrowserPanelItem
	{
	public:
		ContentBrowserPanelItem(const std::string& name, const UUID& id, TerranEngine::Shared<TerranEngine::Texture> icon);
		virtual ~ContentBrowserPanelItem() = default;

		void BeginRender();
		void EndRender();
		ContentBrowserPanelAction OnRender();
		virtual ContentBrowserPanelAction OnClick() = 0;

	private:
		std::string m_Name;
		TerranEngine::Shared<TerranEngine::Texture> m_Icon;
		UUID m_ID;
	};

	class ContentBrowserPanelDirectory : public ContentBrowserPanelItem
	{
	public:
		ContentBrowserPanelDirectory(const Shared<DirectoryInfo>& directoryInfo);
		virtual ~ContentBrowserPanelDirectory() override = default;

		virtual ContentBrowserPanelAction OnClick() override;
		const Shared<DirectoryInfo>& GetDirectoryInfo();

	private:
		Shared<DirectoryInfo> m_DirectoryInfo;
	};

	class ContentBrowserPanelAsset : public ContentBrowserPanelItem 
	{
	public:
		ContentBrowserPanelAsset(const AssetInfo& assetInfo, const Shared<Texture>& icon);
		virtual ~ContentBrowserPanelAsset() override = default;

		virtual ContentBrowserPanelAction OnClick() override;
		const AssetInfo& GetAssetInfo() { return m_AssetInfo; }

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
		std::vector <TerranEngine::Shared<ContentBrowserPanelItem>> m_CurrentItems;
		std::queue<std::function<void()>> m_PostRenderActions;
	};
}
