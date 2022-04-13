#pragma once

#include "Terran.h"

namespace TerranEditor 
{
	using namespace TerranEngine;
	class SceneHierarchy 
	{
	public:
		SceneHierarchy() = default;
		SceneHierarchy(const Shared<Scene>& scene);

		~SceneHierarchy() = default;

		void SetScene(const Shared<Scene>& scene);

		Entity GetSelected() { return m_Scene->FindEntityWithUUID(m_SelectedID); }
		void SetSelected(Entity selectedEntity) 
		{
			if(selectedEntity)
				m_SelectedID = selectedEntity.GetID(); 
		}

		void SetOpen(bool open) { m_Open = open; }

		void OnEvent(Event& event);

		void ImGuiRender();

	private:
		bool OnKeyPressed(KeyPressedEvent& e);

		void DrawEntityNode(Entity entity);

		bool m_Open = true;

		UUID m_SelectedID;
		Shared<Scene> m_Scene;
	};
}