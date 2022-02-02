#include "PropertiesPanel.h"

#include "../UI/TerranEditorUI.h"

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_internal.h>

#include <functional>
#include <filesystem>

namespace TerranEditor 
{
	template<typename Component>
	using UIFunc = std::function<void(Component&)>;

	template<typename T>
	static void DrawComponent(const char* label, Entity entity, UIFunc<T> func, bool removable = true)
	{
		if (entity.HasComponent<T>()) 
		{
			auto& component = entity.GetComponent<T>();

			ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap;

			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			bool treeOpen = ImGui::TreeNodeEx(label, treeFlags);

			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);

			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
				ImGui::OpenPopup("ComponentSettings");

			ImVec4 windowBGColor = ImGui::GetStyleColorVec4(ImGuiCol_PopupBg);
			windowBGColor.w = 1.0f;

			ImGui::PushStyleColor(ImGuiCol_PopupBg, windowBGColor);
			if (ImGui::BeginPopup("ComponentSettings")) 
			{
				if (ImGui::MenuItem("Reset"))
					entity.GetComponent<T>() = T();

				if (ImGui::MenuItem("Remove Component", (const char*)0, false, removable))
					entity.RemoveComponent<T>();

				ImGui::EndPopup();
			}
			ImGui::PopStyleColor();

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
		if(m_Open)
		{
			ImGui::Begin("Properties", &m_Open);
			
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

				DrawComponent<TransformComponent>("Transform", entity, [](TransformComponent& component)
				{
					if (TerranEditorUI::DrawVec3Control("Position", component.Position))
						component.IsDirty = true;

					if (TerranEditorUI::DrawVec3Control("Scale", component.Scale))
						component.IsDirty = true;

					if (TerranEditorUI::DrawVec3Control("Rotation", component.Rotation))
						component.IsDirty = true;

				}, false);

				DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent& component)
				{
					TerranEditorUI::DrawColor4Control("Color", component.Color);
					
					{
						ImGui::PushID("TEXTURE_FIELD");
						ImGui::Columns(2, NULL, false);
						ImGui::SetColumnWidth(0, 100.0f);
						ImGui::Text("Sprite");

						ImGui::NextColumn();
						ImGui::PushItemWidth(ImGui::CalcItemWidth());

						char buf[256];
						memset(buf, 0, sizeof(buf));
						strcpy_s(buf, sizeof(buf), component.Texture == nullptr ? "None" : component.Texture->GetName().c_str());
						ImGui::InputText("##TextureField", buf, sizeof(buf));

						if (ImGui::BeginDragDropTarget()) 
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET")) 
							{
								std::filesystem::path texturePath = (const char*)payload->Data;
								std::string texturePathStr = texturePath.string();
								TextureParameters parameters;
								Shared<Texture> texture = CreateShared<Texture>(texturePathStr.c_str(), parameters);
								component.Texture = texture;
							}

							ImGui::EndDragDropTarget();
						}

						ImGui::Columns(1);

						ImGui::PopID();
					}

					TerranEditorUI::DrawIntControl("Z index", component.ZIndex);
				});

				DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](CircleRendererComponent& component)
				{
					TerranEditorUI::DrawColor4Control("Color", component.Color);
					TerranEditorUI::DrawFloatControl("Thickness", component.Thickness);
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
					if(!entity.HasComponent<SpriteRendererComponent>())
						if (ImGui::MenuItem("Sprite Renderer"))
							entity.AddComponent<SpriteRendererComponent>();
					if (!entity.HasComponent<CircleRendererComponent>())
						if (ImGui::MenuItem("Circle Renderer"))
							entity.AddComponent<CircleRendererComponent>();
					if (!entity.HasComponent<CameraComponent>())
						if (ImGui::MenuItem("Camera"))
							entity.AddComponent<CameraComponent>();

					ImGui::EndPopup();
				}

				ImGui::SetCursorPos(cursorPos);

			}

			ImGui::End();
		}

	}
}

