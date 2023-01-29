#pragma once

#include "Core/Base.h"
#include "Graphics/Texture.h"

#include "EditorPanel.h"

#include <filesystem>
#include <queue>

namespace TerranEditor 
{
	using namespace TerranEngine;

	struct DirectoryInfo 
	{
		UUID ID;
		std::filesystem::path Path;
		std::vector<Shared<DirectoryInfo>> Subdirectories;
		Shared<DirectoryInfo> Parent;
	};

	enum class ContentBrowserPanelAction
	{
		None,
		NavigateTo
	};

	class ContentBrowserPanelItem
	{
	public:
		ContentBrowserPanelItem(const std::filesystem::path& name, TerranEngine::Shared<TerranEngine::Texture> icon);
		virtual ~ContentBrowserPanelItem() = default;

		virtual ContentBrowserPanelAction OnRender() { return ContentBrowserPanelAction::None; };
	protected:
		std::filesystem::path m_Name;
		TerranEngine::Shared<TerranEngine::Texture> m_Icon;
	};

	class ContentBrowserPanelDirectory : public ContentBrowserPanelItem
	{
	public:
		ContentBrowserPanelDirectory(const Shared<DirectoryInfo>& directoryInfo);

		virtual ~ContentBrowserPanelDirectory() override = default;
		virtual ContentBrowserPanelAction OnRender() override;

		const Shared<DirectoryInfo>& GetDirectoryInfo();

	private:
		Shared<DirectoryInfo> m_DirectoryInfo;
	};

	class ContentPanel : public EditorPanel
	{
	public:
		ContentPanel();
		~ContentPanel() = default;

		virtual void ImGuiRender() override;

		virtual void OnProjectChanged(const std::filesystem::path& projectPath) override;
	private:
		UUID ProcessDirectory(const std::filesystem::path& directoryPath, const Shared<DirectoryInfo>& parent = nullptr);
		const Shared<DirectoryInfo>& GetDirectory(const std::filesystem::path& directoryPath);
		const Shared<DirectoryInfo>& GetDirectory(const UUID& id);
	void ChangeDirectory(const Shared<DirectoryInfo>& directory);

		std::filesystem::path m_CurrentPath;
		std::unordered_map<UUID, Shared<DirectoryInfo>> m_DirectoryInfoMap;
		std::vector <TerranEngine::Shared<ContentBrowserPanelItem>> m_CurrentItems;
		std::queue<std::function<void()>> m_PostRenderActions;
	};
}
