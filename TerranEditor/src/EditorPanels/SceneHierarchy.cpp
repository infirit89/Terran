#include "SceneHierarchy.h"

#include <imgui.h>

namespace TerranEngine 
{
	SceneHierarchy::SceneHierarchy(const Shared<Scene>& scene)
	{
		SetScene(scene);
	}

	void SceneHierarchy::SetScene(const Shared<Scene>& scene)
	{
		m_Scene = scene;
		m_Selected = {};
	}

    void SceneHierarchy::SetClosed(bool closed)
    {
        m_Closed = closed;
    }

	void SceneHierarchy::ImGuiRender()
	{
        if(m_Closed)
        {
            ImGui::Begin("Hierarchy", &m_Closed);
            
            auto view = m_Scene->GetRegistry().view<TagComponent>();
            for (auto e : view)
            {
                Entity entity(e, m_Scene.get());

                TagComponent& tagComp = entity.GetComponent<TagComponent>();

                char buf[256];
                sprintf_s(buf, sizeof(buf), "%s", tagComp.Name.c_str());

                // TODO: if entity is a parent than render a imgui tree
                if (ImGui::Selectable(buf, m_Selected == entity))
                    m_Selected = entity;

                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem("Delete entity")) 
                    {
                        m_Scene->DestroyEntity(entity);
                        if (m_Selected == entity)
                            m_Selected = {};
                    }

                    ImGui::EndPopup();
                }

            }
            if (ImGui::BeginPopupContextWindow(0, 1, false))
            {
                if (ImGui::MenuItem("Create an entity"))
                    m_Scene->CreateEntity("Entity");

                ImGui::EndPopup();
            }

            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
                m_Selected = {};

            ImGui::End();
        }

	}
}

