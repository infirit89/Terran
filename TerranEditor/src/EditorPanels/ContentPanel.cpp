#include "ContentPanel.h"

#include "Core/Log.h"

#include <imgui.h>

#pragma warning(push)
#pragma warning(disable : 4312)

namespace TerranEditor 
{
	ContentPanel::ContentPanel(std::filesystem::path resPath) 
		: m_AssetPath(resPath), m_CurrentPath(resPath)
	{
		TextureParameters params;
		m_DirIcon = CreateShared<Texture>("Resources/Textures/temp_folder_icon.png", params);
		m_FileIcon = CreateShared<Texture>("Resources/Textures/temp_file_icon.png", params);
	}

	void ContentPanel::ImGuiRender()
	{
		if (m_Open) 
		{
			if (!std::filesystem::exists(m_CurrentPath)) 
			{
				TR_ERROR("Asset path doesn't exist");
				return;
			}

			ImGui::Begin("Content", &m_Open);
			
			if (m_CurrentPath != std::filesystem::path(m_AssetPath))
			{
				if (ImGui::Button("<-"))
					m_CurrentPath = m_CurrentPath.parent_path();
			}

			const float padding = 32.0f;
			const float cellSize = 74.0f;

			float totalSize = padding + cellSize + 10.0f;
			float availRegionWidth = ImGui::GetContentRegionAvailWidth();
			int columnCount = (int)(availRegionWidth / totalSize) < 1 ? 1 : (int)(availRegionWidth / totalSize);

			ImGui::Columns(columnCount, (const char*)0, false);

			// for every entry in the current 
			for (auto& dirEntry : std::filesystem::directory_iterator(m_CurrentPath))
			{
				const auto& entryPath = dirEntry.path();
				
				std::string entryName = entryPath.filename().string();
				
				ImGui::PushID(entryName.c_str());

				ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });

				Shared<Texture> entryIcon = dirEntry.is_directory() ? m_DirIcon : m_FileIcon;

				// if the entry is a directory then use the folder icon else use the file icon
				ImGui::ImageButton((ImTextureID)entryIcon->GetTextureID(), { cellSize + 10.0f, cellSize }, { 0, 1 }, { 1, 0 });

				ImGui::PopStyleColor();

				if (!dirEntry.is_directory()) 
				{
					if (ImGui::BeginDragDropSource()) 
					{
						std::string entryPathStr = entryPath.string();
						ImGui::SetDragDropPayload("ASSET", entryPathStr.c_str(), entryPathStr.size() + 1);

						ImGui::Text("File %s", entryName.c_str());

						ImGui::EndDragDropSource();
					}
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && dirEntry.is_directory()) 
				{
					// if the entry is a folder and is clicked then concatinate that current path
					// with the folder name
					m_CurrentPath /= entryPath.filename();
				}

				float textWidth = ImGui::CalcTextSize(entryName.c_str()).x;
				ImVec2 cursorPos = ImGui::GetCursorPos();

				// some indent calculations, just did something that didn't look bad
				float textIndent = ((cellSize + 20.0f) - textWidth) * 0.5f;
				textIndent = textIndent <= 0.0f ? 1.0f : textIndent;

				ImGui::SetCursorPosX(cursorPos.x + textIndent);
				ImGui::TextWrapped(entryName.c_str());
				ImGui::SetCursorPosX(cursorPos.x);

				ImGui::NextColumn();

				ImGui::PopID();
			}

			ImGui::Columns(1);

			ImGui::End();
		}
	}
}
#pragma warning(pop)
