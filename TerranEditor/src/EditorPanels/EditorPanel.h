#pragma once

#include "Core/Base.h"
#include "Events/Event.h"

#include "Scene/Scene.h"

namespace TerranEditor 
{
	class EditorPanel 
	{
    public:
        EditorPanel() = default;
        virtual ~EditorPanel() = default;

        virtual void SetSceneContext(const TerranEngine::Shared<TerranEngine::Scene>& scene) { m_Scene = scene; }
        virtual void ImGuiRender() = 0;
		virtual void OnEvent(TerranEngine::Event& event) {  }

        void SetOpen(bool open) { m_Open = open; }

    protected:
        bool m_Open = true;    
        TerranEngine::Shared<TerranEngine::Scene> m_Scene;
	};
}
