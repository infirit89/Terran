#include "SettingsPanel.h"

#include "UI/UI.h"

#include "Physics/PhysicsLayerManager.h"

#include "Project/Project.h"
#include "Project/ProjectSerializer.h"

#include <spdlog/fmt/bundled/core.h>

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

        DrawPhysicsSettings();
        
        ImGui::End();
    }

    void SettingsPanel::DrawPhysicsSettings() 
    {
        bool serializeSettings = false;
        Shared<Project> activeProject = Project::GetActive();
        serializeSettings |= UI::PropertyVec2("Gravity", activeProject->Settings.Gravity);
        serializeSettings |= UI::PropertyFloat("Fixed Timestep", activeProject->Settings.PhysicsTimestep);
        activeProject->Settings.PhysicsTimestep = std::max(activeProject->Settings.PhysicsTimestep, 0.001f);

        serializeSettings |= UI::PropertyInt("Position Iterations", activeProject->Settings.PositionIterations);
        activeProject->Settings.PositionIterations = std::max(activeProject->Settings.PositionIterations, 1);
        serializeSettings |= UI::PropertyInt("Velocity Iterations", activeProject->Settings.VelocityIterations);
        activeProject->Settings.VelocityIterations = std::max(activeProject->Settings.VelocityIterations, 1);

        {
            std::vector<const char*> layerNames = PhysicsLayerManager::GetLayerNames();
            serializeSettings |= UI::PropertyComboBox("Layer", layerNames.data(), layerNames.size(), m_LayerIndex);

            ImGui::BeginTable("##LayerCollisionSelection", 1);
            for (int row = 0; row < layerNames.size(); row++)
            {
                if (strlen(layerNames[row]) == 0) continue;
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                bool canLayersCollide = PhysicsLayerManager::CanLayersCollide(m_LayerIndex, row);

                if (UI::PropertyBool(layerNames[row], canLayersCollide)) 
                {
                    PhysicsLayerManager::SetLayerMask(m_LayerIndex, row, canLayersCollide);
                    serializeSettings |= true;
                }
            }

            ImGui::EndTable();
        }

        if (serializeSettings) 
        {
            ProjectSerializer serializer(Project::GetActive());
            serializer.SerializePhysicsSettings();
        }
    }
}
