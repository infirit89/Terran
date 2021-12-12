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
            
            auto view = m_Scene->GetEntitiesWith<TagComponent>();
            
            for (auto e : view)
            {
                Entity entity(e, m_Scene.get());

                if(!entity.HasParent())
                    DrawEntityNode(entity);
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

    // poc code not final

    // TODO: god oh geez please fucking get rid of this mess, this huge pile of shit

    void SceneHierarchy::DrawEntityNode(Entity entity)
    {
        TagComponent& tagComp = entity.GetComponent<TagComponent>();

        ImGuiTreeNodeFlags flags = (m_Selected == entity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        flags |= ImGuiTreeNodeFlags_FramePadding;

        // TODO: clean up all this shit

        if (!entity.HasComponent<RelationshipComponent>() || entity.GetComponent<RelationshipComponent>().Children.size() <= 0)
            flags |= ImGuiTreeNodeFlags_NoButton;

        ImGuiStyle& style = ImGui::GetStyle();
        ImGuiStyle orgStyle = style;

        style.FramePadding.y = 1.5f;

        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tagComp.Name.c_str());

        style = orgStyle;

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right))
            m_Selected = entity;

        bool isDeleted = false;
        
        if (Input::IsKeyPressed(Key::Delete))
            if (m_Selected == entity)
                isDeleted = true;

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete entity")) 
                isDeleted = true;

            if (ImGui::MenuItem("Create Entity")) 
            {
                auto entity = m_Scene->CreateEntity("Entity");

                if (m_Selected) 
                    entity.SetParent(m_Selected);
            }

            ImGui::EndPopup();
        }

        if (opened)
        {
            if (entity.HasComponent<RelationshipComponent>())
            {
                RelationshipComponent& relComp = entity.GetComponent<RelationshipComponent>();

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
