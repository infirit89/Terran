#pragma once

#include "LibCore/Base.h"
#include "LibCore/Hash.h"

#include "Scene/Scene.h"

#include "EditorPanel.h"

#include <string>
#include <unordered_map>
#include <cstdint>
#include <string_view>
#include <filesystem>

namespace TerranEditor {

class PanelManager {
public:
    PanelManager() = default;
    ~PanelManager() = default;

    template<typename T>
    Terran::Core::Shared<T> AddPanel(std::string_view panelName)
    {
        Terran::Core::Shared<T> panel = Terran::Core::CreateShared<T>();
        uint32_t hashedPanelName = Terran::Core::Hash::FNVHash(panelName);
        m_Panels.emplace(hashedPanelName, panel);
        return panel;
    }

    template<typename T>
    Terran::Core::Shared<T> GetPanel(std::string const& panelName)
    {
        uint32_t hashedPanelName = Terran::Core::Hash::FNVHash(panelName);
        return Terran::Core::DynamicCast<T>(m_Panels[hashedPanelName]);
    }

    void SetScene(Terran::Core::Shared<TerranEngine::Scene> const& scene);

    void OnEvent(Terran::Core::Event& event);
    void OnProjectChanged(std::filesystem::path const& projectPath);

    void ImGuiRender();

    void SetPanelOpen(std::string_view panelName, bool open);

private:
    std::unordered_map<uint32_t, Terran::Core::Shared<EditorPanel>> m_Panels;
};

}
