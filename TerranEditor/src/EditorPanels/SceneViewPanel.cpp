#include "SceneViewPanel.h"

#include "EditorLayer.h"

#include "UI/UI.h"

#include <glm/gtc/type_ptr.hpp>

#include <ImGui/imgui.h>

#pragma warning(push, 0)
#include <ImGuizmo.h>
#pragma warning(pop)

#include <functional>

#pragma warning(push)
#pragma warning(disable : 4312)

namespace TerranEditor 
{
	bool operator!=(const glm::vec2& vec1, const ImVec2& vec2) { return vec1.x != vec2.x || vec1.y != vec2.y; }

	static ImGuizmo::OPERATION ConvertToImGuizmoOperation(GizmoType gizmoType) 
	{
		switch (gizmoType)
		{
		case TerranEditor::GizmoType::None:			return (ImGuizmo::OPERATION)0;
		case TerranEditor::GizmoType::Translaste:	return ImGuizmo::TRANSLATE;
		case TerranEditor::GizmoType::Rotate:		return ImGuizmo::ROTATE;
		case TerranEditor::GizmoType::Scale:		return ImGuizmo::SCALE;
		}

		return (ImGuizmo::OPERATION)0;
	}

	void SceneViewPanel::ImGuiRender(Entity selectedEntity, EditorCamera& editorCamera)
	{
		if (m_Open) 
		{ 
			UI::ScopedStyleVar styleVar(
			{
				{ ImGuiStyleVar_WindowPadding, {0.0f, 0.0f} }
			});

			ImGui::Begin("Scene view", &m_Open);
			
			bool isFocused = ImGui::IsWindowFocused();
			bool isHovered = ImGui::IsWindowHovered();

			if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
				ImGui::SetWindowFocus();

			SceneState sceneState = EditorLayer::GetInstace()->GetSceneState();

			if (m_Position.x != ImGui::GetWindowPos().x || m_Position.y != ImGui::GetWindowPos().y) 
			{
				m_Position = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };
				m_WindowMoved = true;
			}

			ImVec2 regionAvail = ImGui::GetContentRegionAvail();

			if (m_ViewportSize != regionAvail) 
			{
				m_ViewportSize = { regionAvail.x, regionAvail.y };
				m_ViewportSizeChangedCallback(m_ViewportSize);
			}

			ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
			ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			ImVec2 viewportOffset = ImGui::GetWindowPos();

			glm::vec2 viewportBounds[2] = {
				{ viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y },
				{ viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y }
			};

			Application::Get()->GetImGuiLayer().SetBlockInput(!isFocused || !isHovered);

			ImGui::Image((ImTextureID)m_RenderTextureID, regionAvail, { 0, 1 }, { 1, 0 });

			m_Visible = ImGui::IsItemVisible();

			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(viewportBounds[0].x, viewportBounds[0].y, viewportBounds[1].x - viewportBounds[0].x, viewportBounds[1].y - viewportBounds[0].y);

			// Gizmos
			if (selectedEntity && sceneState == SceneState::Edit)
			{
				bool altPressed = Input::IsKeyPressed(Key::LeftAlt) || Input::IsKeyPressed(Key::RightAlt);
				if (altPressed)
					m_UseSnapping = true;
				else
					m_UseSnapping = false;

				const glm::mat4& cameraProjection = editorCamera.GetProjection();
				glm::mat4 cameraView = editorCamera.GetView();

				auto& tc = selectedEntity.GetComponent<TransformComponent>();
				
				glm::mat4 transformMatrix = tc.WorldTransformMatrix;

				if (selectedEntity.HasParent())
					m_GizmoMode = ImGuizmo::LOCAL;
				else
					m_GizmoMode = ImGuizmo::WORLD;

				ImGuizmo::OPERATION gizmoOperation = ConvertToImGuizmoOperation(m_GizmoType);

				ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
					gizmoOperation, (ImGuizmo::MODE)m_GizmoMode, glm::value_ptr(transformMatrix), nullptr, m_UseSnapping ? glm::value_ptr(m_Snap) : nullptr);

				if (ImGuizmo::IsUsing())
				{
					glm::vec3 position, rotation, scale;

					if (selectedEntity.HasParent()) 
					{
						glm::mat4 parentMat = selectedEntity.GetParent().GetWorldMatrix();

						transformMatrix = glm::inverse(parentMat) * transformMatrix;
					}

					if (Math::Decompose(transformMatrix, position, rotation, scale))
					{
						tc.Position = position;
						tc.Scale = scale;
						tc.Rotation = rotation;

						tc.IsDirty = true;
					}
				}
			}

			if (ImGui::BeginDragDropTarget()) 
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET"))
				{
					const char* entryPath = (const char*)payload->Data;
					m_OpenSceneCallback(entryPath, m_ViewportSize);
				}

				ImGui::EndDragDropTarget();
			}

			editorCamera.SetBlockInput(ImGuizmo::IsUsing() || !isFocused || !isHovered || m_WindowMoved);

			ImGui::End();

			m_WindowMoved = false;
		}
	}

	void SceneViewPanel::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(TR_EVENT_BIND_FN(SceneViewPanel::OnKeyPressed));
	}

	bool SceneViewPanel::OnKeyPressed(KeyPressedEvent& e)
	{
		SceneState sceneState = EditorLayer::GetInstace()->GetSceneState();

		if (e.GetRepeatCount() > 0 || sceneState == SceneState::Play)
			return false;

		switch (e.GetKeyCode())
		{
		case Key::Q:	m_GizmoType = GizmoType::None; break;
		case Key::W:	m_GizmoType = GizmoType::Translaste; break;
		case Key::E:	m_GizmoType = GizmoType::Rotate; break;
		case Key::R:	m_GizmoType = GizmoType::Scale; break;
		}

		return false;
	}
}
#pragma warning(pop)
