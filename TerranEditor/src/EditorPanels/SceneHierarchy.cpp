#include "SceneHierarchy.h"

#include <imgui.h>

namespace TerranEditor 
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

	void SceneHierarchy::ImGuiRender()
	{
        if(m_Open)
        {
            ImGui::Begin("Hierarchy", &m_Open, ImGuiWindowFlags_NoCollapse);
            
            if (m_Scene) 
            {
                auto view = m_Scene->GetEntitiesWith<TagComponent>();

				for (auto e : view)
				{
					Entity entity(e, m_Scene.get());

					if (!entity.HasParent())
						DrawEntityNode(entity);
				}

				if (ImGui::BeginPopupContextWindow(0, 1, false))
				{
					if (ImGui::MenuItem("Create an entity"))
						m_Selected = m_Scene->CreateEntity("Entity");

					ImGui::EndPopup();
				}

				if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
					m_Selected = {};
            }

            ImGui::End();
        }

	}

    void SceneHierarchy::DrawEntityNode(Entity entity)
    {
        TagComponent& tagComp = entity.GetComponent<TagComponent>();

        ImGuiTreeNodeFlags flags = (m_Selected == entity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

        if (!entity.HasComponent<RelationshipComponent>() || entity.GetComponent<RelationshipComponent>().Children.size() <= 0)
            flags |= ImGuiTreeNodeFlags_NoButton;
        else
            flags |= ImGuiTreeNodeFlags_DefaultOpen;

        ImGuiStyle& style = ImGui::GetStyle();
        ImGuiStyle orgStyle = style;

        style.FramePadding.y = 1.5f;

        bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, flags, tagComp.Name.c_str());
        
        style = orgStyle;

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right))
            m_Selected = entity;

        bool isDeleted = false;
        
        if (Input::IsKeyPressed(Key::Delete))
            if (m_Selected == entity)
                isDeleted = true;

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Create an Entity")) 
            {
                auto entity = m_Scene->CreateEntity("Entity");

                if (m_Selected) 
                    entity.SetParent(m_Selected);
            }

            if (ImGui::MenuItem("Delete entity")) 
                isDeleted = true;

            ImGui::EndPopup();
        }

        if (opened)
        {
            if (entity.HasComponent<RelationshipComponent>())
            {
                for (auto eID : entity.GetChildren())
                {
                    auto currEntity = m_Scene->FindEntityWithUUID(eID);

                    if (currEntity)
                        DrawEntityNode(currEntity);
                    else
                        TR_ERROR("Not a valid child");
                }
            }
            
            ImGui::TreePop();
        }

        if (isDeleted)
        {
            m_Scene->DestroyEntity(entity, true);
            if (m_Selected == entity)
                m_Selected = {};
        }
    }
}
