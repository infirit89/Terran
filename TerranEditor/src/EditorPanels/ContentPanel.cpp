#include "ContentPanel.h"

#include "EditorResources.h"

#include "Core/Log.h"
#include "Core/FileUtils.h"

#include "Assets/AssetManager.h"

#include "Project/Project.h"

#include "SelectionManager.h"

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
		if (!m_Open) return;

		ImGui::Begin("Content", &m_Open);

		ImGui::BeginChild("##topbar", { 0.0f, 40.0f });
		ImGui::BeginHorizontal("##topbar", ImGui::GetWindowSize());
		{
			auto buttonFunc = [this](const std::string& buttonText, bool condition)
			{
				bool result = false;
				if (!condition)
					ImGui::BeginDisabled();

				result = ImGui::Button(buttonText.c_str());

				if (!condition)
					ImGui::EndDisabled();

				return result;
			};

			// back button
			if (buttonFunc("<-", m_PreviousDirectory != nullptr)) 
			{
				m_NextDirectoryStack.push(m_CurrentDirectory);
				ChangeDirectory(m_PreviousDirectory);
			}

			ImGui::Spring(-1.0f, 4.0f);
			
			if (buttonFunc("->", m_NextDirectoryStack.size() > 0))
			{
				ChangeDirectory(m_NextDirectoryStack.top());
				m_NextDirectoryStack.pop();
			}
			//ImGui::Spring(-1.0f, 4.0f * 2.0f);
			if (ImGui::Button("Reload"))
				Reload();

			//ImGui::Spring(-1.0f, 4.0f * 2.0f);
		}
		ImGui::EndHorizontal();
		ImGui::EndChild();

		if (ImGui::BeginPopupContextWindow("CONTENT_PANEL_ACTIONS", ImGuiMouseButton_Right)) 
		{
			if (ImGui::MenuItem("Reveal in explorer"))
				FileSystem::RevealInExplorer(m_CurrentDirectory->Path);

			if (ImGui::MenuItem("Create physics material"))
				CreateAssetInDirectory<PhysicsMaterial2DAsset>("test.trpm2d");

			ImGui::EndPopup();
		}

		// render items
		{
			std::scoped_lock<std::mutex> lock(s_ContentBrowserMutex);
			const float padding = 18.0f;
			const float cellSize = 74.0f;

			float totalSize = padding + cellSize;
			float availRegionWidth = ImGui::GetContentRegionAvail().x;
			int columnCount = (int)(availRegionWidth / totalSize) < 1 ? 1 : (int)(availRegionWidth / totalSize);

			ImGui::Columns(columnCount, (const char*)0, false);

			// for every entry in the current 
			for (const auto& item : m_CurrentItems)
			{
				item->BeginRender();
				ContentBrowserPanelAction action = item->OnRender();

				if (action == ContentBrowserPanelAction::NavigateTo) 
				{
					m_PostRenderActions.push([&item, this]() { ChangeDirectory(DynamicCast<ContentBrowserPanelDirectory>(item)->GetDirectoryInfo()); });
					while (!m_NextDirectoryStack.empty()) m_NextDirectoryStack.pop();
				}

				if (action == ContentBrowserPanelAction::Activate)
					SelectionManager::Select(SelectionContext::ContentPanel, item->GetID());

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

	ContentBrowserPanelItem::ContentBrowserPanelItem(const std::string& name, const UUID& id, Shared<Texture> icon, ContentBrowserPanelItemType type)
		: m_Name(name), m_Icon(icon), m_ID(id), m_Type(type)
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

		ImGui::BeginGroup();
		const float cellSize = 74.0f;
		const float edgeOffset = 4.0f;

		const float textLineHeight = ImGui::GetTextLineHeightWithSpacing() * 2.0f + edgeOffset * 2.0f;
		const float infoPanelHeight = std::min(false ? cellSize * 0.5f : textLineHeight, textLineHeight);

		const ImVec2 topLeft = ImGui::GetCursorScreenPos();
		const ImVec2 thumbBottomRight = { topLeft.x + cellSize, topLeft.y + cellSize };
		const ImVec2 infoTopLeft = { topLeft.x, topLeft.y + cellSize };
		const ImVec2 bottomRight = { topLeft.x + cellSize, topLeft.y + cellSize + infoPanelHeight };

		if (ImGui::ItemHoverable(ImRect(topLeft, bottomRight), ImGui::GetID(&m_ID)))
		{
			auto* drawList = ImGui::GetWindowDrawList();
			ImGuiStyle& style = ImGui::GetStyle();

			drawList->AddRectFilled(topLeft, bottomRight,
				ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Header]), 6.0f);
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 4.0f });

		ImGui::InvisibleButton("##thumbnailButton", { cellSize, cellSize }, ImGuiButtonFlags_PressedOnClickRelease);
		UI::ShiftCursor(edgeOffset, -cellSize);
		UI::Image((ImTextureID)m_Icon->GetTextureID(), { cellSize - edgeOffset * 2.0, cellSize - edgeOffset * 2.0 });
		
		UI::ShiftCursor(edgeOffset, edgeOffset);

		ImGui::BeginVertical((std::string("InfoPanel") + m_Name).c_str(), ImVec2(cellSize - edgeOffset * 2.0f, infoPanelHeight - edgeOffset));
		ImGui::BeginHorizontal(m_Name.c_str(), { cellSize - 2.0f, 0.0f });
		ImGui::Spring();
		ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + (cellSize - edgeOffset * 3.0f));
		const float textWidth = std::min(ImGui::CalcTextSize(m_Name.c_str()).x, cellSize);
		ImGui::SetNextItemWidth(textWidth);
		ImGui::Text(m_Name.c_str());
		ImGui::PopTextWrapPos();
		ImGui::Spring();
		ImGui::EndHorizontal();
		ImGui::Spring();
		ImGui::EndVertical();

		ImGui::PopStyleVar();

		UI::ShiftCursor(-edgeOffset, -edgeOffset);

		ImGui::EndGroup();

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::SetDragDropPayload("ASSET", m_ID.GetRaw(), sizeof(UUID));
			ImGui::Text("File %s", m_Name.c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			action = OnClick();

		//// some indent calculations, just did something that didn't look bad
		//float textIndent = ((cellSize + 10.0f) - textWidth) * 0.5f;
		//textIndent = textIndent <= 0.0f ? 1.0f : textIndent;

		//ImGui::TextWrapped(m_Name.c_str());
		//ImGui::SetCursorPosX(cursorPos.x);


		return action;
	}

	ContentBrowserPanelDirectory::ContentBrowserPanelDirectory(const Shared<DirectoryInfo>& directoryInfo)
		: ContentBrowserPanelItem(directoryInfo->Path.filename().string(), directoryInfo->ID, EditorResources::GetDirectoryTexture(), ContentBrowserPanelItemType::Directory), m_DirectoryInfo(directoryInfo)
	{ }

	ContentBrowserPanelAction ContentBrowserPanelDirectory::OnClick() { return ContentBrowserPanelAction::NavigateTo; }
	const Shared<DirectoryInfo>& ContentBrowserPanelDirectory::GetDirectoryInfo() { return m_DirectoryInfo; }


	
	ContentBrowserPanelAsset::ContentBrowserPanelAsset(const AssetInfo& assetInfo, const Shared<Texture>& icon)
		: ContentBrowserPanelItem(assetInfo.Path.stem().string(), assetInfo.Handle, icon, ContentBrowserPanelItemType::File), m_AssetInfo(assetInfo)
	{ }

	ContentBrowserPanelAction ContentBrowserPanelAsset::OnClick() { return ContentBrowserPanelAction::Activate; }
}
#pragma warning(pop)
