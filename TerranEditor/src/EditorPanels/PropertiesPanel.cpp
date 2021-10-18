#include "PropertiesPanel.h"

#include "../EditorUI/TerranEditorUI.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include <functional>

namespace TerranEngine 
{
	template<typename Component>
	using UIFunc = std::function<void(Component&)>;

	template<typename T>
	static void DrawComponent(const char* label, Entity entity, UIFunc<T> func, bool closable = true)
	{
		if (entity.HasComponent<T>()) 
		{
			auto& component = entity.GetComponent<T>();

			ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap;

			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			bool treeOpen = ImGui::TreeNodeEx(label, treeFlags);

			if (closable) 
			{
				ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);

				if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
					ImGui::OpenPopup("ComponentSettings");

				if (ImGui::BeginPopup("ComponentSettings")) 
				{
					if (ImGui::MenuItem("Remove Component"))
						entity.RemoveComponent<T>();

					ImGui::EndPopup();
				}

			}
			if (treeOpen)
			{
				func(component);
				ImGui::TreePop();
			}

			ImVec2 cursorPos = ImGui::GetCursorPos();
			ImGui::SetCursorPosY(cursorPos.y + 4.0f);
		}
	}

	void PropertiesPanel::ImGuiRender(Entity& entity)
	{
		if(m_Closed)
		{
			ImGui::Begin("Properties", &m_Closed);
			if (entity) 
			{
				if (entity.HasComponent<TagComponent>()) 
				{
					auto& tagComp = entity.GetComponent<TagComponent>();
					char buf[256];
					memset(buf, 0, sizeof(buf));
					strcpy_s(buf, sizeof(buf), tagComp.Name.c_str());

					if (ImGui::InputText("##Tag", buf, sizeof(buf)) && ImGui::IsKeyPressed((int)Key::Enter)) 
						tagComp.Name = buf;
				}
				ImGui::Separator();
				ImGui::SetCursorPosY(ImGui::GetCursorPos().y + 2.0f);

				DrawComponent<TransformComponent>("Transform", entity, [](TransformComponent& componenet)
				{
					TerranEditorUI::DrawVec3Control("Position", componenet.Position);
					TerranEditorUI::DrawVec3Control("Rotation", componenet.Rotation);
					TerranEditorUI::DrawVec3Control("Scale",	componenet.Scale);
				}, false);

				DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent& component)
				{
					TerranEditorUI::DrawColor4Control("Color", component.Color);
					TerranEditorUI::DrawIntControl("Z index", component.ZIndex);
				});

				DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& component)
				{
					TerranEditorUI::DrawColor4Control("Background color", component.BackgroundColor);

					float camSize = component.Camera.GetOrthographicSize();
					if (TerranEditorUI::DrawFloatControl("Size", camSize))
						component.Camera.SetOrthographicSize(camSize);

					ImGui::Text("Clipping planes");

					float camNear = component.Camera.GetOrthographicNear();
					if (TerranEditorUI::DrawFloatControl("Near", camNear))
						component.Camera.SetOrthographicNear(camNear);

					float camFar = component.Camera.GetOrthographicFar();
					if (TerranEditorUI::DrawFloatControl("Far", camFar))
						component.Camera.SetOrthographicFar(camFar);
				});

				ImVec2 cursorPos = ImGui::GetCursorPos();

				ImGui::SetCursorPos(ImVec2{ cursorPos.x + ImGui::GetContentRegionAvailWidth() / 4.0f, cursorPos.y += 5.0f });

				float buttonWidth = (ImGui::GetContentRegionAvailWidth() / 2.0f) + GImGui->Font->FontSize + 5.0f;

				if (ImGui::Button("Add Component", ImVec2{ buttonWidth, 0 }))
					ImGui::OpenPopup("AddComponent");
				
				if (ImGui::BeginPopup("AddComponent")) 
				{
					if (ImGui::MenuItem("Add Sprite Renderer"))
						entity.AddComponent<SpriteRendererComponent>();

					ImGui::EndPopup();
				}

				ImGui::SetCursorPos(cursorPos);

			}
			ImGui::End();
		}

	}
}

