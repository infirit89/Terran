#include "SettingsPanel.h"

#include "UI/UI.h"

#include "Physics/PhysicsLayerManager.h"

#include "Project/Project.h"
#include "spdlog/fmt/bundled/core.h"

#include <algorithm>
#include <imgui.h>
#include <string.h>
#include <string>
#include <vector>

namespace TerranEditor
{
    using namespace TerranEngine;
    void SettingsPanel::ImGuiRender()
    {
        if(!m_Open) return;

        ImGui::Begin("Settings", &m_Open);

        Shared<Project> activeProject = Project::GetActive();
        UI::DrawVec2Control("Gravity", activeProject->Settings.Gravity); 
        UI::DrawFloatControl("Fixed Timestep", activeProject->Settings.PhysicsTimestep);
        activeProject->Settings.PhysicsTimestep = std::max(activeProject->Settings.PhysicsTimestep, 0.001f);

        UI::DrawIntControl("Position Iterations", activeProject->Settings.PositionIterations);
        activeProject->Settings.PositionIterations = std::max(activeProject->Settings.PositionIterations , 1);
        UI::DrawIntControl("Velocity Iterations", activeProject->Settings.VelocityIterations);
        activeProject->Settings.VelocityIterations = std::max(activeProject->Settings.VelocityIterations, 1);

        {
            std::vector<const char*> layerNames = PhysicsLayerManager::GetLayerNames();
            UI::DrawComboBox("Layer", layerNames.data(), layerNames.size(), m_LayerIndex);
            
            ImGui::BeginTable("##LayerCollisionSelection", 1);
            for(int row = 0; row < layerNames.size(); row++ )
            {
                if(strlen(layerNames[row]) == 0) continue;
                ImGui::TableNextRow();
                
                ImGui::TableSetColumnIndex(0);
                bool canLayersCollide = PhysicsLayerManager::CanLayersCollide(m_LayerIndex, row);

                if(UI::DrawBoolControl(layerNames[row], canLayersCollide))
                    PhysicsLayerManager::SetLayerMask(m_LayerIndex, row, canLayersCollide);
            }

            ImGui::EndTable();
        }        
        
        ImGui::End();
    }
}
