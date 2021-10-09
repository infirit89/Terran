#pragma once

#include "Terran.h"

namespace TerranEngine 
{
	class SceneHierarchy 
	{
	public:
		SceneHierarchy() = default;
		SceneHierarchy(const Shared<Scene>& scene);

		void SetScene(const Shared<Scene>& scene);

		Entity& GetSelected() { return m_Selected; }

		void SetClosed(bool closed);

		void ImGuiRender();


	private:
		bool m_Closed = true;

		Entity m_Selected = {};
		Shared<Scene> m_Scene;
	};
}