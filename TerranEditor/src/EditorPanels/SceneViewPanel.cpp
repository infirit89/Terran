#include "SceneViewPanel.h"

#include "EditorLayer.h"
#include "SelectionManager.h"
#include "EditorResources.h"
#include "Graphics/Renderer.h"

#include "Utils/Debug/OptickProfiler.h"

#include "UI/UI.h"

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#pragma warning(push, 0)
#include <ImGuizmo.h>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4312)

namespace TerranEditor 
{
	bool operator!=(const glm::vec2& vec1, const ImVec2& vec2) { return vec1.x != vec2.x || vec1.y != vec2.y; }

	static int s_PixelData = -1;
	static int s_PreviousPixelData = s_PixelData;

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

	void SceneViewPanel::OnRender()
	{ 
		TR_PROFILE_FUNCTION();
		if (!m_Open) return;

		Shared<Framebuffer> framebuffer = m_SceneRenderer->GetFramebuffer();
		EditorCamera& editorCamera = EditorLayer::GetInstace()->GetEditorCamera();

		UI::ScopedStyleVar styleVar(
		{
			{ ImGuiStyleVar_WindowPadding, {0.0f, 0.0f} }
		});

		ImGui::Begin(GetName().data(), &m_Open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);

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

		Shared<Texture2D> colorAttachment = framebuffer->GetColorAttachment(0);

		m_Scene->SetViewportPosition({ viewportOffset.x, viewportOffset.y });
		ImGui::Image(
			(ImTextureID)(colorAttachment ? colorAttachment->GetHandle() : -1),
			regionAvail, { 0, 1 }, { 1, 0 });

#if 0
		{
			UI::ScopedStyleVar test({
				{ ImGuiStyleVar_WindowRounding, { 0.2f, 0.2f } }
			});
			constexpr float playButtonSize = 20.0f;
			constexpr float padding = 20.0f;
			const ImVec2 windowSize = { playButtonSize + padding, playButtonSize + padding };
			ImGui::SetNextWindowPos({ ImGui::GetWindowPos().x + regionAvail.x * 0.5f, ImGui::GetWindowPos().y + 30.0f });
			ImGui::BeginChild("toolbar", windowSize, false, ImGuiWindowFlags_NoDecoration);
			Shared<Texture> playButtonIcon = EditorResources::GetPlayTexture();

			ImVec2 playButtonPos = { (ImGui::GetContentRegionAvail().x * 0.5f) - (playButtonSize * 0.7f), 
									(ImGui::GetContentRegionAvail().y * 0.5f) - (playButtonSize * 0.65f) };
			ImGui::SetCursorPos(playButtonPos);
			ImGui::ImageButton((ImTextureID)playButtonIcon->GetTextureID(), { playButtonSize, playButtonSize }, { 0, 1 }, { 1, 0 });
			ImGui::EndChild();
		}
#endif

		m_Visible = ImGui::IsItemVisible();

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(viewportBounds[0].x, viewportBounds[0].y, viewportBounds[1].x - viewportBounds[0].x, viewportBounds[1].y - viewportBounds[0].y);
			
		if (sceneState == SceneState::Edit) 
		{
			// Gizmos
			DrawGizmos(editorCamera.GetProjection(), editorCamera.GetView());
			
			ImVec2 mousePos = ImGui::GetMousePos();
			mousePos.x -= viewportBounds[0].x;
			mousePos.y -= viewportBounds[0].y;

			mousePos.y = m_ViewportSize.y - mousePos.y;

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGuizmo::IsUsing())
			{
				int mouseX = (int)mousePos.x;
				int mouseY = (int)mousePos.y;

				if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)m_ViewportSize.x && mouseY < (int)m_ViewportSize.y)
				{
					framebuffer->Bind();

					Renderer::Submit([framebuffer, mouseX, mouseY]() 
					{
						s_PixelData = framebuffer->ReadPixel_RT(1, mouseX, mouseY);
					});

					framebuffer->Unbind();
				}
			}

			if (s_PixelData != s_PreviousPixelData)
			{
				Entity entity = s_PixelData == -1 ? Entity() : Entity((entt::entity)s_PixelData, m_Scene->GetRaw());

				SelectionManager::DeselectAll(SelectionContext::Scene);
				if (entity)
					SelectionManager::Select(entity);

				s_PreviousPixelData = s_PixelData;
			}
		}

		if (ImGui::BeginDragDropTarget()) 
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET"))
			{
				UUID id = UUID::CreateFromRaw((uint8_t*)payload->Data);
				AssetInfo sceneAssetInfo = AssetManager::GetAssetInfo(id);

				if (sceneAssetInfo.Type == AssetType::Scene)
					m_OpenSceneCallback(sceneAssetInfo, m_ViewportSize);
			}
			ImGui::EndDragDropTarget();
		}

		editorCamera.SetBlockInput(ImGuizmo::IsUsing() || !isFocused || !isHovered || m_WindowMoved);

		ImGui::End();
		m_WindowMoved = false;
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

	void SceneViewPanel::DrawGizmos(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
	{
		const auto& selected = SelectionManager::GetSelected(SelectionContext::Scene);
		if (selected.empty())
			return;

		// TODO: add multiselect functionality
		Entity selectedEntity = m_Scene->FindEntityWithUUID(selected[0]);
		bool usingGizmo = false;
		if (selectedEntity)
		{
			bool altPressed = Input::IsKeyPressed(Key::LeftAlt) || Input::IsKeyPressed(Key::RightAlt);
			if (altPressed)
				m_UseSnapping = true;
			else
				m_UseSnapping = false;

			auto& tc = selectedEntity.GetComponent<TransformComponent>();

			glm::mat4 transformMatrix = tc.WorldSpaceTransformMatrix;

			m_GizmoMode = selectedEntity.HasParent() ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

			ImGuizmo::OPERATION gizmoOperation = ConvertToImGuizmoOperation(m_GizmoType);

			ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix),
				gizmoOperation, (ImGuizmo::MODE)m_GizmoMode, glm::value_ptr(transformMatrix), nullptr, m_UseSnapping ? glm::value_ptr(m_Snap) : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 position, rotation, scale;

				Entity parent = selectedEntity.GetParent();
				if (parent)
				{
					glm::mat4 parentMat = parent.GetTransform().WorldSpaceTransformMatrix;
					transformMatrix = glm::inverse(parentMat) * transformMatrix;
				}

				Math::Decompose(transformMatrix, position, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Rotation += deltaRotation;
				tc.Position = position;
				tc.Scale = scale;
				tc.IsDirty = true;
			}
		}
	}
}
#pragma warning(pop)
