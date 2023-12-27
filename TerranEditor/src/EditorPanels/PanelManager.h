#pragma once

#include "Core/Base.h"
#include "Core/Hash.h"

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
        TerranEngine::Shared<T> AddPanel(std::string_view panelName)
        {
            TerranEngine::Shared<T> panel = TerranEngine::CreateShared<T>();
            uint32_t hashedPanelName = TerranEngine::Hash::FNVHash(panelName);
            m_Panels.emplace(hashedPanelName, panel);
            return panel;
        }

        template<typename T>
        TerranEngine::Shared<T> GetPanel(const std::string& panelName)
        {
            uint32_t hashedPanelName = TerranEngine::Hash::FNVHash(panelName);
            return TerranEngine::DynamicCast<T>(m_Panels[hashedPanelName]);
        }

        void SetScene(const TerranEngine::Shared<TerranEngine::Scene>& scene);

        void OnEvent(TerranEngine::Event& event);
        void OnProjectChanged(const std::filesystem::path& projectPath);

        void ImGuiRender();

        void SetPanelOpen(std::string_view panelName, bool open);

    private:
        std::unordered_map<uint32_t, TerranEngine::Shared<EditorPanel>> m_Panels;
    };
}
