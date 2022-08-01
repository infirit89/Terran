#pragma once

#include "Core/Base.h"

#include "Scene/Scene.h"

#include "EditorPanel.h"

#include <unordered_map>
#include <string>

namespace TerranEditor
{
    class PanelManager 
    {
    public:
        PanelManager() = default;
        ~PanelManager() = default;

        template<typename T>
        TerranEngine::Shared<T> AddPanel(const std::string& panelName)
        {
            TerranEngine::Shared<T> editorPanel = TerranEngine::CreateShared<T>();
            m_Panels.emplace(panelName, editorPanel);
            return editorPanel;
        }

        template<typename T>
        TerranEngine::Shared<T> GetPanel(const std::string& panelName) { return TerranEngine::DynamicCast<T>(m_Panels[panelName]); }
        void SetScene(const TerranEngine::Shared<TerranEngine::Scene>& scene);

        void OnEvent(TerranEngine::Event& event);
        void ImGuiRender();

    private:
        std::unordered_map<std::string, TerranEngine::Shared<EditorPanel>> m_Panels;
    };
}

