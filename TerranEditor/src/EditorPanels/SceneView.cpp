#include "SceneView.h"

#include <ImGui/imgui.h>
#include <ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>

namespace TerranEditor 
{
	void SceneView::ImGuiRender(Entity selectedEntity, EditorCamera& editorCamera)
	{
        if (m_Open) 
        {
            ImGui::Begin("Scene view", &m_Open);

            if (m_Position.x != ImGui::GetWindowPos().x || m_Position.y != ImGui::GetWindowPos().y) 
            {
                m_Position = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };
                m_WindowMoved = true;
            }

            ImVec2 regionAvail = ImGui::GetContentRegionAvail();

            m_ViewportSize = { regionAvail.x, regionAvail.y };

            bool isFocused = ImGui::IsWindowFocused();
            bool isHovered = ImGui::IsWindowHovered();

            Application::Get()->GetImGuiLayer().SetBlockInput(!isFocused || !isHovered);

            ImGui::Image((void*)m_RenderTextureID, regionAvail, { 0, 1 }, { 1, 0 });

            m_Visible = ImGui::IsItemVisible();

            ImGuizmo::SetOrthographic(true);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

            // Gizmos
            if (selectedEntity)
            {
                auto& tc = selectedEntity.GetComponent<TransformComponent>();
                glm::mat4 transformMatrix = tc.TransformMatrix;

                ImGuizmo::Manipulate(glm::value_ptr(glm::inverse(editorCamera.GetView())), glm::value_ptr(editorCamera.GetProjection()),
                    (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transformMatrix));

                if (ImGuizmo::IsUsing())
                {
                    glm::vec3 position, rotation, scale;
                    if (Decompose(transformMatrix, position, rotation, scale))
                    {
                        glm::vec3 deltaPosition = position - tc.Position;
                        tc.LocalPosition += deltaPosition;
                        tc.LocalRotation = rotation;
                        tc.LocalScale = scale;

                        tc.Dirty = true;
                    }
                }
            }

            editorCamera.SetBlockInput(ImGuizmo::IsUsing() || !isFocused || !isHovered || m_WindowMoved);

            ImGui::End();

            m_WindowMoved = false;
        }
	}
    void SceneView::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(TR_EVENT_BIND_FN(SceneView::OnKeyPressed));
    }

    bool SceneView::OnKeyPressed(KeyPressedEvent& e)
    {
        bool ctrlPressed = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);

        if (e.GetRepeatCount() > 0)
            return false;

        switch (e.GetKeyCode())
        {
        case Key::Q: 
        {
            m_GizmoType = ImGuizmo::TRANSLATE;
            break;
        }
        case Key::W:
        {
            m_GizmoType = ImGuizmo::ROTATE;
            break;
        }
        case Key::E:
        {
            m_GizmoType = ImGuizmo::SCALE;
            break;
        }
        }

        return false;
    }
}

