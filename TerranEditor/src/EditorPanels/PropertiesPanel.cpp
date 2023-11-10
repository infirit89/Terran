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
	static Shared<Scene> s_CopyScene;
	static Entity s_CopyEntity;

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
			if (ImGui::ImageButton(reinterpret_cast<ImTextureID>((uint64_t)EditorResources::SettingsTexture->GetHandle()), 
				ImVec2{ lineHeight - 4.0f, lineHeight - 4.0f }, { 0, 1 }, { 1, 0 }))
				ImGui::OpenPopup("ComponentSettings");

			ImGui::PopStyleColor();

			ImVec4 windowBGColor = ImGui::GetStyleColorVec4(ImGuiCol_PopupBg);
			windowBGColor.w = 1.0f;

			ImGui::PushStyleColor(ImGuiCol_PopupBg, windowBGColor);
			if (ImGui::BeginPopup("ComponentSettings")) 
			{
				if (ImGui::MenuItem("Reset"))
					entity.GetComponent<T>() = T();

				if (ImGui::MenuItem("Remove Component", nullptr, false, removable))
					removedComponent = true;

				if (ImGui::MenuItem("Copy"))
					s_CopyEntity.AddOrReplaceComponent<T>(component);

				if (ImGui::MenuItem("Paste", nullptr, false, s_CopyEntity.HasComponent<T>()))
					component = s_CopyEntity.GetComponent<T>();

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

	PropertiesPanel::PropertiesPanel()
	{
		s_CopyScene = CreateShared<Scene>();
		s_CopyEntity = s_CopyScene->CreateEmptyEntity();
	}

	void PropertiesPanel::OnRender()
	{
		if(m_Open)
		{
			ImGui::Begin(GetName(), &m_Open);
			DrawComponents();
			ImGui::End();
		}
	}

	template<typename ComponentType>
	static void DrawAddComponentButton(Entity entity, const char* label) 
	{
		if (!entity.HasComponent<ComponentType>())
			if (ImGui::MenuItem(label))
				entity.AddComponent<ComponentType>();
	}

	void PropertiesPanel::DrawComponents()
	{
		// TODO: add multiselect functionality
		const std::vector<UUID>& selected = SelectionManager::GetSelected(SelectionContext::Scene);
		if (selected.empty())
			return;

		Entity entity = m_Scene->FindEntityWithUUID(selected[0]);

		if (!entity)
			return;

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
			UI::PropertyAssetField<Texture2D>("Sprite", AssetType::Texture2D, component.TextureHandle);

			Shared<Texture2D> texture = 
							AssetManager::GetAsset<Texture2D>(component.TextureHandle);
			if (texture) 
			{
				ImGui::TableNextRow();
				constexpr size_t textureFilterNamesSize = 2;
				const char* textureFilterNames[textureFilterNamesSize] = 
				{ 
					"Linear",
					"Nearest" 
				};

				TextureFilter filter = texture->GetTextureParameters().Filter;
				if (UI::PropertyDropdown("Filter", textureFilterNames, 
										textureFilterNamesSize, filter)) 
				{
					texture->SetTextureFilter(filter);
				}
			}

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

			UI::BeginPropertyGroup("clipping_planes");
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

			UI::BeginPropertyGroup("rigidbody_properties");
			// rigidbody body type selection
			ImGui::TableNextRow();
			if (UI::PropertyDropdown("Body Type", bodyTypeNames, 3, rbComponent.BodyType) &&
				isScenePlaying)
				physicsBody->SetBodyType(rbComponent.BodyType);

			ImGui::TableNextRow();
			if (UI::PropertyAssetField<PhysicsMaterial2DAsset>("Material", AssetType::PhysicsMaterial2D, rbComponent.PhysicsMaterialHandle))
			{
				// TODO: change in scene runtime
			}

			ImGui::TableNextRow();
			UI::PropertyBool("Fixed Rotation", rbComponent.FixedRotation);

			ImGui::TableNextRow();
			UI::PropertyBool("Enabled", rbComponent.Enabled);

			ImGui::TableNextRow();
			// rigidbody awake state selection 
			UI::PropertyDropdown("Sleep State", sleepStateNames, 3, rbComponent.SleepState);

			// rigidbody layer selection
			std::vector<const char*> layerNames = PhysicsLayerManager::GetLayerNames();
			ImGui::TableNextRow();
			UI::PropertyDropdown("Layer", layerNames.data(), layerNames.size(), rbComponent.LayerIndex);

			if (rbComponent.BodyType != PhysicsBodyType::Static)
			{
				ImGui::TableNextRow();
				if (UI::PropertyFloat("Gravity Scale", rbComponent.GravityScale))
				{
					if (isScenePlaying)
						physicsBody->SetGravityScale(rbComponent.GravityScale);
				}
			}
			UI::EndPropertyGroup();

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

			
		auto displayColliderError = []() 
		{
			ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 1.0f });
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TextWrapped("The collider did not create any collision shapes. This is because the size was too small.");
			ImGui::PopStyleColor();
		};

		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [&](BoxCollider2DComponent& bcComponent)
		{
			bool isRuntime = EditorLayer::GetInstace()->GetSceneState() == SceneState::Play;
			Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
			Shared<BoxCollider2D> boxCollider;

			if (isRuntime)
				boxCollider = DynamicCast<BoxCollider2D>(physicsBody->GetColliders()[bcComponent.ColliderIndex]);

			UI::BeginPropertyGroup("box_collider");

			ImGui::TableNextRow();
			if (UI::PropertyVec2("Offset", bcComponent.Offset))
			{
				if (isRuntime && boxCollider)
					boxCollider->SetOffset(bcComponent.Offset);
			}

			ImGui::TableNextRow();
			if (UI::PropertyVec2("Size", bcComponent.Size))
			{
				bcComponent.Size = glm::max({ 0.0f, 0.0f }, bcComponent.Size);

				if (isRuntime && boxCollider)
					boxCollider->SetSize(bcComponent.Size);
			}

			if (bcComponent.Size.x <= FLT_EPSILON || bcComponent.Size.y <= FLT_EPSILON)
				displayColliderError();

			ImGui::TableNextRow();
			if (UI::PropertyBool("Is Sensor", bcComponent.Sensor))
			{
				if (isRuntime && boxCollider)
					boxCollider->SetSensor(bcComponent.Sensor);
			}
			UI::EndPropertyGroup();
		});

		DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [&](CircleCollider2DComponent& ccComponent)
		{
			bool isRuntime = EditorLayer::GetInstace()->GetSceneState() == SceneState::Play;
			Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
			Shared<CircleCollider2D> circleCollider;

			if (isRuntime)
				circleCollider = DynamicCast<CircleCollider2D>(physicsBody->GetColliders()[ccComponent.ColliderIndex]);

			UI::BeginPropertyGroup("circle_collider");

			ImGui::TableNextRow();
			if (UI::PropertyVec2("Offset", ccComponent.Offset))
			{
				if (isRuntime)
					circleCollider->SetOffset(ccComponent.Offset);
			}

			ImGui::TableNextRow();
			if (UI::PropertyFloat("Radius", ccComponent.Radius))
			{
				ccComponent.Radius = glm::max(0.0f, ccComponent.Radius);

				if (isRuntime)
					circleCollider->SetRadius(ccComponent.Radius);
			}

			if (ccComponent.Radius <= FLT_EPSILON)
				displayColliderError();

			ImGui::TableNextRow();
			if (UI::PropertyBool("Is Sensor", ccComponent.Sensor))
			{
				if (isRuntime)
					circleCollider->SetSensor(ccComponent.Sensor);
			}
			UI::EndPropertyGroup();
		});

		DrawComponent<CapsuleCollider2DComponent>("Capsule Collider 2D", entity, [&](CapsuleCollider2DComponent& ccComponent)
		{
			bool isRuntime = EditorLayer::GetInstace()->GetSceneState() == SceneState::Play;
			Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
			Shared<CapsuleCollider2D> capsuleCollider;

			if (isRuntime && physicsBody)
				capsuleCollider = DynamicCast<CapsuleCollider2D>(physicsBody->GetColliders()[ccComponent.ColliderIndex]);

			UI::BeginPropertyGroup("capsule_collider");

			ImGui::TableNextRow();
			if (UI::PropertyVec2("Offset", ccComponent.Offset))
			{
				if (capsuleCollider)
					capsuleCollider->SetOffset(ccComponent.Offset);
			}

			ImGui::TableNextRow();
			if (UI::PropertyVec2("Size", ccComponent.Size))
			{
				ccComponent.Size = glm::max({ 0.0f, 0.0f }, ccComponent.Size);

				if (capsuleCollider)
					capsuleCollider->SetSize(ccComponent.Size);
			}

			if (ccComponent.Size.x <= FLT_EPSILON || ccComponent.Size.y <= FLT_EPSILON)
				displayColliderError();

			ImGui::TableNextRow();
			if (UI::PropertyBool("Is Sensor", ccComponent.Sensor))
			{
				if (capsuleCollider)
					capsuleCollider->SetSensor(ccComponent.Sensor);
			}

			UI::EndPropertyGroup();
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
				textRenderer.FontAtlas = CreateShared<Font>("Resources/Fonts/Roboto/Roboto-Regular.ttf");

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
			DrawAddComponentButton<SpriteRendererComponent>(entity, "Sprite Renderer");
			DrawAddComponentButton<CircleRendererComponent>(entity, "Circle Renderer");
			DrawAddComponentButton<LineRendererComponent>(entity, "Line Renderer");
			DrawAddComponentButton<TextRendererComponent>(entity, "Text Renderer");
			DrawAddComponentButton<CameraComponent>(entity, "Camera");
			DrawAddComponentButton<ScriptComponent>(entity, "Script");
			DrawAddComponentButton<Rigidbody2DComponent>(entity, "Rigidbody 2D");
			DrawAddComponentButton<BoxCollider2DComponent>(entity, "Box Collider 2D");
			DrawAddComponentButton<CircleCollider2DComponent>(entity, "Circle Collider 2D");
			DrawAddComponentButton<CapsuleCollider2DComponent>(entity, "Capsule Collider 2D");

			ImGui::EndPopup();
		}

		ImGui::SetCursorPos(cursorPos);
	}
}
