#include "PropertiesPanel.h"

#include "../EditorUI/TerranEditorUI.h"

#include <imgui.h>

namespace TerranEngine 
{
	void PropertiesPanel::ImGuiRender(Entity& entity)
	{
		if(m_Closed)
		{
			ImGui::Begin("Properties", &m_Closed);
			if (entity) 
			{
				if (entity.HasComponent<TransformComponent>()) 
				{
					auto& transformComp = entity.GetComponent<TransformComponent>();

					TerranEditorUI::DrawVec3Control("Position", transformComp.Position);
					TerranEditorUI::DrawVec3Control("Rotation", transformComp.Rotation);
					TerranEditorUI::DrawVec3Control("Scale",	transformComp.Scale);
				}

			}
			ImGui::End();
		}

	}
}

