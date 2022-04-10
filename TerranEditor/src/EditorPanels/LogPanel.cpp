#include "LogPanel.h"

#include "../UI/UI.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace TerranEditor 
{
	LogPanel* LogPanel::s_Instance;

	LogPanel::LogPanel() 
	{
		TextureParameters params;
		//params.MagFilter = TextureFilter::NEAREST;
		//params.MinFilter = TextureFilter::NEAREST;
		m_InfoTexture = CreateShared<Texture>("Resources/Textures/info-icon.png", params);
		m_WarnTexture = CreateShared<Texture>("Resources/Textures/warning-icon.png", params);
		m_ErrorTexture = CreateShared<Texture>("Resources/Textures/error-icon.png", params);

		s_Instance = this;
		ClearMessageBuffer();
	}

	LogPanel::~LogPanel() 
	{
		ClearMessageBuffer();
	}

	void LogPanel::AddLog(LogMessage logMessage) 
	{
		m_TextBuffer.push_back(logMessage);
	}

	void LogPanel::ImGuiRender() 
	{
		if (!m_Open)
			return;
		
		ImGui::Begin("Log", &m_Open);

		if (ImGui::BeginTable("log_setting_table", 3, ImGuiTableFlags_BordersV | ImGuiTableFlags_SizingFixedFit, { 0.0f, 25.0f }))
		{
			ImGui::TableSetupColumn("clear_column", ImGuiTableColumnFlags_WidthFixed, 60.0f);
			ImGui::TableNextColumn();
			if (ImGui::Button("Clear"))
				ClearMessageBuffer();
			
			ImGui::TableNextColumn();
			ImGui::Checkbox("Auto scroll", &m_AutoScroll);

			ImGui::TableNextColumn();
			ImGui::Checkbox("Clear on play", &m_ClearOnPlay);
			
			ImGui::EndTable();
		}

		ImGui::Separator();
		
		ImGui::BeginChild("MessageRegion", { 0, 0 }, false, ImGuiWindowFlags_HorizontalScrollbar);
		if (ImGui::BeginTable("log_table", 1))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
			for (size_t i = 0; i < m_TextBuffer.size(); i++)
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				LogMessage logMessage = m_TextBuffer[i];

				ImDrawList* backgroundDrawList = ImGui::GetWindowDrawList();

				ImVec2 cursorPos = ImGui::GetCursorScreenPos();

				ImU32 color;

				if(i % 2 == 0)
					color = ImGui::ColorConvertFloat4ToU32({ 0.19f, 0.19f, 0.19f, 1.0f });
				else
					color = ImGui::ColorConvertFloat4ToU32({ 0.13f, 0.13f, 0.13f, 1.0f });

				float x = cursorPos.x - 2.0f;
				float y = cursorPos.y - 2.0f;

				ImVec2 textSize = ImGui::CalcTextSize(logMessage.Message.c_str());

				backgroundDrawList->AddRectFilled({ x, y }, { x + ImGui::GetContentRegionAvailWidth() + 2.0f,  y + textSize.y + 8.0f }, color);

				ImGui::Indent(4.0f);
				switch (logMessage.MessageLevel)
				{
				case LogMessageLevel::Trace:
				case LogMessageLevel::Info:
				{
					ImGui::Image((ImTextureID)m_InfoTexture->GetTextureID(), { 20.0f, 20.0f }, { 0, 1 }, { 1, 0 });

					break;
				}
				case LogMessageLevel::Warn:
				{
					ImGui::Image((ImTextureID)m_WarnTexture->GetTextureID(), { 20.0f, 20.0f }, { 0, 1 }, { 1, 0 });
					break;
				}
				case LogMessageLevel::Error:
				{
					ImGui::Image((ImTextureID)m_ErrorTexture->GetTextureID(), { 20.0f, 20.0f }, { 0, 1 }, { 1, 0 });
					break;
				}
				}

				ImGui::SameLine();

				float cursorPosY = ImGui::GetCursorPosY();
				ImGui::SetCursorPosY(cursorPosY + 2.0f);

				ImGui::TextUnformatted(logMessage.Message.c_str());
				ImGui::Unindent(4.0f);

				ImGui::SetCursorPosY(cursorPosY);
			}

			ImGui::PopStyleVar();

			ImGui::EndTable();

			if (m_AutoScroll && ImGui::GetScrollY() == ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);

		}
		ImGui::EndChild();

		ImGui::End();
	}

	void LogPanel::ClearMessageBuffer() 
	{
		m_TextBuffer.clear();
	}
}