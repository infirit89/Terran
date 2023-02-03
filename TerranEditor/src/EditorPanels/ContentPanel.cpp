#include "ContentPanel.h"

#include "EditorResources.h"

#include "Core/Log.h"
#include "Core/FileUtils.h"

#include "Assets/AssetManager.h"

#include "Project/Project.h"

#include "UI/UI.h"

#include <imgui.h>
#include <imgui_internal.h>

#pragma warning(push)
#pragma warning(disable : 4312)

namespace TerranEditor 
{
	ContentPanel::ContentPanel() 
	{
		AssetManager::SetAssetChangedCallback(TR_EVENT_BIND_FN(ContentPanel::OnFileSystemChanged));
	}


	static std::mutex s_ContentBrowserMutex;
	void ContentPanel::ImGuiRender()
	{
		if (m_Open) 
		{
			ImGui::Begin("Content", &m_Open);
			
			
			auto buttonFunc = [this](const std::string& buttonText, bool condition)
			{
				bool result = false;
				if (!condition)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					float alpha = ImGui::GetStyle().Alpha * 0.5f;
					UI::ScopedStyleVar scoped({ { ImGuiStyleVar_Alpha, {alpha, alpha} } });
					ImGui::Button(buttonText.c_str());
					ImGui::PopItemFlag();
				}
				else 
					result = ImGui::Button(buttonText.c_str());

				return result;
			};

			// back button
			if (buttonFunc("<-", m_PreviousDirectory != nullptr)) 
			{
				m_NextDirectoryStack.push(m_CurrentDirectory);
				ChangeDirectory(m_PreviousDirectory);
			}
			
			ImGui::SameLine();
			if (buttonFunc("->", m_NextDirectoryStack.size() > 0))
			{
				ChangeDirectory(m_NextDirectoryStack.top());
				m_NextDirectoryStack.pop();
			}

			ImGui::SameLine();
			if (ImGui::Button("Reload"))
				Reload();

			if (ImGui::BeginPopupContextWindow("CONTENT_PANEL_ACTIONS", ImGuiMouseButton_Right, false)) 
			{
				if (ImGui::MenuItem("Reveal in explorer"))
					FileSystem::RevealInExplorer(m_CurrentDirectory->Path);

				ImGui::EndPopup();
			}

			// render items
			{
				std::scoped_lock<std::mutex> lock(s_ContentBrowserMutex);
				const float padding = 18.0f;
				const float cellSize = 74.0f;

				float totalSize = padding + cellSize;
				float availRegionWidth = ImGui::GetContentRegionAvailWidth();
				int columnCount = (int)(availRegionWidth / totalSize) < 1 ? 1 : (int)(availRegionWidth / totalSize);

				ImGui::Columns(columnCount, (const char*)0, false);

				// for every entry in the current 
				for (const auto& item : m_CurrentItems)
				{
					item->BeginRender();
					ContentBrowserPanelAction action = item->OnRender();

					if (action == ContentBrowserPanelAction::NavigateTo)
						m_PostRenderActions.push([&item, this]() { ChangeDirectory(DynamicCast<ContentBrowserPanelDirectory>(item)->GetDirectoryInfo()); });

					ImGui::NextColumn();
					item->EndRender();
				}

				ImGui::Columns(1);
			}

			while (!m_PostRenderActions.empty())
			{
				auto postRenderAction = m_PostRenderActions.front();
				postRenderAction();
				m_PostRenderActions.pop();
			}

			ImGui::End();
		}
	}

	void ContentPanel::Reload()
	{
		m_DirectoryInfoMap.clear();
		UUID dirID = ProcessDirectory(Project::GetAssetPath());
		m_CurrentDirectory = GetDirectory(dirID);
		ChangeDirectory(m_CurrentDirectory);
	}

	UUID ContentPanel::ProcessDirectory(const std::filesystem::path& directoryPath, const Shared<DirectoryInfo>& parent)
	{
		const auto& directory = GetDirectory(directoryPath);
		if (directory) return directory->ID;

		Shared<DirectoryInfo> directoryInfo = CreateShared<DirectoryInfo>();
		directoryInfo->ID = UUID();
		directoryInfo->Parent = parent;

		if (directoryPath == Project::GetAssetPath())
			directoryInfo->Path = "";
		else
			directoryInfo->Path = std::filesystem::relative(directoryPath, Project::GetAssetPath());

		for (auto& directoryEntry : std::filesystem::directory_iterator(directoryPath))
		{
			if (directoryEntry.is_directory()) 
			{
				UUID subdirectoryID = ProcessDirectory(directoryEntry, directoryInfo);
				directoryInfo->Subdirectories.push_back(m_DirectoryInfoMap[subdirectoryID]);
			}
			else 
			{
				UUID assetID = AssetManager::ImportAsset(directoryEntry);

				if(assetID)
					directoryInfo->AssetHandles.push_back(assetID);
			}
		}

		m_DirectoryInfoMap[directoryInfo->ID] = directoryInfo;
		return directoryInfo->ID;
	}

	const Shared<DirectoryInfo>& ContentPanel::GetDirectory(const std::filesystem::path& directoryPath)
	{
		for (const auto&[id, directoryInfo] : m_DirectoryInfoMap)
		{
			if (directoryInfo->Path == directoryPath)
				return directoryInfo;
		}

		return nullptr;
	}

	const Shared<DirectoryInfo>& ContentPanel::GetDirectory(const UUID& id) 
	{
		if (m_DirectoryInfoMap.find(id) != m_DirectoryInfoMap.end())
			return m_DirectoryInfoMap.at(id);

		return nullptr;
	}

	void ContentPanel::ChangeDirectory(const Shared<DirectoryInfo>& directory)
	{
		if (!directory) return;

		m_CurrentItems.clear();

		for (const auto& subdirectory : directory->Subdirectories)
			m_CurrentItems.push_back(CreateShared<ContentBrowserPanelDirectory>(subdirectory));

		for (const auto& assetHandle : directory->AssetHandles)
		{
			AssetInfo info = AssetManager::GetAssetInfo(assetHandle);
			m_CurrentItems.push_back(CreateShared<ContentBrowserPanelAsset>(info, EditorResources::GetFileTexture()));
		}

		m_CurrentDirectory = directory;
		m_PreviousDirectory = m_CurrentDirectory->Parent;
	}

	void ContentPanel::OnFileSystemChanged(const std::vector<TerranEngine::FileSystemChangeEvent>& events)
	{
		Reload();
	}

	static void PrintDirectoryInfo(const Shared<DirectoryInfo>& parent, int level = 0) 
	{
		std::string childStr = std::string(level, '-');

		if (level == 0)
			TR_TRACE("Assets");
		else
			TR_TRACE("{0}{1}", childStr, parent->Path.string());

		for (const auto& subdirectory : parent->Subdirectories)
			PrintDirectoryInfo(subdirectory, level + 1);
	}

	void ContentPanel::OnProjectChanged(const std::filesystem::path& projectPath)
	{
		UUID dirID = ProcessDirectory(Project::GetAssetPath());

		m_CurrentDirectory = GetDirectory(dirID);
		ChangeDirectory(m_CurrentDirectory);
	}

	ContentBrowserPanelItem::ContentBrowserPanelItem(const std::string& name, const UUID& id, Shared<Texture> icon)
		: m_Name(name), m_Icon(icon), m_ID(id)
	{ }

	void ContentBrowserPanelItem::BeginRender()
	{
		ImGui::PushID(&m_ID);
	}

	void ContentBrowserPanelItem::EndRender()
	{
		ImGui::PopID();
	}

	ContentBrowserPanelAction ContentBrowserPanelItem::OnRender()
	{
		ContentBrowserPanelAction action = ContentBrowserPanelAction::None;
		const float cellSize = 74.0f;
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });
		ImGui::ImageButton((ImTextureID)m_Icon->GetTextureID(), { cellSize, cellSize }, { 0, 1 }, { 1, 0 });
		ImGui::PopStyleColor();

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("ASSET", m_ID.GetRaw(), sizeof(UUID));
			ImGui::Text("File %s", m_Name.c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			action = OnClick();

		float textWidth = ImGui::CalcTextSize(m_Name.c_str()).x;
		ImVec2 cursorPos = ImGui::GetCursorPos();

		// some indent calculations, just did something that didn't look bad
		float textIndent = ((cellSize + 10.0f) - textWidth) * 0.5f;
		textIndent = textIndent <= 0.0f ? 1.0f : textIndent;

		ImGui::SetCursorPosX(cursorPos.x + textIndent);
		ImGui::TextWrapped(m_Name.c_str());
		ImGui::SetCursorPosX(cursorPos.x);

		return action;
	}

	ContentBrowserPanelDirectory::ContentBrowserPanelDirectory(const Shared<DirectoryInfo>& directoryInfo)
		: ContentBrowserPanelItem(directoryInfo->Path.filename().string(), directoryInfo->ID, EditorResources::GetDirectoryTexture()), m_DirectoryInfo(directoryInfo)
	{ }

	ContentBrowserPanelAction ContentBrowserPanelDirectory::OnClick() { return ContentBrowserPanelAction::NavigateTo; }
	const Shared<DirectoryInfo>& ContentBrowserPanelDirectory::GetDirectoryInfo() { return m_DirectoryInfo; }


	
	ContentBrowserPanelAsset::ContentBrowserPanelAsset(const AssetInfo& assetInfo, const Shared<Texture>& icon)
		: ContentBrowserPanelItem(assetInfo.Path.filename().string(), assetInfo.Handle, icon), m_AssetInfo(assetInfo)
	{ }

	ContentBrowserPanelAction ContentBrowserPanelAsset::OnClick() { return ContentBrowserPanelAction::None; }
}
#pragma warning(pop)
