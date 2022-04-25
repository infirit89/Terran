#pragma once

#include "Terran.h"

#include <functional>

namespace TerranEditor 
{
	using namespace TerranEngine;
	class SceneHierarchyPanel 
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Shared<Scene>& scene);

		~SceneHierarchyPanel() = default;

		void SetScene(const Shared<Scene>& scene);

		void SetSelected(Entity selectedEntity) 
		{
			if (selectedEntity) 
			{
				m_SelectedID = selectedEntity.GetID(); 
				m_OnSelectedChangedCallback(selectedEntity);
			}
		}

		void SetOpen(bool open) { m_Open = open; }

		void OnEvent(Event& event);

		void ImGuiRender();

		void SetOnSelectedChangedCallback(std::function<void(Entity)> callback) { m_OnSelectedChangedCallback = callback; }

	private:
		void SetSelectedID(UUID id) 
		{
			m_SelectedID = id;
			m_OnSelectedChangedCallback(GetSelected());
		}

		Entity GetSelected() { return m_Scene->FindEntityWithUUID(m_SelectedID); }
		bool OnKeyPressed(KeyPressedEvent& e);

		void DrawEntityNode(Entity entity);

		bool m_Open = true;

		UUID m_SelectedID;
		Shared<Scene> m_Scene;

		std::function<void(Entity)> m_OnSelectedChangedCallback;
	};
}