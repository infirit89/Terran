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

				DrawComponent<LineRendererComponent>("Line Renderer", entity, [](LineRendererComponent& lineRenderer) 
				{
					UI::DrawColor4Control("Color", lineRenderer.Color);
					UI::DrawFloatControl("Thickness", lineRenderer.Thickness);

					int origPointCount = lineRenderer.PointCount;

					if (UI::DrawIntControl("Point Count", lineRenderer.PointCount))
					{
						// TODO: copy points to a temporary array, realocate points, copy the temporary array to points
						/*glm::vec3* tempArr = new glm::vec3[origPointCount];
						memcpy(tempArr, lineRenderer.Points, origPointCount);

						delete[] */

						if(lineRenderer.Points)
							lineRenderer.Points = (glm::vec3*)realloc(lineRenderer.Points, lineRenderer.PointCount * sizeof(glm::vec3));

						if (lineRenderer.PointCount > origPointCount)
						{
							for (size_t i = 0; i < lineRenderer.PointCount - origPointCount; i++)
								lineRenderer.Points[i + origPointCount] = { 0.0f, 0.0f, 0.0f };
						}
					}

					for (size_t i = 0; i < lineRenderer.PointCount; i++)
					{
						std::string formatedLabel = fmt::format("Point {0}", i + 1);

						UI::DrawVec3Control(formatedLabel.c_str(), lineRenderer.Points[i]);
					}

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
								bool value = field.GetData<bool>();

								if (UI::DrawBoolControl(field.GetName(), value))
									field.SetData(value);

								break;
							}
							case ScriptFieldType::Int8:
							{
								int8_t value = field.GetData<int8_t>();

								if (UI::DrawScalar(field.GetName(), ImGuiDataType_S8, &value))
									field.SetData(value);

								break;
							}
							case ScriptFieldType::Int16:
							{
								int16_t value = field.GetData<int16_t>();

								if (UI::DrawScalar(field.GetName(), ImGuiDataType_S16, &value))
									field.SetData(value);

								break;
							}
							case ScriptFieldType::Int:
							{
								int32_t value = field.GetData<int32_t>();

								if (UI::DrawScalar(field.GetName(), ImGuiDataType_S32, &value))
									field.SetData(value);

								break;
							}
							case ScriptFieldType::Int64:
							{
								int64_t value = field.GetData<int64_t>();

								if (UI::DrawScalar(field.GetName(), ImGuiDataType_S64, &value))
									field.SetData(value);

								break;
							}
							case ScriptFieldType::UInt8:
							{
								uint8_t value = field.GetData<uint8_t>();

								if (UI::DrawScalar(field.GetName(), ImGuiDataType_U8, &value))
									field.SetData(value);

								break;
							}
							case ScriptFieldType::UInt16:
							{
								uint16_t value = field.GetData<uint16_t>();

								if (UI::DrawScalar(field.GetName(), ImGuiDataType_U16, &value))
									field.SetData(value);

								break;
							}
							case ScriptFieldType::UInt:
							{
								uint32_t value = field.GetData<uint32_t>();

								if (UI::DrawScalar(field.GetName(), ImGuiDataType_U32, &value))
									field.SetData(value);

								break;
							}
							case ScriptFieldType::UInt64:
							{
								uint64_t value = field.GetData<uint64_t>();

								if (UI::DrawScalar(field.GetName(), ImGuiDataType_U64, &value))
									field.SetData(value);

								break;
							}
							case ScriptFieldType::Float:
							{
								float value = field.GetData<float>();

								if (UI::DrawFloatControl(field.GetName(), value, 0.1f, "%.2f"))
									field.SetData(value);

								break;
							}
							case ScriptFieldType::Double:
							{
								double value = field.GetData<double>();

								if (UI::DrawScalar(field.GetName(), ImGuiDataType_Double, &value, 0.1f, "%.4f"))
									field.SetData(value);

								break;
							}
							case ScriptFieldType::String: 
							{
								std::string value = field.GetData<const char*>();

								if (UI::DrawStringControl(field.GetName(), value))
									field.SetData<const char*>(value.c_str());

								break;
							}
							case ScriptFieldType::Vector2: 
							{
								glm::vec2 value = field.GetData<glm::vec2>();
								
								if (UI::DrawVec2Control(field.GetName(), value))
									field.SetData<glm::vec2>(value);

								break;
							}
							case ScriptFieldType::Vector3: 
							{
								glm::vec3 value = field.GetData<glm::vec3>();

								if (UI::DrawVec3Control(field.GetName(), value))
									field.SetData<glm::vec3>(value);

								break;
							}
							}
						}
					}
				});

				DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](Rigidbody2DComponent& rbComponent) 
				{
					UI::ScopedVarTable::TableInfo tableInfo;

					// rigidbody body type selection
					{
						const char* bodyTypeNames[] = { "Static", "Dynamic", "Kinematic" };
						const char* currentBodyType = bodyTypeNames[(int)rbComponent.BodyType];
						
						UI::ScopedVarTable bodyTypeTable("Body Type", tableInfo);

						if (ImGui::BeginCombo("##body_type", currentBodyType)) 
						{
							for (int i = 0; i < 3; i++)
							{
								const bool is_selected = (bodyTypeNames[i] == currentBodyType);
								if (ImGui::Selectable(bodyTypeNames[i], is_selected))
									rbComponent.BodyType = (RigidbodyBodyType)i;

								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}

							ImGui::EndCombo();
						}
						
					}

					UI::DrawBoolControl("Fixed Rotation", rbComponent.FixedRotation);

					// rigidbody awake state selection 
					{
						const char* awakeStateNames[] = { "Sleep", "Awake", "Never Sleep" };
						const char* currentAwakeState = awakeStateNames[(int)rbComponent.AwakeState];

						UI::ScopedVarTable awakeStateTable("Awake State", tableInfo);

						if (ImGui::BeginCombo("##awake_state", currentAwakeState))
						{
							for (int i = 0; i < 3; i++)
							{
								const bool is_selected = (awakeStateNames[i] == currentAwakeState);
								if (ImGui::Selectable(awakeStateNames[i], is_selected))
									rbComponent.AwakeState = (RigidbodyAwakeState)i;

								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}

							ImGui::EndCombo();
						}

					}

					UI::DrawFloatControl("Gravity Scale", rbComponent.GravityScale);
				});

				DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](BoxCollider2DComponent& bcComponent) 
				{
					UI::DrawVec2Control("Offset", bcComponent.Offset);
					UI::DrawVec2Control("Size", bcComponent.Size);

					UI::DrawBoolControl("Is Sensor", bcComponent.IsSensor);
				});

				DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](CircleCollider2DComponent& ccComponent) 
				{
					UI::DrawVec2Control("Offset", ccComponent.Offset);
					UI::DrawFloatControl("Radius", ccComponent.Radius);

					UI::DrawBoolControl("Is Sensor", ccComponent.IsSensor);
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

					if (!entity.HasComponent<LineRendererComponent>())
						if (ImGui::MenuItem("Line Renderer"))
							entity.AddComponent<LineRendererComponent>();

					if (!entity.HasComponent<CameraComponent>())
						if (ImGui::MenuItem("Camera"))
							entity.AddComponent<CameraComponent>();

					if (!entity.HasComponent<ScriptComponent>())
						if (ImGui::MenuItem("Script"))
							entity.AddComponent<ScriptComponent>();

					if (!entity.HasComponent<Rigidbody2DComponent>())
						if (ImGui::MenuItem("Rigidbody 2D"))
							entity.AddComponent<Rigidbody2DComponent>();

					if (!entity.HasComponent<BoxCollider2DComponent>())
						if (ImGui::MenuItem("Box Collider 2D"))
							entity.AddComponent<BoxCollider2DComponent>();

					if (!entity.HasComponent<CircleCollider2DComponent>())
						if (ImGui::MenuItem("Circle Collider 2D"))
							entity.AddComponent<CircleCollider2DComponent>();

					ImGui::EndPopup();
				}

				ImGui::SetCursorPos(cursorPos);

			}

			ImGui::End();
		}

	}
}
