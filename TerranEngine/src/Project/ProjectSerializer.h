#pragma once

#include "Project.h"

namespace TerranEngine
{
    class ProjectSerializer
    {
    public:
        ProjectSerializer() = default;
        ProjectSerializer(Shared<Project>& project);

        void Serialize();
        bool Deserizlize();

    private:
        void SerializePhysicsSettings();
        bool DeserializePhysicsSettings();

    private:
        Shared<Project> m_Project;
        const std::filesystem::path m_ProjectSettingsPath = "ProjectSettings";
        const std::filesystem::path m_PhysicsSettingsFile = "PhysicsSettings.tstg";
    };
}
