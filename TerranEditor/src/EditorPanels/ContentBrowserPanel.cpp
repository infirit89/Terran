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
#if REWORK
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
#endif

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
					SelectionManager::Select(SelectionContext::ContentPanel, item->GetHandle());

				if (action == ItemAction::MoveTo)
				{
					auto selectedItem = std::find_if(m_CurrentItems.begin(), m_CurrentItems.end(), [](Shared<ContentBrowserItem> browserItem)
					{
						return browserItem->GetHandle() == SelectionManager::GetSelected(SelectionContext::ContentPanel);
					});

					SelectionManager::Deselect(SelectionContext::ContentPanel);
					if (selectedItem != m_CurrentItems.end()) 
					{
						std::filesystem::path directoryPath = m_Directories[item->GetHandle()]->Path;
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

	void ContentPanel::UpdateCurrentItems()
	{
		//std::scoped_lock<std::mutex> lock(s_ContentBrowserMutex);
		auto tempItems = m_CurrentItems;
		for (const auto& item : tempItems)
		{
			auto& subdirectories = m_CurrentDirectory->Subdirectories;
			auto& assets = m_CurrentDirectory->Assets;

			bool directoryExists = subdirectories.find(item->GetHandle()) != subdirectories.end();
			bool assetExists = std::find(assets.begin(), assets.end(), item->GetHandle()) != assets.end();
			if (!directoryExists && !assetExists) m_CurrentItems.Erase(item);
		}

		for (const auto& [id, subdirectory] : m_CurrentDirectory->Subdirectories)
		{
			auto it = std::find_if(m_CurrentItems.begin(), m_CurrentItems.end(), [&subdirectory = subdirectory](Shared<ContentBrowserItem> item)
			{
				return item->GetHandle() == subdirectory->Handle;
			});

			if (it == m_CurrentItems.end()) m_CurrentItems.Items.push_back(CreateShared<ContentBrowserDirectory>(subdirectory));
		}

		for (const auto& assetHandle : m_CurrentDirectory->Assets)
		{
			auto it = std::find_if(m_CurrentItems.begin(), m_CurrentItems.end(), [&assetHandle](Shared<ContentBrowserItem> item)
			{
				return item->GetHandle() == assetHandle;
			});

			if (it == m_CurrentItems.end())
			{
				AssetInfo info = AssetManager::GetAssetInfo(assetHandle);
				m_CurrentItems.Items.push_back(CreateShared<ContentBrowserAsset>(info, EditorResources::FileTexture));
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

	void ContentPanel::Refresh() 
	{
		std::filesystem::path currentDirectoryPath = m_CurrentDirectory->Path;

		std::scoped_lock<std::mutex> lock(s_ContentBrowserMutex);
		m_Directories.clear();
		ProcessDirectory(Project::GetAssetPath());
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
				directory->Subdirectories.emplace(subdirectoryHandle, m_Directories[subdirectoryHandle]);
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

	void ContentPanel::OnProjectChanged(const std::filesystem::path& projectPath)
	{
		UUID projectRootDirectoryHandle = ProcessDirectory(Project::GetAssetPath());
		m_CurrentDirectory = GetDirectory(projectRootDirectoryHandle);
		ChangeDirectory(m_CurrentDirectory);
	}
}
#pragma warning(pop)
