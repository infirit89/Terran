#include "ECSPanel.h"

#include "Scene/Components.h"
#include "Scene/Entity.h"

#include <imgui.h>

namespace TerranEditor 
{
	static Entity s_Selected;

	void ECSPanel::ImGuiRender()
	{
		if (m_Open) 
		{
			ImGui::Begin("ECS Debug", &m_Open);

			if (ImGui::IsWindowFocused()) 
			{
				ImGui::Columns(3, "ECSDebugPanelCols");

				ImGui::Text("Name");
				ImGui::NextColumn();

				ImGui::Text("UUID");
				ImGui::NextColumn();

				ImGui::Text("Runtime ID");
				ImGui::NextColumn();

				auto tagView = m_Context->GetEntitiesWith<TagComponent>();
			
				for (auto e : tagView)
				{
					Entity entity(e, m_Context.get());

					const bool selected = (s_Selected == entity);
				
					ImGui::Text(entity.GetName().c_str());
					ImGui::NextColumn();

					ImGui::Text(std::to_string(entity.GetID()).c_str());
					ImGui::NextColumn();

					ImGui::Text("%i", (uint32_t)entity);
					ImGui::NextColumn();
				}

				ImGui::Columns(1);

			}

			ImGui::End();
		}
	}
}
