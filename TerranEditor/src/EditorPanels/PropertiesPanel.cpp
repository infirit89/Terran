#include "PropertiesPanel.h"

#include "EditorLayer.h"
#include "Physics/Collider.h"
#include "Scene/Components.h"
#include "SelectionManager.h"

#include "Scripting/ScriptCache.h"

#include "Physics/PhysicsLayerManager.h"

#include "UI/UI.h"
#include "EditorResources.h"

#include <float.h>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_internal.h>

#include <functional>
#include <filesystem>

namespace TerranEditor 
{
	template<typename Component>
	using UIFunc = std::function<void(Component&)>;

	static TransformComponent s_TransformClipboard;

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

			if(ImGui::IsItemClicked(ImGuiMouseButton_Right))
				ImGui::OpenPopup("ComponentSettings");

			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);

			ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });
			if (ImGui::ImageButton((ImTextureID)(EditorResources::GetSettingsTexture()->GetTextureID()), 
				ImVec2{ lineHeight - 4.0f, lineHeight - 6.0f }, { 0, 1 }, { 1, 0 }))
				ImGui::OpenPopup("ComponentSettings");

			ImGui::PopStyleColor();

			ImVec4 windowBGColor = ImGui::GetStyleColorVec4(ImGuiCol_PopupBg);
			windowBGColor.w = 1.0f;

			ImGui::PushStyleColor(ImGuiCol_PopupBg, windowBGColor);
			if (ImGui::BeginPopup("ComponentSettings")) 
			{
				if (ImGui::MenuItem("Reset"))
					entity.GetComponent<T>() = T();

				if (ImGui::MenuItem("Remove Component", (const char*)0, false, removable))
					removedComponent = true;

				if (std::is_same<T, TransformComponent>())
				{
					TransformComponent& transform = (TransformComponent&)component;
					if (ImGui::MenuItem("Copy"))
						s_TransformClipboard = transform;

					if (ImGui::MenuItem("Paste"))
						transform = s_TransformClipboard;
				}

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

	void PropertiesPanel::ImGuiRender()
	{
		if(m_Open)
		{
			ImGui::Begin("Properties", &m_Open);
			DrawComponents();
			ImGui::End();
		}
	}
	void PropertiesPanel::DrawComponents()
	{
		Entity entity = SelectionManager::GetSelected();

		if (entity)
		{
			if (entity.HasComponent<TagComponent>())
			{
				auto& tagComp = entity.GetComponent<TagComponent>();
				char buf[256];
				memset(buf, 0, sizeof(buf));
				strcpy_s(buf, sizeof(buf), tagComp.Name.c_str());

				if (ImGui::InputText("##Tag", buf, sizeof(buf)) && ImGui::IsKeyPressed((ImGuiKey)Key::Enter))
					tagComp.Name = buf;
			}

			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPos().y + 2.0f);

			DrawComponent<TransformComponent>("Transform", entity, [&](TransformComponent& component)
			{
				bool isScenePlaying = EditorLayer::GetInstace()->GetSceneState() == SceneState::Play;
				Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);

				UI::BeginPropertyGroup("transform");
				ImGui::TableNextRow();

				if (UI::PropertyVec3("Position", component.Position))
				{
					component.IsDirty = true;

					if (physicsBody && isScenePlaying)
						physicsBody->SetPosition({ component.Position.x, component.Position.y });

				}

				ImGui::TableNextRow();
				if (UI::PropertyVec3("Rotation", component.Rotation))
				{
					component.IsDirty = true;

					if (physicsBody && isScenePlaying)
						physicsBody->SetRotation(component.Rotation.z);
				}

				ImGui::TableNextRow();
				if (UI::PropertyVec3("Scale", component.Scale))
					component.IsDirty = true;

				UI::EndPropertyGroup();

			}, false);

			DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent& component)
			{
				UI::BeginPropertyGroup("sprite_renderer");
				ImGui::TableNextRow();
				UI::PropertyColor("Color", component.Color);
				ImGui::TableNextRow();
				UI::PropertyAssetField<Texture>("Sprite", AssetType::Texture, component.TextureHandle);
				ImGui::TableNextRow();
				UI::PropertyInt("Z index", component.ZIndex);
				UI::EndPropertyGroup();
			});

			DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](CircleRendererComponent& component)
			{
				UI::BeginPropertyGroup("circle_renderer");
				ImGui::TableNextRow();
				UI::PropertyColor("Color", component.Color);
				ImGui::TableNextRow();
				UI::PropertyFloat("Thickness", component.Thickness);
				UI::EndPropertyGroup();
			});

			DrawComponent<LineRendererComponent>("Line Renderer", entity, [](LineRendererComponent& lineRenderer)
			{
				UI::BeginPropertyGroup("line_renderer");
				ImGui::TableNextRow();
				UI::PropertyColor("Color", lineRenderer.Color);
				ImGui::TableNextRow();
				UI::PropertyFloat("Thickness", lineRenderer.Thickness);
				ImGui::TableNextRow();
				UI::PropertyVec3("Start Point", lineRenderer.StartPoint);
				ImGui::TableNextRow();
				UI::PropertyVec3("End Point", lineRenderer.EndPoint);
				UI::EndPropertyGroup();

			});

			DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& component)
			{
				UI::BeginPropertyGroup("camera_1");
				ImGui::TableNextRow();
				UI::PropertyColor("Background", component.BackgroundColor);
				ImGui::TableNextRow();

				float camSize = component.Camera.GetOrthographicSize();
				if (UI::PropertyFloat("Size", camSize))
					component.Camera.SetOrthographicSize(camSize);

				UI::EndPropertyGroup();

				ImGui::Text("Clipping planes");

				UI::BeginPropertyGroup("camera_2");
				ImGui::TableNextRow();
				float camNear = component.Camera.GetOrthographicNear();
				if (UI::PropertyFloat("Near", camNear))
					component.Camera.SetOrthographicNear(camNear);
				ImGui::TableNextRow();

				float camFar = component.Camera.GetOrthographicFar();
				if (UI::PropertyFloat("Far", camFar))
					component.Camera.SetOrthographicFar(camFar);
				UI::EndPropertyGroup();
			});

			DrawComponent<ScriptComponent>("Script", entity, [&](ScriptComponent& component)
			{
				UI::BeginPropertyGroup("script_module");
				ImGui::TableNextRow();
				if (UI::PropertyString("Script", component.ModuleName, ImGuiInputTextFlags_EnterReturnsTrue))
					ScriptEngine::InitializeScriptable(entity);
				UI::EndPropertyGroup();

				if (!component.ClassExists)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 1.0f });
					ImGui::Text("The class could not be found");
					ImGui::PopStyleColor();
				}

				UI::BeginPropertyGroup("script_properties");
				if (!component.PublicFieldIDs.empty())
				{
					const GCHandle handle = ScriptEngine::GetScriptInstanceGCHandle(entity.GetSceneID(), entity.GetID());

					for (const auto& fieldID : component.PublicFieldIDs)
					{
						ImGui::TableNextRow();
						ScriptField* field = ScriptCache::GetCachedFieldFromID(fieldID);

						if (field->GetType().IsArray())
						{
							ScriptArray array = field->GetArray(handle);
							if (UI::PropertyScriptArrayField(m_Scene, field->GetName(), array))
								field->SetArray(array, handle);
						}
						else
							UI::PropertyScriptField(m_Scene, field, handle);


					}
				}
				UI::EndPropertyGroup();

			});

			DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [&](Rigidbody2DComponent& rbComponent)
			{
				bool isScenePlaying = EditorLayer::GetInstace()->GetSceneState() == SceneState::Play;

				const char* bodyTypeNames[] = { "Static", "Dynamic", "Kinematic" };
				const char* sleepStateNames[] = { "Sleep", "Awake", "Never Sleep" };

				Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);

				UI::ScopedVarTable::TableInfo tableInfo;

				// rigidbody body type selection
				if (UI::PropertyComboBox("Body Type", bodyTypeNames, 3, rbComponent.BodyType) &&
					isScenePlaying)
					physicsBody->SetBodyType(rbComponent.BodyType);

				if (UI::PropertyAssetField<PhysicsMaterial2DAsset>("Material", AssetType::PhysicsMaterial2D, rbComponent.PhysicsMaterialHandle))
				{
					// TODO: change in scene runtime
				}

				UI::PropertyBool("Fixed Rotation", rbComponent.FixedRotation);
				UI::PropertyBool("Enabled", rbComponent.Enabled);

				// rigidbody awake state selection 
				UI::PropertyComboBox("Sleep State", sleepStateNames, 3, rbComponent.SleepState);

				// rigidbody layer selection
				std::vector<const char*> layerNames = PhysicsLayerManager::GetLayerNames();
				UI::PropertyComboBox("Layer", layerNames.data(), layerNames.size(), rbComponent.LayerIndex);

				if (rbComponent.BodyType != PhysicsBodyType::Static)
				{
					if (UI::PropertyFloat("Gravity Scale", rbComponent.GravityScale))
					{
						if (isScenePlaying)
							physicsBody->SetGravityScale(rbComponent.GravityScale);
					}
				}

				{
					ImGui::Unindent(20.0f);

					UI::ScopedStyleColor stlyeColor({
						{ ImGuiCol_HeaderHovered,	{ 1.0f, 0.0f, 0.0f, 0.0f } },
						{ ImGuiCol_HeaderActive,	{ 1.0f, 0.0f, 0.0f, 0.0f } }
						});
					bool treeNode = ImGui::TreeNodeEx("Details", ImGuiTreeNodeFlags_None);

					if (treeNode)
					{
						{
							UI::ScopedVarTable currentSleepStateTable("Sleep State", tableInfo);

							int sleepStateNamesIndex = isScenePlaying ? (int)physicsBody->GetSleepState() : (int)PhysicsBodySleepState::Awake;
							ImGui::Text(sleepStateNames[sleepStateNamesIndex]);
						}

						{
							UI::ScopedVarTable currentLinearVelocityTable("Linear Velocity", tableInfo);

							glm::vec2 velocity = isScenePlaying ? physicsBody->GetLinearVelocity() : glm::vec2(0.0f, 0.0f);
							std::string velocityText = fmt::format("X: {0}, Y: {1}", velocity.x, velocity.y);
							ImGui::Text(velocityText.c_str());
						}

						ImGui::TreePop();
					}
					ImGui::Indent(20.0f);
				}
			});

			DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [&](BoxCollider2DComponent& bcComponent)
				{
					bool isRuntime = EditorLayer::GetInstace()->GetSceneState() == SceneState::Play;

			Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);

			Shared<BoxCollider2D> boxCollider;

			if (isRuntime)
				boxCollider = DynamicCast<BoxCollider2D>(physicsBody->GetColliders()[bcComponent.ColliderIndex]);

			if (UI::PropertyVec2("Offset", bcComponent.Offset))
			{
				if (isRuntime && boxCollider)
					boxCollider->SetOffset(bcComponent.Offset);
			}

			if (UI::PropertyVec2("Size", bcComponent.Size))
			{
				bcComponent.Size = glm::max({ 0.0f, 0.0f }, bcComponent.Size);

				if (isRuntime && boxCollider)
					boxCollider->SetSize(bcComponent.Size);
			}

			if (bcComponent.Size.x <= FLT_EPSILON || bcComponent.Size.y <= FLT_EPSILON)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 1.0f });
				ImGui::TextWrapped("The collider did not create any collision shapes. This is because the size was too small.");
				ImGui::PopStyleColor();
			}

			if (UI::PropertyBool("Is Sensor", bcComponent.IsSensor))
			{
				if (isRuntime && boxCollider)
					boxCollider->SetSensor(bcComponent.IsSensor);
			}
				});

			DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [&](CircleCollider2DComponent& ccComponent)
				{
					bool isRuntime = EditorLayer::GetInstace()->GetSceneState() == SceneState::Play;
			Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
			Shared<CircleCollider2D> circleCollider;

			if (isRuntime)
				circleCollider = DynamicCast<CircleCollider2D>(physicsBody->GetColliders()[ccComponent.ColliderIndex]);

			if (UI::PropertyVec2("Offset", ccComponent.Offset))
			{
				if (isRuntime)
					circleCollider->SetOffset(ccComponent.Offset);
			}

			if (UI::PropertyFloat("Radius", ccComponent.Radius))
			{
				ccComponent.Radius = glm::max(0.0f, ccComponent.Radius);

				if (isRuntime)
					circleCollider->SetRadius(ccComponent.Radius);
			}

			if (ccComponent.Radius <= FLT_EPSILON)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 1.0f });
				ImGui::TextWrapped("The collider did not create any collision shapes. This is because the size was too small.");
				ImGui::PopStyleColor();
			}

			if (UI::PropertyBool("Is Sensor", ccComponent.IsSensor))
			{
				if (isRuntime)
					circleCollider->SetSensor(ccComponent.IsSensor);
			}
				});

			DrawComponent<CapsuleCollider2DComponent>("Capsule Collider 2D", entity, [&](CapsuleCollider2DComponent& ccComponent)
				{
					bool isRuntime = EditorLayer::GetInstace()->GetSceneState() == SceneState::Play;
			Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
			Shared<CapsuleCollider2D> capsuleCollider;

			if (isRuntime && physicsBody)
				capsuleCollider = DynamicCast<CapsuleCollider2D>(physicsBody->GetColliders()[ccComponent.ColliderIndex]);

			if (UI::PropertyVec2("Offset", ccComponent.Offset))
			{
				if (capsuleCollider)
					capsuleCollider->SetOffset(ccComponent.Offset);
			}

			if (UI::PropertyVec2("Size", ccComponent.Size))
			{
				ccComponent.Size = glm::max({ 0.0f, 0.0f }, ccComponent.Size);

				if (capsuleCollider)
					capsuleCollider->SetSize(ccComponent.Size);
			}

			if (ccComponent.Size.x <= FLT_EPSILON || ccComponent.Size.y <= FLT_EPSILON)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 1.0f });
				ImGui::TextWrapped("The collider did not create any collision shapes. This is because the size was too small.");
				ImGui::PopStyleColor();
			}

			if (UI::PropertyBool("Is Sensor", ccComponent.IsSensor))
			{
				if (capsuleCollider)
					capsuleCollider->SetSensor(ccComponent.IsSensor);
			}
				});

			DrawComponent<TextRendererComponent>("Text Renderer", entity, [](TextRendererComponent& textRenderer)
				{
					{
						UI::ScopedVarTable textScopedVarTable("Text", {});

						char buf[256];
						memset(buf, 0, sizeof(buf));
						strcpy_s(buf, textRenderer.Text.c_str());

						if (ImGui::InputTextMultiline("##text", buf, sizeof(buf), { 0.0f, 0.0f }, ImGuiInputTextFlags_AllowTabInput))
							textRenderer.Text = buf;
					}

			// TODO: make it changeable
			if (!textRenderer.FontAtlas)
				textRenderer.FontAtlas = CreateShared<FontAtlas>("Resources/Fonts/Roboto/Roboto-Regular.ttf");

			UI::PropertyColor("Text Color", textRenderer.TextColor);

			UI::PropertyFloat("Line Spacing", textRenderer.LineSpacing);
			UI::PropertyFloat("Line Width", textRenderer.LineWidth);
				});

			ImVec2 cursorPos = ImGui::GetCursorPos();
			ImGui::SetCursorPos(ImVec2{ cursorPos.x + ImGui::GetContentRegionAvail().x / 4.0f, cursorPos.y += 5.0f });

			float buttonWidth = (ImGui::GetContentRegionAvail().x / 2.0f) + GImGui->Font->FontSize + 5.0f;
			if (ImGui::Button("Add Component", ImVec2{ buttonWidth, 0 }))
				ImGui::OpenPopup("AddComponent");

			if (ImGui::BeginPopup("AddComponent"))
			{
				if (!entity.HasComponent<SpriteRendererComponent>())
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

				if (!entity.HasComponent<CapsuleCollider2DComponent>())
					if (ImGui::MenuItem("Capsule Collider 2D"))
						entity.AddComponent<CapsuleCollider2DComponent>();

				if (!entity.HasComponent<TextRendererComponent>())
					if (ImGui::MenuItem("Text Renderer"))
						entity.AddComponent<TextRendererComponent>();

				ImGui::EndPopup();
			}

			ImGui::SetCursorPos(cursorPos);
		}
	}
}
