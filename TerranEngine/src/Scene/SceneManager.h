#pragma once

#include "Scene.h"
#include "Core/Base.h"

#include <unordered_map>

namespace TerranEngine 
{
	class SceneManager 
	{
	public:
		// Declaration: void OnSceneTransition(const Shared<Scene>& oldScene, const Shared<Scene>& newScene);
		using OnSceneTransitionFn = std::function<void(const Shared<Scene>&, const Shared<Scene>&)>;

		static void SetOnSceneTransition(const OnSceneTransitionFn& sceneTransitionFn);

		static Shared<Scene> CreateEmpyScene();
		static void RemoveScene(const UUID& id);

		static Shared<Scene> GetScene(const UUID& id);

		static const Shared<Scene>& GetCurrentScene() { return s_CurrentScene; }
		static void SetCurrentScene(const Shared<Scene>& newScene);

		static std::unordered_map<UUID, Shared<Scene>>& GetActiveScenes() { return s_ActiveScenes; }

	private:
		static std::unordered_map<UUID, Shared<Scene>> s_ActiveScenes;
		static Shared<Scene> s_CurrentScene;
		static OnSceneTransitionFn s_SceneTransitionFn;
	};
}
