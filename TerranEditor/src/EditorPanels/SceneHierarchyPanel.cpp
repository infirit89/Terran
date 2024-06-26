#include "SceneHierarchyPanel.h"

#include "SelectionManager.h"
#include "EditorLayer.h"

#include "UI/UI.h"

#include "Core/Input.h"
#include "Utils/Debug/OptickProfiler.h"

#include <imgui.h>
#include <imgui_internal.h>

#pragma warning(push)
#pragma warning(disable : 4312)

namespace TerranEditor 
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Shared<Scene>& scene)
	{
		SetSceneContext(scene);
	}

	void SceneHierarchyPanel::SetSceneContext(const Shared<Scene>& scene) 
	{
		m_Scene = scene;
	}

	void SceneHierarchyPanel::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(TR_EVENT_BIND_FN(SceneHierarchyPanel::OnKeyPressed));
	}

	void SceneHierarchyPanel::OnRender()
	{
		TR_PROFILE_FUNCTION();
		if(m_Open)
		{
			ImGui::Begin(GetName().data(), &m_Open, ImGuiWindowFlags_NoCollapse);
			
			if (ImGui::BeginChild("##sc_topbar", {0.0f, 30.0f}))
			{
				if (UI::SearchInput(m_Filter, "Search..."))
					m_Filter.Build();

				ImGui::EndChild();
			}

			ImGui::Separator();

			if (m_Scene)
				DrawScene();

			ImGui::End();
		}

	}

	void SceneHierarchyPanel::DrawScene()
	{
		TR_PROFILE_FUNCTION();
		auto entities = m_Scene->Filter<TagComponent>([this](const TagComponent tag)
		{
			return m_Filter.PassFilter(tag.Name.c_str());
		});

		for (auto entity : entities)
		{
			if (!entity.HasParent())
				DrawEntityNode(entity);
		}

		if (UI::BeginPopupContextWindow("entity_create_popup", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverExistingPopup))
		{
			if (ImGui::MenuItem("Create an entity")) 
			{
				SelectionManager::DeselectAll(SelectionContext::Scene);
				SelectionManager::Select(m_Scene->CreateEntity("Entity"));
			}

			ImGui::EndPopup();
		}

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
			SelectionManager::DeselectAll(SelectionContext::Scene);

		ImGuiWindow* currentWindow = ImGui::GetCurrentWindow();

		if (ImGui::BeginDragDropTargetCustom(currentWindow->Rect(), currentWindow->ID))
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_UUID", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
			{
				TR_ASSERT(payload->DataSize == 16 * sizeof(uint8_t), "The Drag/Drop Payload data's size doesn't match the required size");

				std::array<uint8_t, 16> idArr;
				memcpy(idArr._Elems, payload->Data, 16 * sizeof(uint8_t));
				UUID id(idArr);
				Entity receivedEntity = m_Scene->FindEntityWithUUID(id);
				receivedEntity.Unparent();
			}

			ImGui::EndDragDropTarget();
		}
	}

	bool SceneHierarchyPanel::OnKeyPressed(KeyPressedEvent& e)
	{
		bool ctrlPressed = Input::IsKeyDown(Key::LeftControl) || Input::IsKeyDown(Key::RightControl);
		bool shiftPressed = Input::IsKeyDown(Key::LeftShift) || Input::IsKeyDown(Key::RightShift);

		const auto& selected = SelectionManager::GetSelected(SelectionContext::Scene);

		Entity selectedEntity;
		if(!selected.empty())
			selectedEntity = m_Scene->FindEntityWithUUID(selected[0]);

		if (e.GetRepeatCount() > 0)
			return false;

		switch (e.GetKeyCode())
		{
		case Key::D:
			if (ctrlPressed) 
			{
				if (selectedEntity)
					SelectionManager::Select(m_Scene->DuplicateEntity(selectedEntity));
			}

			break;

		case Key::N:
			if (ctrlPressed && shiftPressed)
			{
				auto entity = m_Scene->CreateEntity();
				SelectionManager::DeselectAll(SelectionContext::Scene);
				SelectionManager::Select(entity);
				return true;
			}

			break;
		}
		return false;
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		TR_PROFILE_FUNCTION();
		TagComponent& tagComp = entity.GetComponent<TagComponent>();
		std::string imguiID = fmt::format("{0} {1}", tagComp.Name, (uint32_t)entity);

		ImGui::PushID(imguiID.c_str());

		bool isSelected = SelectionManager::IsSelected(SelectionContext::Scene, entity.GetID());
		ImGuiTreeNodeFlags flags = (isSelected ? ImGuiTreeNodeFlags_Selected : 0) |
									ImGuiTreeNodeFlags_OpenOnArrow |
									ImGuiTreeNodeFlags_SpanAvailWidth |
									ImGuiTreeNodeFlags_FramePadding |
									ImGuiTreeNodeFlags_AllowItemOverlap;

		if (!entity.HasComponent<RelationshipComponent>() || entity.GetComponent<RelationshipComponent>().Children.size() <= 0)
			flags |= ImGuiTreeNodeFlags_Leaf;
		else
			flags |= ImGuiTreeNodeFlags_DefaultOpen;

		bool opened = false;
		{
			float framePaddingX = ImGui::GetStyle().FramePadding.x;
			UI::ScopedStyleVar framePadding({
				{ ImGuiStyleVar_FramePadding, { framePaddingX, 1.5f } }
			});
			uint32_t entityID = entity;
			opened = ImGui::TreeNodeEx((void*)entityID, flags, tagComp.Name.c_str());

			if (ImGui::BeginDragDropSource())
			{
				const UUID& id = entity.GetID();
				ImGui::SetDragDropPayload("ENTITY_UUID", id.GetRaw(), 16 * sizeof(uint8_t));
				const char* entityName = entity.GetName().c_str();
				ImGui::Text(entityName);
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_UUID", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
				{
					TR_ASSERT(payload->DataSize == 16 * sizeof(uint8_t), "The Drag/Drop Payload data's size doesn't match the required size");

					std::array<uint8_t, 16> idArr;
					memcpy(idArr._Elems, payload->Data, 16 * sizeof(uint8_t));
					UUID id(idArr);
					Entity receivedEntity = m_Scene->FindEntityWithUUID(id);

					if (receivedEntity)
						receivedEntity.SetParent(entity);
				}

				ImGui::EndDragDropTarget();
			}
		}

		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && Input::IsKeyDown(Key::LeftControl))
			{
				SelectionManager::Select(entity);
			}
			else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Right))
			{
				SelectionManager::DeselectAll(SelectionContext::Scene);
				SelectionManager::Select(entity);
			}
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			auto& transformComponent = entity.GetTransform();
			EditorLayer::GetInstace()->GetEditorCamera().SetFocalPoint(transformComponent.Position);
		}

		bool isDeleted = false;

		if (Input::IsKeyDown(Key::Delete) && ImGui::IsItemFocused())
				isDeleted = true;

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Create an Entity"))
			{
				auto child = m_Scene->CreateEntity("Entity");
				child.SetParent(entity);
			}

			if (ImGui::MenuItem("Delete entity"))
				isDeleted = true;

			if (ImGui::MenuItem("Duplicate"))
				m_Scene->DuplicateEntity(entity);

			ImGui::EndPopup();
		}

		if (opened)
		{
			if (entity.HasComponent<RelationshipComponent>())
			{
				for (auto eID : entity.GetChildren())
				{
					auto currEntity = m_Scene->FindEntityWithUUID(eID);

					TR_ASSERT(currEntity, "Not a valid child");

					DrawEntityNode(currEntity);
					/*if (currEntity)
						DrawEntityNode(currEntity);
					else
						TR_ERROR("Not a valid child");*/
				}
			}

			ImGui::TreePop();
		}

		if (isDeleted)
		{
			if (isSelected)
				SelectionManager::Deselect(SelectionContext::Scene, entity.GetID());

			m_Scene->DestroyEntity(entity, true);
		}

		ImGui::PopID();
	}
}
#pragma warning(pop)
