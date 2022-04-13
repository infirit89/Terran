#include "SceneHierarchy.h"

#include <imgui.h>

namespace TerranEditor 
{
	SceneHierarchy::SceneHierarchy(const Shared<Scene>& scene)
	{
		SetScene(scene);
	}

	void SceneHierarchy::SetScene(const Shared<Scene>& scene)
	{
		m_Scene = scene;
		m_SelectedID = UUID({ 0 });
	}

	void SceneHierarchy::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(TR_EVENT_BIND_FN(SceneHierarchy::OnKeyPressed));
	}

	void SceneHierarchy::ImGuiRender()
	{
		if(m_Open)
		{
			ImGui::Begin("Hierarchy", &m_Open, ImGuiWindowFlags_NoCollapse);
			
			if (m_Scene) 
			{
				auto view = m_Scene->GetEntitiesWith<TagComponent>();

				for (auto e : view)
				{
					Entity entity(e, m_Scene.get());

					if (!entity.HasParent())
						DrawEntityNode(entity);
				}

				if (ImGui::BeginPopupContextWindow(0, 1, false))
				{
					if (ImGui::MenuItem("Create an entity"))
						m_SelectedID = m_Scene->CreateEntity("Entity").GetID();

					ImGui::EndPopup();
				}
	
				if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
					m_SelectedID = UUID({ 0 });
			}

			ImGui::End();
		}

	}

	bool SceneHierarchy::OnKeyPressed(KeyPressedEvent& e)
	{
		bool ctrlPressed = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shiftPressed = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		Entity selectedEntity = GetSelected();

		if (e.GetRepeatCount() > 0)
			return false;

		switch (e.GetKeyCode())
		{
		case Key::D:
			if (ctrlPressed) 
			{
				if (selectedEntity)
					m_Scene->DuplicateEntity(selectedEntity);
			}

			break;

		case Key::N:
			if (ctrlPressed && shiftPressed)
			{
				auto entity = m_Scene->CreateEntity();
				m_SelectedID = entity.GetID();

				return true;
			}

			break;
		}
		return false;
	}

	void SceneHierarchy::DrawEntityNode(Entity entity)
	{
		TagComponent& tagComp = entity.GetComponent<TagComponent>();
		std::string imguiID = fmt::format("{0} {1}", tagComp.Name, (uint32_t)entity);
		
		ImGui::PushID(imguiID.c_str());

		Entity selectedEntity = GetSelected();
		ImGuiTreeNodeFlags flags = (selectedEntity == entity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

		if (!entity.HasComponent<RelationshipComponent>() || entity.GetComponent<RelationshipComponent>().Children.size() <= 0)
			flags |= ImGuiTreeNodeFlags_NoButton;
		else
			flags |= ImGuiTreeNodeFlags_DefaultOpen;

		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiStyle orgStyle = style;

		style.FramePadding.y = 1.5f;

		bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, flags, tagComp.Name.c_str());
		
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
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_UUID")) 
			{
				TR_ASSERT(payload->DataSize == 16 * sizeof(uint8_t), "The Drag/Drop Payload data's size doesn't match the required size");

				std::array<uint8_t, 16> idArr;
				memcpy(idArr._Elems, payload->Data, 16 * sizeof(uint8_t));
				UUID id(idArr);
				Entity receivedEntity = m_Scene->FindEntityWithUUID(id);

				if (receivedEntity)
				{
					if (!receivedEntity.IsChildOf(entity)) 
					{
						if (receivedEntity.HasParent())
							receivedEntity.Unparent();

						receivedEntity.SetParent(entity);
					}
				}
			}

			ImGui::EndDragDropTarget();
		}

		style = orgStyle;

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right))
			m_SelectedID = entity.GetID();

		bool isDeleted = false;
		
		if (Input::IsKeyPressed(Key::Delete))
			if (selectedEntity == entity)
				isDeleted = true;

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Create an Entity")) 
			{
				auto entity = m_Scene->CreateEntity("Entity");
			
				if (selectedEntity)
					entity.SetParent(selectedEntity);
			}

			if (ImGui::MenuItem("Delete entity")) 
				isDeleted = true;

			if (ImGui::MenuItem("Duplicate")) 
			{
				if(selectedEntity)
					m_Scene->DuplicateEntity(selectedEntity);
			}

			ImGui::EndPopup();
		}

		if (opened)
		{
			if (entity.HasComponent<RelationshipComponent>())
			{
				for (auto eID : entity.GetChildren())
				{
					auto currEntity = m_Scene->FindEntityWithUUID(eID);

					if (currEntity)
						DrawEntityNode(currEntity);
					else
						TR_ERROR("Not a valid child");
				}
			}
			
			ImGui::TreePop();
		}

		if (isDeleted)
		{
			m_Scene->DestroyEntity(entity, true);
			if (selectedEntity == entity)
				m_SelectedID = UUID({ 0 });
		}

		ImGui::PopID();
	}
}
