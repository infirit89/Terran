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

		Entity& GetSelected() { return m_Selected; }

		void SetOpen(bool open) { m_Open = open; }
		 
		void ImGuiRender();

	private:
		void DrawEntityNode(Entity entity);

		bool m_Open = true;

		Entity m_Selected = {};
		Shared<Scene> m_Scene;
	};
}