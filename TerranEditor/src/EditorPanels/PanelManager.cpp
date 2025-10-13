#include "LibCore/Assert.h"
#include "Utils/Debug/OptickProfiler.h"

#include "PanelManager.h"

#include <imgui.h>
#include <cstdint>
#include <filesystem>

namespace TerranEditor {

using namespace TerranEngine;

void PanelManager::SetScene(Terran::Core::Shared<Scene> const& scene)
{
    for (auto const& [panelName, panel] : m_Panels)
        panel->SetSceneContext(scene);
}
void PanelManager::OnEvent(Terran::Core::Event& event)
{
    TR_PROFILE_FUNCTION();
    for (auto const& [panelName, panel] : m_Panels)
        panel->OnEvent(event);
}

void PanelManager::OnProjectChanged(std::filesystem::path const& projectPath)
{
    for (auto const& [panelName, panel] : m_Panels)
        panel->OnProjectChanged(projectPath);
}

void PanelManager::ImGuiRender()
{
    TR_PROFILE_FUNCTION();
    for (auto const& [panelName, panel] : m_Panels)
        panel->OnRender();
}

void PanelManager::SetPanelOpen(std::string_view panelName, bool open)
{
    uint32_t hashedPanelName = Terran::Core::Hash::FNVHash(panelName);
    TR_ASSERT(m_Panels.find(hashedPanelName) != m_Panels.end(),
        "Panel: {0} does not exist", panelName);

    auto& panel = m_Panels.at(hashedPanelName);
    panel->SetOpen(true);
    ImGui::SetWindowFocus(panel->GetName().data());
}

}
