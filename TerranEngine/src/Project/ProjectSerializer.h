#pragma once

#include "Project.h"

namespace TerranEngine
{
    class ProjectSerialzer
    {
    public:
        ProjectSerialzer() = default;
        ProjectSerialzer(Shared<Project>& project);

        void SerializePhysicsSettings();
        bool DeserializePhysicsSettings();

    private:

        Shared<Project> m_Project;
        const std::filesystem::path m_ProjectSettingsPath = "ProjectSettings";
        const std::filesystem::path m_PhysicsSettingsFile = "PhysicsSettings.tstg";
    };
}
