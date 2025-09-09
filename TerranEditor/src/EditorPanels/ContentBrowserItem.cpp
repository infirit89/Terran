#include "ContentBrowserItem.h"
#include "ContentBrowserPanel.h"

#include "SelectionManager.h"

#include "EditorResources.h"

#include "Core/Input.h"
#include "Project/Project.h"
#include "Utils/Debug/OptickProfiler.h"

#include "UI/UI.h"

#include <imgui_internal.h>

namespace TerranEditor 
{
	#define MAX_RENAME_BUFFER_SIZE 256
	static char s_RenameBuffer[MAX_RENAME_BUFFER_SIZE]{ 0 };

	ContentBrowserItem::ContentBrowserItem(const std::string& name, const UUID& handle, Shared<Texture> icon, ItemType type)
		: m_Name(name), m_Icon(icon), m_Handle(handle), m_Type(type)
	{ }

	void ContentBrowserItem::BeginRender()
	{
		ImGui::PushID(&m_Handle);
		ImGui::BeginGroup();
	}

	void ContentBrowserItem::EndRender()
	{
		ImGui::PopID();
	}

	ItemAction ContentBrowserItem::OnRender()
	{
		TR_PROFILE_FUNCTION();
		ItemAction action = ItemAction::None;

		const float cellSize = 74.0f;
		const float edgeOffset = 4.0f;
		const int maxFileNameLength = 8;

		bool isSelected = SelectionManager::IsSelected(SelectionContext::ContentPanel, m_Handle);

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
		UI::Image(m_Icon, { cellSize - edgeOffset * 2.0, cellSize - edgeOffset * 2.0 });

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
					ImGui::Text("%s", formattedName.c_str());
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
			else if(SelectionManager::IsSelected(SelectionContext::ContentPanel, m_Handle))
				SelectionManager::Deselect(SelectionContext::ContentPanel, m_Handle);
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
			ImGui::SetDragDropPayload("ASSET", m_Handle.GetRaw(), sizeof(UUID));
			ImGui::Text("%s", m_Name.c_str());
			action = ItemAction::Select;
			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				if (action != ItemAction::MoveTo && !isSelected)
					action = ItemAction::Select;
			}

			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				action = OnActivate();
		}

		if (UI::BeginPopupContextItem("cbi_actions_context_menu"))
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
		: ContentBrowserItem(directoryInfo->Path.filename().string(), directoryInfo->Handle, EditorResources::DirectoryTexture, ItemType::Directory), m_DirectoryInfo(directoryInfo)
	{ }

	void ContentBrowserDirectory::Move(const std::filesystem::path& newPath)
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

	void ContentBrowserAsset::Move(const std::filesystem::path& newPath)
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
