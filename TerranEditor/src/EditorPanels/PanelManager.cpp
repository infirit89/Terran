#include "PanelManager.h"

namespace TerranEditor
{
    using namespace TerranEngine;

    void PanelManager::SetScene(const Shared<Scene>& scene)
    {
        for(const auto& [panelName, panel] : m_Panels)
            panel->SetSceneContext(scene);
    }
    void PanelManager::OnEvent(Event& event)
    {
        for (const auto& [panelName, panel] : m_Panels)
            panel->OnEvent(event);
    }

    void PanelManager::ImGuiRender()
    {
        for (const auto& [panelName, panel] : m_Panels)
            panel->ImGuiRender();
    }
}
