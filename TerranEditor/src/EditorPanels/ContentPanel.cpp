#include "ContentPanel.h"

#include <imgui.h>

namespace TerranEditor 
{
	ContentPanel::ContentPanel(std::filesystem::path resPath) 
		: m_AssetPath(resPath), m_CurrentPath(resPath)
	{
		TextureParameters params;
		m_DirIcon = CreateShared<Texture>("res/temp_folder_icon_2.png", params);
		m_FileIcon = CreateShared<Texture>("res/temp_file_icon.png", params);
	}

	void ContentPanel::ImGuiRender()
	{
		if (m_Open) 
		{
			ImGui::Begin("Content", &m_Open);
			
			// loop through the directory entries
			// if the entry is a file, display the file icon, else if it is a directory display the directory icon
			// if its a directory make it clickable and when clicked append the name of the directory to the asset path
			// if its a file make it a drag and drop source

			// pseudo code
			/*
			* for(dir_entry in directory)
			*	Texture texture
			*	if(dir_entry is directory)
			*		texture = directory_icon
			*	else
			*		texture = file_icon
			* 
			*	textureButton(texture)
			* 
			*	if(dir_entry is file)
			*		SetUpDragDropSource
			*	
			*	if(dir_entry is directory)
			*		if(textureButton is double clicked)
			*			asset_path append dir_entry.name
			*/
			
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
			
			for (auto& dirEntry : std::filesystem::directory_iterator(m_CurrentPath))
			{
				const auto& entryPath = dirEntry.path();
				
				std::string entryName = entryPath.filename().string();
				
				ImGui::PushID(entryName.c_str());

				ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });

				ImGui::ImageButton((ImTextureID)(dirEntry.is_directory() ? m_DirIcon->GetTextureID() : m_FileIcon->GetTextureID()), 
					{ cellSize + 10.0f, cellSize }, { 0, 1 }, { 1, 0 });

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
					m_CurrentPath /= entryPath.filename();

				float textWidth = ImGui::CalcTextSize(entryName.c_str()).x;
				ImVec2 cursorPos = ImGui::GetCursorPos();

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

