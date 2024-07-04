#pragma once

#include "Scene.h"
#include "Core/Base.h"

#include <unordered_map>

namespace TerranEngine 
{
	class SceneManager 
	{
	public:
		static Shared<Scene> CreateEmpyScene();
		static void RemoveScene(const UUID& id);

		static Shared<Scene> GetScene(const UUID& id);

		static const Shared<Scene>& GetCurrentScene() { return s_CurrentScene; }
		static void SetCurrentScene(Shared<Scene> newScene);

		static std::unordered_map<UUID, Shared<Scene>>& GetActiveScenes() { return s_ActiveScenes; }

	private:
		static std::unordered_map<UUID, Shared<Scene>> s_ActiveScenes;
		static Shared<Scene> s_CurrentScene;
	};
}
