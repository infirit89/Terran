#include "PropertiesPanel.h"

#include "../UI/UI.h"

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

			bool removedComponent = false;

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
					removedComponent = true;

				ImGui::EndPopup();
			}
			ImGui::PopStyleColor();

			if (treeOpen)
			{
				func(component);
				ImGui::TreePop();
			}

			if(removedComponent)
				entity.RemoveComponent<T>();

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


				//TR_TRACE("Entity: {0}, Position: x: {1}, y: {2}, z: {3}", entity.GetName(), entity.GetTransform().Position.x, entity.GetTransform().Position.y, entity.GetTransform().Position.z);

				DrawComponent<TransformComponent>("Transform", entity, [](TransformComponent& component)
				{
					if (UI::DrawVec3Control("Position", component.Position))
						component.IsDirty = true;

					if (UI::DrawVec3Control("Scale", component.Scale))
						component.IsDirty = true;

					if (UI::DrawVec3Control("Rotation", component.Rotation))
						component.IsDirty = true;

				}, false);

				DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent& component)
				{
					UI::DrawColor4Control("Color", component.Color);
					
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
								parameters.MagFilter = TextureFilter::NEAREST;
								parameters.MinFilter = TextureFilter::NEAREST;
								Shared<Texture> texture = CreateShared<Texture>(texturePathStr.c_str(), parameters);
								component.Texture = texture;
							}

							ImGui::EndDragDropTarget();
						}

						ImGui::Columns(1);

						ImGui::PopID();
					}

					UI::DrawIntControl("Z index", component.ZIndex);
				});

				DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](CircleRendererComponent& component)
				{
					UI::DrawColor4Control("Color", component.Color);
					UI::DrawFloatControl("Thickness", component.Thickness);
				});

				DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& component)
				{
					UI::DrawColor4Control("Background color", component.BackgroundColor);

					float camSize = component.Camera.GetOrthographicSize();
					if (UI::DrawFloatControl("Size", camSize))
						component.Camera.SetOrthographicSize(camSize);

					ImGui::Text("Clipping planes");

					float camNear = component.Camera.GetOrthographicNear();
					if (UI::DrawFloatControl("Near", camNear))
						component.Camera.SetOrthographicNear(camNear);

					float camFar = component.Camera.GetOrthographicFar();
					if (UI::DrawFloatControl("Far", camFar))
						component.Camera.SetOrthographicFar(camFar);
				});

				DrawComponent<ScriptComponent>("Script", entity, [&](ScriptComponent& component) 
				{
					if (UI::DrawStringControl("Script", component.ModuleName, ImGuiInputTextFlags_EnterReturnsTrue)) 
					{
						if (ScriptEngine::ClassExists(component.ModuleName)) 
						{
							ScriptEngine::InitializeScriptable(entity);
							component.ClassExists = true;
						}
						else
							component.ClassExists = false;
					}

					if (!component.ClassExists) 
					{
						ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 1.0f });

						ImGui::Text("The class could not be found");

						ImGui::PopStyleColor();
					}

					if (!component.PublicFields.empty()) 
					{
						for (auto& hashedName : component.FieldOrder)
						{
							ScriptField& field = component.PublicFields.at(hashedName);

							switch (field.GetType())
							{
							case ScriptFieldType::Bool:
							{
								bool value = 0;
								field.GetValue(&value);

								if (UI::DrawBoolControl(field.GetName(), value))
									field.SetValue(&value);

								break;
							}
							case ScriptFieldType::Int8:
							{
								int8_t value = 0;
								field.GetValue(&value);

								if (UI::DrawScalar(field.GetName(), ImGuiDataType_S8, &value))
									field.SetValue(&value);

								break;
							}
							case ScriptFieldType::Int16:
							{
								int16_t value = 0;
								field.GetValue(&value);

								if (UI::DrawScalar(field.GetName(), ImGuiDataType_S16, &value))
									field.SetValue(&value);

								break;
							}
							case ScriptFieldType::Int:
							{
								int32_t value = 0;
								field.GetValue(&value);

								if (UI::DrawScalar(field.GetName(), ImGuiDataType_S32, &value))
									field.SetValue(&value);

								break;
							}
							case ScriptFieldType::Int64:
							{
								int64_t value = 0;
								field.GetValue(&value);

								if (UI::DrawScalar(field.GetName(), ImGuiDataType_S64, &value))
									field.SetValue(&value);

								break;
							}
							case ScriptFieldType::UInt8:
							{
								uint8_t value = 0;
								field.GetValue(&value);

								if (UI::DrawScalar(field.GetName(), ImGuiDataType_U8, &value))
									field.SetValue(&value);

								break;
							}
							case ScriptFieldType::UInt16:
							{
								uint16_t value = 0;
								field.GetValue(&value);

								if (UI::DrawScalar(field.GetName(), ImGuiDataType_U16, &value))
									field.SetValue(&value);

								break;
							}
							case ScriptFieldType::UInt:
							{
								uint32_t value = 0;
								field.GetValue(&value);

								if (UI::DrawScalar(field.GetName(), ImGuiDataType_U32, &value))
									field.SetValue(&value);

								break;
							}
							case ScriptFieldType::UInt64:
							{
								uint64_t value = 0;
								field.GetValue(&value);

								if (UI::DrawScalar(field.GetName(), ImGuiDataType_U64, &value))
									field.SetValue(&value);

								break;
							}
							case ScriptFieldType::Float:
							{
								float value = 0.0f;
								field.GetValue(&value);

								if (UI::DrawFloatControl(field.GetName(), value, 0.1f, "%.2f"))
									field.SetValue(&value);

								break;
							}
							case ScriptFieldType::Double:
							{
								double value = 0.0f;
								field.GetValue(&value);

								if (UI::DrawScalar(field.GetName(), ImGuiDataType_Double, &value, 0.1f, "%.4f"))
									field.SetValue(&value);

								break;
							}
							case ScriptFieldType::String: 
							{
								std::string value = field.GetValue();

								if (UI::DrawStringControl(field.GetName(), value))
									field.SetValue(value.c_str());

								break;
							}
							}
						}
					}
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
					if (!entity.HasComponent<ScriptComponent>())
						if (ImGui::MenuItem("Script"))
							entity.AddComponent<ScriptComponent>();

					ImGui::EndPopup();
				}

				ImGui::SetCursorPos(cursorPos);

			}

			ImGui::End();
		}

	}
}
