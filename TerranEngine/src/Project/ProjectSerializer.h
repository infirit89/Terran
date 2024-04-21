#pragma once

#include "Project.h"

namespace TerranEngine
{
    class ProjectSerializer
    {
    public:
        ProjectSerializer() = default;
        ProjectSerializer(Shared<Project> project);

        void Serialize();
        bool Deserizlize();

    private:
        Shared<Project> m_Project;
        const std::filesystem::path m_ProjectSettingsPath = "ProjectSettings.tstg";
    };
}
