#include "LogPanel.h"

#include "UI/UI.h"
#include "EditorResources.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace TerranEditor 
{
	LogPanel* LogPanel::s_Instance;

	LogPanel::LogPanel() 
	{
		s_Instance = this;
		ClearMessageBuffer();
	}

	LogPanel::~LogPanel() 
	{
		ClearMessageBuffer();
	}

	void LogPanel::AddLogMessage(LogMessage logMessage) 
	{
		m_TextBuffer.push_back(logMessage);
	}

	void LogPanel::OnRender() 
	{
		if (!m_Open) return;
		
		ImGui::Begin(GetName(), &m_Open);

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
			const float framePaddingY = ImGui::GetStyle().FramePadding.y;
			
			UI::ScopedFont mediumFont("Roboto-Regular_Medium");
			//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
			
			for (size_t i = 0; i < m_TextBuffer.size(); i++)
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				
				LogMessage logMessage = m_TextBuffer[i];

				ImDrawList* backgroundDrawList = ImGui::GetWindowDrawList();

				ImVec2 cursorPos = ImGui::GetCursorScreenPos();

				/*ImU32 color;

				if(i % 2 == 0)
					color = ImGui::ColorConvertFloat4ToU32({ 0.19f, 0.19f, 0.19f, 1.0f });
				else
					color = ImGui::ColorConvertFloat4ToU32({ 0.13f, 0.13f, 0.13f, 1.0f });*/

				//float x = cursorPos.x - 2.0f;
				//float y = cursorPos.y - 2.0f;

				//ImVec2 textSize = ImGui::CalcTextSize(logMessage.Message.c_str());

				//backgroundDrawList->AddRectFilled({ x, y }, { x + ImGui::GetContentRegionAvail().x + 2.0f,  y + textSize.y + 8.0f}, color);

				ImGui::Selectable(logMessage.Message.c_str());

				const float lineHeight = ImGui::GetItemRectSize().x - framePaddingY / 2.0f;

				/*ImGui::Indent(4.0f);

				switch (logMessage.MessageLevel)
				{
				case LogMessageLevel::Trace:
				case LogMessageLevel::Info:
				{
					UI::Image(EditorResources::InfoTexture, { lineHeight, lineHeight }, { 0, 1 }, { 1, 0 }, { 0.0f, 0.0f, 1.0f, 1.0f });
					break;
				}
				case LogMessageLevel::Warn:
				{
					UI::Image(EditorResources::WarningTexture, { lineHeight, lineHeight }, { 0, 1 }, { 1, 0 }, { 1.0f, 1.0f, 0.0f, 1.0f });
					break;
				}
				case LogMessageLevel::Error:
				{
					UI::Image(EditorResources::ErrorTexture, { lineHeight, lineHeight }, { 0, 1 }, { 1, 0 }, { 1.0f, 0.0f, 0.0f, 1.0f });
					break;
				}
				}

				ImGui::SameLine();

				UI::ShiftCursorY(2.0f);*/

				//ImGui::TextUnformatted(logMessage.Message.c_str());
				//ImGui::Unindent(4.0f);

				//UI::ShiftCursorY(-2.0f);
			}

			//ImGui::PopStyleVar();

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
