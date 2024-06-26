#include "SettingsPanel.h"

#include "UI/UI.h"

#include "Physics/PhysicsLayerManager.h"

#include "Project/Project.h"
#include "Project/ProjectSerializer.h"

#include "Utils/Debug/OptickProfiler.h"

#include <spdlog/fmt/bundled/core.h>

#include <algorithm>
#include <imgui.h>
#include <string.h>
#include <string>
#include <vector>

namespace TerranEditor
{
    using namespace TerranEngine;
    void SettingsPanel::OnRender()
    {
        TR_PROFILE_FUNCTION();
        if(!m_Open) return;

        ImGui::Begin(GetName().data(), &m_Open);

        DrawPhysicsSettings();
        
        ImGui::End();
    }

    void SettingsPanel::DrawPhysicsSettings() 
    {
        bool serializeSettings = false;
        Shared<Project> activeProject = Project::GetActive();
        PhysicsSettings& settings = activeProject->GetPhysicsSettings();

        if (!UI::BeginPropertyGroup("physics_settings"))
            return;

        ImGui::TableNextRow();
        serializeSettings |= UI::PropertyVec2("Gravity", settings.Gravity);

        ImGui::TableNextRow();
        serializeSettings |= UI::PropertyFloat("Fixed Timestep", settings.PhysicsTimestep);

        settings.PhysicsTimestep = std::max(settings.PhysicsTimestep, 0.001f);

        ImGui::TableNextRow();
        serializeSettings |= UI::PropertyInt("Position Iterations", settings.PositionIterations);
        settings.PositionIterations = std::max(settings.PositionIterations, 1);

        ImGui::TableNextRow();
        serializeSettings |= UI::PropertyInt("Velocity Iterations", settings.VelocityIterations);
        settings.VelocityIterations = std::max(settings.VelocityIterations, 1);

        ImGui::TableNextColumn();
        std::vector<const char*> layerNames = PhysicsLayerManager::GetLayerNames();
        serializeSettings |= UI::PropertyDropdown("Layer", layerNames.data(), layerNames.size(), m_LayerIndex);
        UI::EndPropertyGroup();

        {
            ImGui::BeginTable("##LayerCollisionSelection", 1);
            for (int row = 0; row < layerNames.size(); row++)
            {
                if (strlen(layerNames[row]) == 0) continue;
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                UI::BeginPropertyGroup("layer_mask");
                ImGui::TableNextRow();

                bool canLayersCollide = PhysicsLayerManager::CanLayersCollide(m_LayerIndex, row);

                if (UI::PropertyBool(layerNames[row], canLayersCollide)) 
                {
                    PhysicsLayerManager::SetLayerMask(m_LayerIndex, row, canLayersCollide);
                    serializeSettings |= true;
                }

                UI::EndPropertyGroup();
            }
            ImGui::EndTable();
        }

        if (serializeSettings) 
        {
            ProjectSerializer serializer(Project::GetActive());
            serializer.Serialize();
        }
    }
}
