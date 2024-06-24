#include "LogPanel.h"

#include "Utils/Debug/OptickProfiler.h"

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
		TR_PROFILE_FUNCTION();
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
		
		float framePaddingY = ImGui::GetStyle().FramePadding.y;
		ImGui::BeginChild("MessageRegion", { 0, 0 }, false, ImGuiWindowFlags_HorizontalScrollbar);
		if (ImGui::BeginTable("log_table", 1, ImGuiTableFlags_RowBg))
		{
			const float framePaddingY = ImGui::GetStyle().FramePadding.y;
			
			UI::ScopedFont mediumFont("Roboto-Regular_Medium");
			
			ImGuiListClipper logTextClipper;
			logTextClipper.Begin(static_cast<int>(m_TextBuffer.size()));

			while(logTextClipper.Step()) 
			{
				for (size_t i = logTextClipper.DisplayStart; i < logTextClipper.DisplayEnd; i++)
				{
					ImGui::TableNextRow();

					ImU32 color;
					if (i % 2 == 0)
						color = ImGui::GetColorU32({ 0.19f, 0.19f, 0.19f, 1.0f });
					else
						color = ImGui::GetColorU32({ 0.13f, 0.13f, 0.13f, 1.0f });

					ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, color);
					ImGui::TableNextColumn();

					LogMessage logMessage = m_TextBuffer[i];
					ImDrawList* backgroundDrawList = ImGui::GetWindowDrawList();
					ImGui::TextUnformatted(logMessage.Message.c_str());
				}
			}

			logTextClipper.End();

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
