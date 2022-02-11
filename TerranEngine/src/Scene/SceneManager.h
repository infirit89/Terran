#pragma once

#include "Core/Base.h"

#include "Scene.h"

namespace TerranEngine 
{
	class SceneManager 
	{
	public:
		static Shared<Scene> GetCurrentScene() { return m_CurrentScene; }
		static void SetCurrentScene(const Shared<Scene>& scene) { m_CurrentScene = scene; }

	private:
		static Shared<Scene> m_CurrentScene;
	};
}