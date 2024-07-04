#pragma once

#include "Event.h"

#include "Core/Base.h"

#include "Scene/Scene.h"

namespace TerranEngine
{
	class SceneTransitionEvent : public Event 
	{
	public:
		SceneTransitionEvent(Weak<Scene> oldScene, Shared<Scene> newScene)
			: m_OldScene(oldScene), m_NewScene(newScene) { }

		EVENT_CLASS_TYPE(SceneTransitionEvent)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

		Weak<Scene> GetOldScene() { return m_OldScene; }
		Shared<Scene> GetNewScene() { return m_NewScene; }
	private:
		Weak<Scene> m_OldScene;
		Shared<Scene> m_NewScene;
	};
}
