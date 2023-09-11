#include "ContentBrowserPanel.h"

#include "EditorResources.h"

#include "Core/Log.h"
#include "Core/FileUtils.h"
#include "Core/Input.h"
#include "Core/FileUtils.h"

#include "Assets/AssetManager.h"

#include "Project/Project.h"

#include "Utils/Utils.h"

#include "SelectionManager.h"

#include "UI/UI.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <algorithm>
#include <optional>

#pragma warning(push)
#pragma warning(disable : 4312)

namespace TerranEditor
{
#define MAX_RENAME_BUFFER_SIZE 256

	static char s_RenameBuffer[MAX_RENAME_BUFFER_SIZE]{ 0 };

	ContentPanel::ContentPanel()
	{
		AssetManager::SetAssetChangedCallback(TR_EVENT_BIND_FN(ContentPanel::OnFileSystemChanged));
	}

	template<typename AssetType>
	void DrawNewAssetMenu(ContentPanel* panel, const char* name, const char* fileName, bool& openRenamePopup)
	{
		if (ImGui::MenuItem(name))
		{
			openRenamePopup = true;
			panel->m_NewAssetName = fileName;
			panel->m_CreateAsset = [panel](const std::string& name) 
			{ 
				panel->CreateAssetInDirectory<AssetType>(name, panel->m_CurrentDirectory->Path); 
			};
		}
	}

