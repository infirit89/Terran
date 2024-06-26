#pragma once

#include "Core/Base.h"

#include "Events/Event.h"
#include "Events/KeyboardEvent.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"

#include "EditorPanel.h"

#include <functional>
#include <imgui.h>

namespace TerranEditor 
{
	using namespace TerranEngine;
	class SceneHierarchyPanel : public EditorPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Shared<Scene>& scene);

		~SceneHierarchyPanel() = default;

		virtual void OnEvent(Event& event) override;
        virtual void SetSceneContext(const TerranEngine::Shared<TerranEngine::Scene>& scene) override;
		virtual void OnRender() override;
		virtual std::string_view GetName() override { return "Hierarchy"; }

	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		void DrawEntityNode(Entity entity);
		void DrawScene();

	private:
		ImGuiTextFilter m_Filter;
	};
}
