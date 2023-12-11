#include "Core/Assert.h"

#include "PanelManager.h"

#include <imgui.h>

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

    void PanelManager::OnProjectChanged(const std::filesystem::path& projectPath)
    {
        for (const auto& [panelName, panel] : m_Panels)
            panel->OnProjectChanged(projectPath);
    }

    void PanelManager::ImGuiRender()
    {
        for (const auto& [panelName, panel] : m_Panels)
            panel->OnRender();
    }

    void PanelManager::SetPanelOpen(const std::string& panelName, bool open)
    {
        uint32_t hashedPanelName = Hash::FNVHash(panelName);
        TR_ASSERT(m_Panels.find(hashedPanelName) != m_Panels.end(),
                "Panel: {0} does not exist", panelName);

        auto& panel = m_Panels.at(hashedPanelName);
        panel->SetOpen(true);
        ImGui::SetWindowFocus(panel->GetName());
    }
}