	static std::mutex s_ContentBrowserMutex;
	void ContentPanel::ImGuiRender()
	{
		if (!m_Open) return;

		ImGui::Begin("Content", &m_Open);

		ImGui::BeginChild("##topbar", { 0.0f, 40.0f });
		ImGui::BeginHorizontal("##topbar", ImGui::GetWindowSize());
		{
			auto button = [this](const std::string& buttonText, bool condition)
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
			if (button("<-", m_PreviousDirectory != nullptr))
				ChangeBackwardDirectory();

			ImGui::Spring(-1.0f, 4.0f);
			
			if (button("->", m_NextDirectoryStack.size() > 0))
				ChangeForwardDirectory();
			
			ImGui::Spring(-1.0f, 4.0f * 2.0f);
			if (ImGui::Button("Reload"))
				Refresh();
		}
		ImGui::EndHorizontal();
		ImGui::EndChild();

		bool openRenamePopup = false;
		if (ImGui::BeginPopupContextWindow("content_panel_actions_popup", ImGuiMouseButton_Right)) 
		{
			if (ImGui::MenuItem("Reveal in explorer"))
				FileSystem::RevealInExplorer(AssetManager::GetFileSystemPath(m_CurrentDirectory->Path));

			if (ImGui::BeginMenu("New")) 
			{
				DrawNewAssetMenu<PhysicsMaterial2DAsset>(this, "Physics Material", "New Physics Material.trpm2d", openRenamePopup);
				DrawNewAssetMenu<Scene>(this, "Scene", "New Scene.terran", openRenamePopup);
				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}

		// TODO: this is pretty scuffed; should do it the proper way
		if (openRenamePopup) 
		{
			ImGui::OpenPopup("name_new_asset");
			memcpy(s_RenameBuffer, m_NewAssetName.stem().string().c_str(), m_NewAssetName.stem().string().size());
		}

		if (ImGui::BeginPopup("name_new_asset"))
		{
			if(!ImGui::IsAnyItemActive())
				ImGui::SetKeyboardFocusHere();

			bool result = ImGui::InputText("##name_new_asset_input", s_RenameBuffer, MAX_RENAME_BUFFER_SIZE, ImGuiInputTextFlags_EnterReturnsTrue);

			if (ImGui::Button("Create") || result) 
			{
				std::string fileName = s_RenameBuffer + m_NewAssetName.extension().string();
				m_CreateAsset(fileName);
				memset(s_RenameBuffer, 0, MAX_RENAME_BUFFER_SIZE);
				ImGui::CloseCurrentPopup();
			}

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

			// TODO: change with the imgui tables api
			ImGui::Columns(columnCount, (const char*)0, false);

			// for every entry in the current 
			for (const auto& item : m_CurrentItems)
			{
				item->BeginRender();
				ItemAction action = item->OnRender();

				if (action == ItemAction::NavigateTo) 
				{
					nextDir = DynamicCast<ContentBrowserDirectory>(item)->GetDirectoryInfo();
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

					SelectionManager::Deselect(SelectionContext::ContentPanel);
					if (selectedItem != m_CurrentItems.end()) 
					{
						std::filesystem::path directoryPath = m_Directories[item->GetID()]->Path;
						(*selectedItem)->Move(directoryPath);
					}
				}

				if (action == ItemAction::StartRename)
					item->StartRename();

				ImGui::NextColumn();
				item->EndRender();
			}

			ImGui::Columns(1);
		}

		if (nextDir)
			ChangeDirectory(nextDir);

		ImGui::End();
	}

	void ContentPanel::OnEvent(TerranEngine::Event& event)
	{
		EventDispatcher dispatcher(event);
		//dispatcher.Dispatch<KeyPressedEvent>(TR_EVENT_BIND_FN(ContentPanel::OnKeyPressedEvent));
	}

	static void PrintDirectoryInfo(const Shared<DirectoryInfo>& parent, int level = 0)
	{
		std::string childStr = std::string(level, '-');

		if (level == 0)
			TR_TRACE("Assets");
		else
			TR_TRACE("{0}{1}", childStr, parent->Path.filename().string());

		for (const auto& [handle, subdirectory] : parent->Subdirectories)
			PrintDirectoryInfo(subdirectory, level + 1);
	}

	void ContentPanel::RefreshDirectory(const Shared<DirectoryInfo>& directory)
	{
		if (!directory) return;

		if (!DirectoryExists(directory)) 
		{
			RemoveDirectoryInfo(directory);
			return;
		}

		// TODO: handle asset refreshing
		auto tempAssets = directory->Assets;
		auto tempSubdirectories = directory->Subdirectories;
		std::filesystem::file_time_type currentTime = FileSystem::GetModifiedTime(AssetManager::GetFileSystemPath(directory->Path));
		bool updated = currentTime != directory->ModifiedTime;
		
		for (const auto& [handle, subdirectory] : tempSubdirectories)
			RefreshDirectory(subdirectory);

		if (!updated) return;
		
		directory->ModifiedTime = currentTime;
		for (const auto& assetHandle : tempAssets)
		{
			AssetInfo assetInfo = AssetManager::GetAssetInfo(assetHandle);
			if (!assetInfo)
			{
				auto assetHandleIt = std::find(directory->Assets.begin(), directory->Assets.end(), assetHandle);
				directory->Assets.erase(assetHandleIt);
			}
		}

		// techinically the path isn't absolute
		std::filesystem::path absoluteDirectoryPath = Project::GetAssetPath() / directory->Path;
		for (const auto& directoryEntry : std::filesystem::directory_iterator(absoluteDirectoryPath))
		{
			if (directoryEntry.is_directory()) 
			{
				UUID subdirectoryHandle = ProcessDirectory(directoryEntry, directory);
				if(directory->Subdirectories.find(subdirectoryHandle) == directory->Subdirectories.end())
					directory->Subdirectories.emplace(subdirectoryHandle, m_Directories[subdirectoryHandle]);
				continue;
			}

			UUID assetHandle = AssetManager::ImportAsset(directoryEntry);
			auto assetIt = std::find(directory->Assets.begin(), directory->Assets.end(), assetHandle);
			if (assetHandle && assetIt == directory->Assets.end()) 
			{
				directory->Assets.emplace_back(assetHandle);
				AssetInfo info = AssetManager::GetAssetInfo(assetHandle);
			}
		}
	}

	void ContentPanel::RemoveDirectoryInfo(const Shared<DirectoryInfo>& directory)
	{
		auto parent = directory->Parent;
		parent->Subdirectories.erase(directory->ID);

		for (const auto& [handle, subdirectory] : directory->Subdirectories) 
			m_Directories.erase(subdirectory->ID);

		m_Directories.erase(directory->ID);
	}

	void ContentPanel::UpdateCurrentItems()
	{
		//std::scoped_lock<std::mutex> lock(s_ContentBrowserMutex);
		auto tempItems = m_CurrentItems;
		for (const auto& item : tempItems)
		{
			auto& subdirectories = m_CurrentDirectory->Subdirectories;
			auto& assets = m_CurrentDirectory->Assets;

			bool directoryExists = subdirectories.find(item->GetID()) != subdirectories.end();
			bool assetExists = std::find(assets.begin(), assets.end(), item->GetID()) != assets.end();
			if (!directoryExists && !assetExists) m_CurrentItems.Erase(item);
		}

		for (const auto& [id, subdirectory] : m_CurrentDirectory->Subdirectories)
		{
			auto it = std::find_if(m_CurrentItems.begin(), m_CurrentItems.end(), [&subdirectory = subdirectory](Shared<ContentBrowserItem> item)
			{
				return item->GetID() == subdirectory->ID;
			});

			if (it == m_CurrentItems.end()) m_CurrentItems.Items.push_back(CreateShared<ContentBrowserDirectory>(subdirectory));
		}

		for (const auto& assetHandle : m_CurrentDirectory->Assets)
		{
			auto it = std::find_if(m_CurrentItems.begin(), m_CurrentItems.end(), [&assetHandle](Shared<ContentBrowserItem> item)
			{
				return item->GetID() == assetHandle;
			});

			if (it == m_CurrentItems.end())
			{
				AssetInfo info = AssetManager::GetAssetInfo(assetHandle);
				m_CurrentItems.Items.push_back(CreateShared<ContentBrowserAsset>(info, EditorResources::GetFileTexture()));
			}
		}

		SortItems();
	}

	void ContentPanel::SortItems()
	{
		std::sort(m_CurrentItems.begin(), m_CurrentItems.end(),
		[](const Shared<ContentBrowserItem>& left, const Shared<ContentBrowserItem>& right)
		{
			if(left->GetType() == right->GetType())
				return Utils::ToLower(left->GetName()) < Utils::ToLower(right->GetName());
			
			return (uint8_t)left->GetType() < (uint8_t)right->GetType();
		});
	}

	void ContentPanel::ChangeForwardDirectory()
	{
		ChangeDirectory(m_NextDirectoryStack.top());
		m_NextDirectoryStack.pop();
	}

	void ContentPanel::ChangeBackwardDirectory()
	{
		m_NextDirectoryStack.push(m_CurrentDirectory);
		ChangeDirectory(m_PreviousDirectory);
	}

#if NOT_WORKING
	void ContentPanel::Refresh()
	{
		std::scoped_lock<std::mutex> lock(s_ContentBrowserMutex);
		auto rootDirectory = GetDirectory("");

		RefreshDirectory(rootDirectory);
		//ProcessDirectory(Project::GetAssetPath());

		// NOTE: try this solution if the current one is slow
		//auto tempDirectories = m_DirectoryInfoMap;

		//for (const auto& [handle, directory] : tempDirectories)
		//{
		//	if (!DirectoryExists(directory)) 
		//	{
		//		TR_TRACE(directory->Path);
		//		auto parent = directory->Parent;
		//		parent->Subdirectories.erase(directory->ID);
		//		
		//		for (const auto& [childHandle, subdirectory] : directory->Subdirectories)
		//			m_DirectoryInfoMap.erase(subdirectory->ID);

		//		m_DirectoryInfoMap.erase(directory->ID);

		//		continue;
		//		// TODO: handle deletion
		//	}


		//}

		bool shouldChangeCurrent = false;
		while (!DirectoryExists(m_CurrentDirectory)) 
		{
			m_CurrentDirectory = m_CurrentDirectory->Parent;
			shouldChangeCurrent = true;
		}

		if (shouldChangeCurrent) ChangeDirectory(m_CurrentDirectory);
		else UpdateCurrentItems();
	}
#endif

	void ContentPanel::Refresh() 
	{
		std::scoped_lock<std::mutex> lock(s_ContentBrowserMutex);
		m_Directories.clear();
		UUID dirID = ProcessDirectory(Project::GetAssetPath());
		// NOTE: this chages the directory to the base asset path;
		// pretty fucking dumb as the user may have been in another directory before the refresh
		// should fix
		ChangeDirectory(GetDirectory(dirID));
	}

	bool ContentPanel::OnKeyPressedEvent(KeyPressedEvent& kEvent)
	{
		/*bool altPressed = Input::IsKeyDown(Key::LeftAlt) || Input::IsKeyDown(Key::RightAlt);

		if (kEvent.GetRepeatCount() > 0) 
			return false;

		switch (kEvent.GetKeyCode())
		{
		case Key::Left: ChangeBackwardDirectory(); break;
		case Key::Right: ChangeForwardDirectory(); break;
		}*/

		return false;
	}

	bool ContentPanel::DirectoryExists(const Shared<DirectoryInfo>& directory)
	{
		std::filesystem::path p = Project::GetAssetPath() / directory->Path;
		std::error_code e;
		
		bool result = std::filesystem::exists(p, e);
		TR_TRACE(e.message());
		return result;
	}

	UUID ContentPanel::ProcessDirectory(const std::filesystem::path& directoryPath, const Shared<DirectoryInfo>& parent)
	{
		const auto& directory = GetDirectory(directoryPath);
		if (directory) return directory->ID;

		Shared<DirectoryInfo> directoryInfo = CreateShared<DirectoryInfo>();
		directoryInfo->ID = UUID();
		directoryInfo->Parent = parent;
		directoryInfo->ModifiedTime = FileSystem::GetModifiedTime(directoryPath);

		if (directoryPath == Project::GetAssetPath())
			directoryInfo->Path = "";
		else
			directoryInfo->Path = std::filesystem::relative(directoryPath, Project::GetAssetPath());

		for (auto& directoryEntry : std::filesystem::directory_iterator(directoryPath))
		{
			if (directoryEntry.is_directory()) 
			{
				UUID subdirectoryID = ProcessDirectory(directoryEntry, directoryInfo);
				directoryInfo->Subdirectories.emplace(subdirectoryID, m_Directories[subdirectoryID]);
			}
			else 
			{
				UUID assetID = AssetManager::ImportAsset(directoryEntry);

				if(assetID)
					directoryInfo->Assets.push_back(assetID);
			}
		}

		m_Directories[directoryInfo->ID] = directoryInfo;
		return directoryInfo->ID;
	}

	Shared<DirectoryInfo> ContentPanel::GetDirectory(const std::filesystem::path& directoryPath)
	{
		for (const auto&[id, directoryInfo] : m_Directories)
		{
			//TR_TRACE(Project::GetAssetPath());
			std::filesystem::path relative = directoryPath;
			if(!relative.is_relative())
				relative = std::filesystem::relative(directoryPath, Project::GetAssetPath());
			if (directoryInfo->Path == relative)
				return directoryInfo;
		}

		return nullptr;
	}

	Shared<DirectoryInfo> ContentPanel::GetDirectory(const UUID& id) 
	{
		if (m_Directories.find(id) != m_Directories.end())
			return m_Directories.at(id);

		return nullptr;
	}

	void ContentPanel::ChangeDirectory(const Shared<DirectoryInfo>& directory)
	{
		if (!directory) return;

		m_CurrentItems.Items.clear();

		for (const auto& [handle, subdirectory] : directory->Subdirectories)
			m_CurrentItems.Items.push_back(CreateShared<ContentBrowserDirectory>(subdirectory));

		for (const auto& assetHandle : directory->Assets)
		{
			AssetInfo info = AssetManager::GetAssetInfo(assetHandle);
			m_CurrentItems.Items.push_back(CreateShared<ContentBrowserAsset>(info, EditorResources::GetFileTexture()));
		}

		SortItems();

		m_CurrentDirectory = directory;
		m_PreviousDirectory = m_CurrentDirectory->Parent;
	}

	void ContentPanel::OnFileSystemChanged(const std::vector<TerranEngine::FileSystemChangeEvent>& events)
	{
		Refresh();
	}

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
		const int maxFileNameLength = 8;

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
					std::string formattedName = m_Name;
					if (formattedName.size() > maxFileNameLength)
						formattedName = formattedName.replace(maxFileNameLength, formattedName.size() - 1, "...");
					const float textWidth = std::min(ImGui::CalcTextSize(formattedName.c_str()).x, cellSize);
					ImGui::SetNextItemWidth(textWidth);
					ImGui::Text(formattedName.c_str());
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
			ImGui::Text("% s", m_Name.c_str());
			action = ItemAction::Select;
			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemHovered()) 
		{
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				if(action != ItemAction::MoveTo && !isSelected)
					action = ItemAction::Select;
			}

			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) 
				action = OnActivate();
		}

		if (ImGui::BeginPopupContextItem("cbi_actions_context_menu"))
		{
			if (ImGui::MenuItem("Rename"))
				action = ItemAction::StartRename;

			ImGui::EndPopup();
		}

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
