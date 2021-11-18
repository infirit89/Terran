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
            ImGui::Begin("Hierarchy", &m_Closed, ImGuiWindowFlags_NoCollapse);
            
            auto view = m_Scene->GetRegistry().view<TagComponent>();
            
            for (auto e : view)
            {
                Entity entity(e, m_Scene.get());
                DrawEntityNode(entity, false);

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

    bool SceneHierarchy::DrawEntityNode(Entity entity, bool isChild)
    {
        TagComponent& tagComp = entity.GetComponent<TagComponent>();

        // bruuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuh
        if (entity.HasParent() &&
            !isChild)
            return false;

        ImGuiTreeNodeFlags flags = (m_Selected == entity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        flags |= ImGuiTreeNodeFlags_FramePadding;

        // TODO: clean up all this shit

        //if (!entity.HasComponent<RelationshipComponent>() || entity.GetComponent<RelationshipComponent>().Children <= 0)
        //    flags |= ImGuiTreeNodeFlags_NoButton;

        ImGuiStyle& style = ImGui::GetStyle();
        ImGuiStyle orgStyle = style;

        style.FramePadding.y = 1.5f;

        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tagComp.Name.c_str());

        style = orgStyle;

        if (ImGui::IsItemClicked())
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
                {
                    entity.SetParent(m_Selected);
                    //m_Scene->AddChild(m_Selected, entity);
                }
            }

            ImGui::EndPopup();
        }

        if (opened)
        {
            if (entity.HasComponent<RelationshipComponent>())
            {
                RelationshipComponent& relComp = entity.GetComponent<RelationshipComponent>();

                TR_TRACE("Entity name: {0}, Child count: {1}", entity.GetName(), entity.GetChildCount());

                for (auto eID : entity.GetChildren())
                {
                    auto currEntity = m_Scene->FindEntityWithUUID(eID);

                    if (currEntity)
                    {
                        if (!DrawEntityNode(currEntity, true))
                        {
//                            TR_TRACE("Entity id {0}, Entity name {1}", (uint32_t)currEntity, currEntity.GetName());
                        }
                    }
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

            return true;
        }

        return false;
    }
}
