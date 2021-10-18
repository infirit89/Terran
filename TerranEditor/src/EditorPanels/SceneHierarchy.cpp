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

    // TODO: god oh geez please fucking get rid of this mess, this huge pile of shit

    void SceneHierarchy::DrawEntityNode(Entity entity, bool isChild)
    {
        TagComponent& tagComp = entity.GetComponent<TagComponent>();

        // bruuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuh
        if (entity.HasComponent<RelationshipComponent>() && 
            entity.GetComponent<RelationshipComponent>().Parent && 
            !isChild)
            return;

        ImGuiTreeNodeFlags flags = (m_Selected == entity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        flags |= ImGuiTreeNodeFlags_FramePadding;

        // TODO: clean up all this shit

        if (!entity.HasComponent<RelationshipComponent>() || entity.GetComponent<RelationshipComponent>().Children <= 0)
            flags |= ImGuiTreeNodeFlags_NoButton;

        ImGuiStyle& style = ImGui::GetStyle();
        ImGuiStyle orgStyle = style;

        style.FramePadding.y = 1.5f;

        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tagComp.Name.c_str());

        style = orgStyle;

        if (ImGui::IsItemClicked())
        {
            m_Selected = entity;

            if (isChild)
            {
                m_Selected = entity;
                TR_TRACE(m_Selected.GetComponent<TagComponent>().Name);
            }
        }

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

        if (opened)
        {
            if (entity.HasComponent<RelationshipComponent>())
            {
                RelationshipComponent& relComp = entity.GetComponent<RelationshipComponent>();

                if (relComp.Children > 0)
                {
                    Entity currChild = relComp.First;

                    for (size_t i = 0; i < relComp.Children; i++)
                    {
                        DrawEntityNode(currChild, true);

                        currChild = currChild.GetComponent<RelationshipComponent>().Next;
                    }
                }
            }

            ImGui::TreePop();
        }
    }
}

