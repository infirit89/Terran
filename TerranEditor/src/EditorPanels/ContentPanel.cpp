#include "ContentPanel.h"

#include "EditorResources.h"

#include "Core/Log.h"
#include "Core/FileUtils.h"
#include "Core/Input.h"
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
			ImGui::Spring(-1.0f, 4.0f * 2.0f);
			if (ImGui::Button("Reload"))
				Reload();
		}
		ImGui::EndHorizontal();
		ImGui::EndChild();

		if (ImGui::BeginPopupContextWindow("content_panel_actions_popup", ImGuiMouseButton_Right)) 
		{
			if (ImGui::MenuItem("Reveal in explorer"))
				FileSystem::RevealInExplorer(m_CurrentDirectory->Path);

			if (ImGui::MenuItem("Create physics material"))
				CreateAssetInDirectory<PhysicsMaterial2DAsset>("test.trpm2d");

			ImGui::EndPopup();
		}

		Shared<DirectoryInfo> nextDir;
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
				ItemAction action = item->OnRender();

				if (action == ItemAction::NavigateTo) 
				{
					nextDir = DynamicCast<ContentBrowserDirectory>(item)->GetDirectoryInfo();
					//m_PostRenderActions.push([&item, this]() 
					//{ 
						//ChangeDirectory(DynamicCast<ContentBrowserPanelDirectory>(item)->GetDirectoryInfo());
					//});
					while (!m_NextDirectoryStack.empty()) m_NextDirectoryStack.pop();
				}

				if (action == ItemAction::Activate)
				{
				}

				if (action == ItemAction::Select)
					SelectionManager::Select(SelectionContext::ContentPanel, item->GetID());

				if (action == ItemAction::MoveTo)
				{
					auto selectedItem = std::find_if(m_CurrentItems.begin(), m_CurrentItems.end(), [](Shared<ContentBrowserItem> browserItem)
					{
						return browserItem->GetID() == SelectionManager::GetSelected(SelectionContext::ContentPanel);
					});

					if (selectedItem != m_CurrentItems.end()) 
					{
						std::filesystem::path directoryPath = m_DirectoryInfoMap[item->GetID()]->Path;
						(*selectedItem)->Move(directoryPath);
					}
				}

				ImGui::NextColumn();
				item->EndRender();
			}

			ImGui::Columns(1);
		}

		if (nextDir)
			ChangeDirectory(nextDir);

		/*while (!m_PostRenderActions.empty())
		{
			auto postRenderAction = m_PostRenderActions.front();
			postRenderAction();
			m_PostRenderActions.pop();
		}*/

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
			m_CurrentItems.push_back(CreateShared<ContentBrowserDirectory>(subdirectory));

		for (const auto& assetHandle : directory->AssetHandles)
		{
			AssetInfo info = AssetManager::GetAssetInfo(assetHandle);
			m_CurrentItems.push_back(CreateShared<ContentBrowserAsset>(info, EditorResources::GetFileTexture()));
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

#define MAX_RENAME_BUFFER_SIZE 256

	static char s_RenameBuffer[MAX_RENAME_BUFFER_SIZE]{ 0 };

	void ContentPanel::OnProjectChanged(const std::filesystem::path& projectPath)
	{
		UUID dirID = ProcessDirectory(Project::GetAssetPath());

		m_CurrentDirectory = GetDirectory(dirID);
		ChangeDirectory(m_CurrentDirectory);
	}

	ContentBrowserItem::ContentBrowserItem(const std::string& name, const UUID& id, Shared<Texture> icon, ItemType type)
		: m_Name(name), m_Icon(icon), m_ID(id), m_Type(type)
	{ }

	void ContentBrowserItem::BeginRender()
	{
		ImGui::PushID(&m_ID);
		ImGui::BeginGroup();
	}

	void ContentBrowserItem::EndRender()
	{
		ImGui::PopID();
	}

	ItemAction ContentBrowserItem::OnRender()
	{
		ItemAction action = ItemAction::None;

		const float cellSize = 74.0f;
		const float edgeOffset = 4.0f;

		bool isSelected = SelectionManager::IsSelected(SelectionContext::ContentPanel, m_ID);

		const float textLineHeight = ImGui::GetTextLineHeightWithSpacing() * 2.0f + edgeOffset * 2.0f;
		const float infoPanelHeight = std::min(false ? cellSize * 0.5f : textLineHeight, textLineHeight);

		const ImVec2 topLeft = ImGui::GetCursorScreenPos();
		const ImVec2 thumbBottomRight = { topLeft.x + cellSize, topLeft.y + cellSize };
		const ImVec2 infoTopLeft = { topLeft.x, topLeft.y + cellSize };
		const ImVec2 bottomRight = { topLeft.x + cellSize, topLeft.y + cellSize + infoPanelHeight };
		ImRect itemBox = ImRect(topLeft, bottomRight);

		if (ImGui::IsMouseHoveringRect(itemBox.Min, itemBox.Max) || isSelected)
		{
			auto* drawList = ImGui::GetWindowDrawList();
			ImGuiStyle& style = ImGui::GetStyle();

			drawList->AddRectFilled(topLeft, bottomRight,
				ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Header]), 6.0f);
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 4.0f });

		ImGui::InvisibleButton("##thumbnailButton", { cellSize, cellSize });
		UI::ShiftCursor(edgeOffset, -cellSize);
		UI::Image((ImTextureID)m_Icon->GetTextureID(), { cellSize - edgeOffset * 2.0, cellSize - edgeOffset * 2.0 });
		
		UI::ShiftCursor(edgeOffset, edgeOffset);

		// file name
		{
			ImGui::BeginVertical((std::string("InfoPanel") + m_Name).c_str(), ImVec2(cellSize - edgeOffset * 2.0f, infoPanelHeight - edgeOffset));
			{
				ImGui::BeginHorizontal(m_Name.c_str(), { cellSize - 2.0f, 0.0f });
				ImGui::Spring();
				ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + (cellSize - edgeOffset * 3.0f));
				if (m_IsRenaming) 
				{
					ImGui::SetNextItemWidth(cellSize - edgeOffset * 3.0f);
					ImGui::SetKeyboardFocusHere();
					if (ImGui::InputText("##rename", s_RenameBuffer, MAX_RENAME_BUFFER_SIZE, ImGuiInputTextFlags_EnterReturnsTrue)) 
					{
						OnRename(s_RenameBuffer);
						StopRename();
					}
				}
				else 
				{
					const float textWidth = std::min(ImGui::CalcTextSize(m_Name.c_str()).x, cellSize);
					ImGui::SetNextItemWidth(textWidth);
					ImGui::Text(m_Name.c_str());
				}
				ImGui::PopTextWrapPos();
				ImGui::Spring();
				ImGui::EndHorizontal();
				ImGui::Spring();
			}
			ImGui::EndVertical();
		}

		ImGui::PopStyleVar();

		UI::ShiftCursor(-edgeOffset, -edgeOffset);

		if (Input::IsKeyPressed(Key::F2) && isSelected && !m_IsRenaming)
			StartRename();

		ImGui::EndGroup();

		if (!ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) 
		{
			if (m_IsRenaming)
				StopRename();
			else
				SelectionManager::Deselect(SelectionContext::ContentPanel);
		}

		if (m_Type == ItemType::Directory)
		{
			if (ImGui::BeginDragDropTarget()) 
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET")) 
					action = ItemAction::MoveTo;

				ImGui::EndDragDropTarget();
			}
		}

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::SetDragDropPayload("ASSET", m_ID.GetRaw(), sizeof(UUID));
			ImGui::Text("File %s", m_Name.c_str());
			action = ItemAction::Select;
			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemHovered()) 
		{
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				m_IsClicked = true;

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !isSelected && m_IsClicked) 
			{
				if(action != ItemAction::MoveTo)
					action = ItemAction::Select;

				m_IsClicked = false;
			}

			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) 
				action = OnActivate();
		}

		if (ImGui::BeginPopupContextItem("cbi_actions_context_menu"))
		{
			action = ItemAction::Select;
			if (ImGui::MenuItem("Rename")) {}

			ImGui::EndPopup();
		}

		//// some indent calculations, just did something that didn't look bad
		//float textIndent = ((cellSize + 10.0f) - textWidth) * 0.5f;
		//textIndent = textIndent <= 0.0f ? 1.0f : textIndent
		//ImGui::TextWrapped(m_Name.c_str());
		//ImGui::SetCursorPosX(cursorPos.x);


		return action;
	}

	void ContentBrowserItem::StartRename()
	{
		if (m_IsRenaming) return;

		memcpy(s_RenameBuffer, m_Name.c_str(), m_Name.size());
		m_IsRenaming = true;
	}

	void ContentBrowserItem::StopRename()
	{
		memset(s_RenameBuffer, 0, MAX_RENAME_BUFFER_SIZE);
		m_IsRenaming = false;
	}

	ContentBrowserDirectory::ContentBrowserDirectory(const Shared<DirectoryInfo>& directoryInfo)
		: ContentBrowserItem(directoryInfo->Path.filename().string(), directoryInfo->ID, EditorResources::GetDirectoryTexture(), ItemType::Directory), m_DirectoryInfo(directoryInfo)
	{ }

	void ContentBrowserDirectory::Move(const std::filesystem::path & newPath)
	{
		auto sourcePath = Project::GetAssetPath() / m_DirectoryInfo->Path;
		auto destinationPath = Project::GetAssetPath() / newPath / m_DirectoryInfo->Path.filename();
		FileSystem::Rename(sourcePath, destinationPath);
	}

	ItemAction ContentBrowserDirectory::OnActivate() { return ItemAction::NavigateTo; }
	ItemAction ContentBrowserDirectory::OnRename(const std::string& newName)
	{
		auto sourcePath = Project::GetAssetPath() / m_DirectoryInfo->Path;
		auto destinationPath = Project::GetAssetPath() / m_DirectoryInfo->Path.parent_path() / newName;
		FileSystem::Rename(sourcePath, destinationPath);

		return ItemAction::Renamed;
	}

	ContentBrowserAsset::ContentBrowserAsset(const AssetInfo& assetInfo, const Shared<Texture>& icon)
		: ContentBrowserItem(assetInfo.Path.stem().string(), assetInfo.Handle, icon, ItemType::File), m_AssetInfo(assetInfo)
	{ }

	void ContentBrowserAsset::Move(const std::filesystem::path & newPath)
	{
		auto sourcePath = Project::GetAssetPath() / m_AssetInfo.Path;
		auto destinationPath = Project::GetAssetPath() / newPath / m_AssetInfo.Path.filename();
		FileSystem::Rename(sourcePath, destinationPath);
	}

	ItemAction ContentBrowserAsset::OnActivate() { return ItemAction::Activate; }
	ItemAction ContentBrowserAsset::OnRename(const std::string& newName)
	{
		auto sourcePath = Project::GetAssetPath() / m_AssetInfo.Path;
		auto destinationPath = Project::GetAssetPath() / m_AssetInfo.Path.parent_path() / (newName + m_AssetInfo.Path.extension().string());
		FileSystem::Rename(sourcePath, destinationPath);
		
		return ItemAction::Renamed;
	}
}
#pragma warning(pop)
