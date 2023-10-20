#include "ContentBrowserPanel.h"

#include "EditorResources.h"

#include "Core/Log.h"
#include "Core/FileUtils.h"
#include "Core/Input.h"

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
	#define MAX_NAME_BUFFER_SIZE 256
	static char s_NameBuffer[MAX_NAME_BUFFER_SIZE]{ 0 };

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
	void ContentPanel::OnRender()
	{
		if (!m_Open) return;

		ImGuiWindowFlags contentBrowserFlags = ImGuiWindowFlags_NoScrollbar;

		ImGui::Begin("Content", &m_Open, contentBrowserFlags);

		if (ImGui::BeginTable("content_browser_table", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable)) 
		{
			//const float folderViewColumnWidth = 100.0f;
			//ImGui::TableSetupColumn("folder_view", 0, folderViewColumnWidth);
			//ImGui::TableSetupColumn("detailed_view", 0, ImGui::GetContentRegionAvail().x - folderViewColumnWidth);
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			RenderSideBar();

			ImGui::TableNextColumn();
			RenderTopBar();
			ImGui::Separator();

			// render items
			if(ImGui::BeginChild("content_browser_item_view"))
			{
				// content panel actions popup
				{
					bool openRenamePopup = false;
					auto drawNewDirectoryMenu = [&openRenamePopup, this](const char* name, const char* fileName)
						{
							if (ImGui::MenuItem(name))
							{
								openRenamePopup = true;
								m_NewAssetName = fileName;
								m_CreateAsset = [this](const std::string& name)
									{
										CreateNewDirectory(name, m_CurrentDirectory->Path);
									};
							}
						};

					if (UI::BeginPopupContextWindow("content_panel_actions_popup"))
					{
						if (ImGui::MenuItem("Reveal in explorer"))
							FileSystem::RevealInExplorer(AssetManager::GetFileSystemPath(m_CurrentDirectory->Path));

						if (ImGui::BeginMenu("New"))
						{
							DrawNewAssetMenu<PhysicsMaterial2DAsset>(this, "Physics Material", "New Physics Material.trpm2d", openRenamePopup);
							DrawNewAssetMenu<Scene>(this, "Scene", "New Scene.terran", openRenamePopup);
							drawNewDirectoryMenu("Folder", "New Folder");
							ImGui::EndMenu();
						}

						ImGui::EndPopup();
					}

					// TODO: this is pretty scuffed; should do it the proper way
					if (openRenamePopup)
					{
						ImGui::OpenPopup("name_new_asset");
						memcpy(s_NameBuffer, m_NewAssetName.stem().string().c_str(), m_NewAssetName.stem().string().size());
					}

					if (ImGui::BeginPopup("name_new_asset"))
					{
						if (!ImGui::IsAnyItemActive())
							ImGui::SetKeyboardFocusHere();

						bool result = ImGui::InputText("##name_new_asset_input", s_NameBuffer, MAX_NAME_BUFFER_SIZE, ImGuiInputTextFlags_EnterReturnsTrue);

						if (ImGui::Button("Create") || result)
						{
							std::string fileName = s_NameBuffer + m_NewAssetName.extension().string();
							m_CreateAsset(fileName);
							memset(s_NameBuffer, 0, MAX_NAME_BUFFER_SIZE);
							ImGui::CloseCurrentPopup();
						}

						ImGui::EndPopup();
					}
				}

				std::scoped_lock<std::mutex> lock(s_ContentBrowserMutex);
				const float padding = 18.0f;
				const float cellSize = 74.0f;

				float totalSize = padding + cellSize;
				float availRegionWidth = ImGui::GetContentRegionAvail().x;
				int columnCount = (int)(availRegionWidth / totalSize) < 1 ? 1 : (int)(availRegionWidth / totalSize);

				if (!ImGui::BeginTable("##content_browser_items", columnCount))
					return;

				ImGui::TableNextRow();

				Shared<DirectoryInfo> directoryToOpen;

				// for every entry in the current directory 
				for (const auto& item : m_CurrentItems)
				{
					ImGui::TableNextColumn();
					item->BeginRender();
					ItemAction action = item->OnRender();

					if (action == ItemAction::NavigateTo) 
					{
						directoryToOpen = DynamicCast<ContentBrowserDirectory>(item)->GetDirectoryInfo();
						while (!m_NextDirectoryStack.empty()) m_NextDirectoryStack.pop();
					}
					else if (action == ItemAction::Activate)
					{
					}
					else if (action == ItemAction::Select)
						SelectionManager::Select(SelectionContext::ContentPanel, item->GetHandle());
					else if (action == ItemAction::MoveTo)
						MoveSelectedItemTo(m_Directories[item->GetHandle()]);
					else if (action == ItemAction::StartRename)
						item->StartRename();

					item->EndRender();
				}

				ImGui::EndTable();

				if (directoryToOpen)
					ChangeDirectory(directoryToOpen);

				ImGui::EndChild();
			}
			ImGui::EndTable();
		}

		ImGui::End();
	}

	void ContentPanel::RenderTopBar()
	{
		if (!ImGui::BeginChild("##topbar", { 0.0f, 20.0f }))
			return;

		ImGui::BeginHorizontal("##topbar", ImGui::GetWindowSize());
		{
			constexpr float baseSpacing = 4.0f;

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

			ImGui::Spring(-1.0f, baseSpacing);

			if (button("->", m_NextDirectoryStack.size() > 0))
				ChangeForwardDirectory();

			ImGui::Spring(-1.0f, baseSpacing * 2.0f);
			if (ImGui::Button("Reload"))
				Refresh();

			ImGui::Spring(-1.0f, baseSpacing * 2.0f);
			Shared<DirectoryInfo> nextDirectory = nullptr;
			for (int i = 0; i < m_BreadCrumbs.size(); i++)
			{
				if (i > 0)
				{
					ImGui::Text(">");
					ImGui::Spring(-1.0f, baseSpacing);
				}

				std::string name = m_BreadCrumbs[i] == m_RootDirectory ? 
													"Assets" : 
													m_BreadCrumbs[i]->Path.stem().string();

				ImGui::PushID(&(m_BreadCrumbs[i]->Handle));
				if (ImGui::Button(name.c_str()))
					nextDirectory = m_BreadCrumbs[i];

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET"))
						MoveSelectedItemTo(m_BreadCrumbs[i]);

					ImGui::EndDragDropTarget();
				}
				ImGui::PopID();
				ImGui::Spring(-1.0f, baseSpacing);
			}

			if (nextDirectory)
				ChangeDirectory(nextDirectory);
		}
		ImGui::EndHorizontal();
		ImGui::EndChild();
	}

	void ContentPanel::RenderSideBar()
	{
		if (!ImGui::BeginChild("##sidebar", { 0.0f, 0.0f }))
			return;

		RenderDirectoryTree(m_RootDirectory);

		ImGui::EndChild();
	}

	void ContentPanel::RenderDirectoryTree(Shared<DirectoryInfo> parent)
	{
		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_SpanAvailWidth |
										ImGuiTreeNodeFlags_FramePadding |
										ImGuiTreeNodeFlags_AllowItemOverlap | 
										ImGuiTreeNodeFlags_OpenOnArrow |
										ImGuiTreeNodeFlags_OpenOnDoubleClick;

		bool opened = false;
		if (parent == m_CurrentDirectory)
			nodeFlags |= ImGuiTreeNodeFlags_Selected;

		ImGui::BeginHorizontal("##directoryView");
		nodeFlags |= parent->Subdirectories.empty() ? 
						ImGuiTreeNodeFlags_Leaf : 
							parent == m_RootDirectory ?
								ImGuiTreeNodeFlags_DefaultOpen :
								0;

		std::string id = fmt::format("##{0}", parent->Path.stem().string());
		opened = UI::TreeNodeEx(id.c_str(), nodeFlags);
		ImGui::BeginVertical("##directoryVertical", { 0.0f, ImGui::GetItemRectSize().y });
		ImGui::Spring(0.1f, 0.0f);
		ImGui::BeginHorizontal("##directoryInfo", { ImGui::GetItemRectSize().x, 0.0f }, 1.0f);
		ImGui::Spring(0.0f, opened ? 11.0f : 6.0f);
		UI::Image((ImTextureID)EditorResources::DirectoryTexture->GetHandle(), { 15.0f, 15.0f });

		if (parent == m_RootDirectory)
		{
			ImGuiIO& io = ImGui::GetIO();
			ImGui::PushFont(io.Fonts->Fonts[TR_BOLD_FONT_INDEX]);
			ImGui::Text("Assets");
			ImGui::PopFont();
		}
		else
			ImGui::Text(parent->Path.stem().string().c_str());

		ImGui::EndHorizontal();
		ImGui::Spring(0.1f, 0.0f);
		ImGui::EndVertical();
		ImGui::EndHorizontal();
		

		if (ImGui::IsItemClicked())
			ChangeDirectory(parent);

		if (ImGui::BeginDragDropTarget()) 
		{
			ImGuiDragDropFlags dragDropFlags = ImGuiDragDropFlags_AcceptNoDrawDefaultRect;
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET", dragDropFlags))
				MoveSelectedItemTo(parent);

			ImGui::EndDragDropTarget();
		}

		if (opened)
		{
			for (const auto& [subdirectoryId, subdirectory] : parent->Subdirectories)
				RenderDirectoryTree(subdirectory);

			ImGui::TreePop();
		}
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

	// NOTE: there's a macro from windows with the name RemoveDirectory
	UUID ContentPanel::RemoveDirectoryInfo(Shared<DirectoryInfo> directory)
	{
		if (!directory) return UUID::Invalid();

		auto parent = directory->Parent;
		parent->Subdirectories.erase(directory->Handle);

		for (const auto& [handle, subdirectory] : directory->Subdirectories) 
			m_Directories.erase(subdirectory->Handle);

		m_Directories.erase(directory->Handle);
		return directory->Handle;
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

	void ContentPanel::Refresh() 
	{
		// NOTE: why the fuck is everything taking an absolute path?
		std::filesystem::path currentDirectoryPath = AssetManager::GetFileSystemPath(m_CurrentDirectory->Path);

		std::scoped_lock<std::mutex> lock(s_ContentBrowserMutex);
		m_Directories.clear();
		UUID rootDirectoryHandle = ProcessDirectory(Project::GetAssetPath());
		m_RootDirectory = GetDirectory(rootDirectoryHandle);
		ChangeDirectory(GetDirectory(currentDirectoryPath));
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

	UUID ContentPanel::ProcessDirectory(const std::filesystem::path& directoryPath, Shared<DirectoryInfo> parent)
	{
		Shared<DirectoryInfo> directory = GetDirectory(directoryPath);
		if (directory) return directory->Handle;

		directory = CreateShared<DirectoryInfo>();
		directory->Handle = UUID();
		directory->Parent = parent;

		if (parent)
			parent->Subdirectories.emplace(directory->Handle, directory);
		
		if (directoryPath == Project::GetAssetPath())
			directory->Path = ".";
		else
			directory->Path = std::filesystem::relative(directoryPath, Project::GetAssetPath());

		for (auto& directoryEntry : std::filesystem::directory_iterator(directoryPath))
		{
			if (directoryEntry.is_directory()) 
			{
				UUID subdirectoryHandle = ProcessDirectory(directoryEntry, directory);
			}
			else
			{
				UUID assetID = AssetManager::ImportAsset(directoryEntry);

				if(assetID)
					directory->Assets.push_back(assetID);
			}
		}

		m_Directories[directory->Handle] = directory;
		return directory->Handle;
	}

	Shared<DirectoryInfo> ContentPanel::GetDirectory(const std::filesystem::path& directoryPath)
	{
		for (const auto&[handle, directoryInfo] : m_Directories)
		{
			std::filesystem::path relative = std::filesystem::relative(directoryPath, Project::GetAssetPath());
			if (directoryInfo->Path == relative)
				return directoryInfo;
		}

		return nullptr;
	}

	Shared<DirectoryInfo> ContentPanel::GetDirectory(const UUID& handle) 
	{
		if (m_Directories.find(handle) != m_Directories.end())
			return m_Directories.at(handle);

		return nullptr;
	}

	void ContentPanel::MoveSelectedItemTo(const Shared<DirectoryInfo>& directory)
	{
		auto selectedItem = std::find_if(m_CurrentItems.begin(), m_CurrentItems.end(), [](Shared<ContentBrowserItem> browserItem)
			{
				return browserItem->GetHandle() == SelectionManager::GetSelected(SelectionContext::ContentPanel);
			});

		SelectionManager::Deselect(SelectionContext::ContentPanel);
		if (selectedItem != m_CurrentItems.end())
		{
			std::filesystem::path directoryPath = directory->Path;
			(*selectedItem)->Move(directoryPath);
		}
	}

	void ContentPanel::ChangeDirectory(const Shared<DirectoryInfo>& directory)
	{
		if (!directory) return;

		m_CurrentItems.Items.clear();

		for (const auto& [handle, subdirectory] : directory->Subdirectories)
			m_CurrentItems.AddDirectory(subdirectory);

		for (const auto& assetHandle : directory->Assets)
		{
			AssetInfo info = AssetManager::GetAssetInfo(assetHandle);
			m_CurrentItems.AddAsset(info, EditorResources::FileTexture);
		}

		SortItems();

		m_CurrentDirectory = directory;
		m_PreviousDirectory = m_CurrentDirectory->Parent;

		m_BreadCrumbs.clear();
		FillBreadCrumbs();
	}

	void ContentPanel::OnFileSystemChanged(const std::vector<TerranEngine::FileSystemChangeEvent>& events)
	{
		std::scoped_lock<std::mutex> lock(s_ContentBrowserMutex);

		for (const auto& event : events)
		{
			switch (event.Action)
			{
			case FileAction::Added: 
			{
				std::filesystem::path path = AssetManager::GetFileSystemPath(event.FileName);
				Shared<DirectoryInfo> parentDirectory = GetDirectory(path.parent_path());

				if (std::filesystem::is_directory(path)) 
				{
					UUID directoryHandle = ProcessDirectory(path, parentDirectory);
					if (m_CurrentDirectory == parentDirectory)
						m_CurrentItems.AddDirectory(GetDirectory(directoryHandle));
				}
				else 
				{
					UUID assetHandle = AssetManager::ImportAsset(path);

					if (assetHandle) 
					{
						parentDirectory->Assets.push_back(assetHandle);
						if (m_CurrentDirectory == parentDirectory)
							m_CurrentItems.AddAsset(AssetManager::GetAssetInfo(assetHandle), EditorResources::FileTexture);
					}
				}
				break;
			}
			case FileAction::Removed: 
			{
				std::filesystem::path path = AssetManager::GetFileSystemPath(event.FileName);
				Shared<DirectoryInfo> parentDirectory = GetDirectory(path.parent_path());

				UUID assetHandle = AssetManager::GetAssetInfo(event.FileName).Handle;

				if (assetHandle)
				{
					auto it = std::find(parentDirectory->Assets.begin(), parentDirectory->Assets.end(), assetHandle);
					parentDirectory->Assets.erase(it);

					m_CurrentItems.Erase(assetHandle);
				}
				else 
				{
					UUID removedDirectoryHandle = RemoveDirectoryInfo(GetDirectory(path));
					m_CurrentItems.Erase(removedDirectoryHandle);
				}

				break;
			}
			case FileAction::Renamed: 
			{
				if(std::filesystem::is_directory(AssetManager::GetFileSystemPath(event.FileName)))
				{
					Shared<DirectoryInfo> directory = GetDirectory(AssetManager::GetFileSystemPath(event.OldFileName));
					directory->Path = event.FileName;
					auto it = m_CurrentItems.Find(directory->Handle);
					if (it != m_CurrentItems.end())
						(*(it))->m_Name = event.FileName.filename().string();
				}
				else 
				{
					UUID assetHandle = AssetManager::GetAssetID(event.OldFileName);
					auto it = m_CurrentItems.Find(assetHandle);
					if (it != m_CurrentItems.end())
						(*(it))->m_Name = event.FileName.stem().string();
				}
				break;
			}
			}
		}

		SortItems();
	}

	void ContentPanel::FillBreadCrumbs()
	{
		std::deque<Shared<DirectoryInfo>> directoryNodes;

		Shared<DirectoryInfo> parent = m_CurrentDirectory;
		while (true) 
		{
			directoryNodes.push_back(parent);
			if (parent->Handle == m_RootDirectory->Handle)
				break;

			parent = parent->Parent;
		}

		while (!directoryNodes.empty()) 
		{
			m_BreadCrumbs.push_back(directoryNodes.back());
			directoryNodes.pop_back();
		}
	}

	void ContentPanel::OnProjectChanged(const std::filesystem::path& projectPath)
	{
		m_Directories.clear();
		UUID projectRootDirectoryHandle = ProcessDirectory(Project::GetAssetPath());
		m_RootDirectory = GetDirectory(projectRootDirectoryHandle);
		m_CurrentDirectory = m_RootDirectory;
		ChangeDirectory(m_CurrentDirectory);
	}
}
#pragma warning(pop)
