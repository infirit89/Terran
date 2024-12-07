#pragma once

#include "Project.h"

namespace TerranEngine
{
    class ProjectSerializer final
    {
    public:
        ProjectSerializer() = default;
        ProjectSerializer(const Shared<Project>& project);

        void Serialize();
        bool Deserialize();

    private:
        Shared<Project> m_Project;
        std::filesystem::path m_ProjectSettingsPath = "ProjectSettings.tstg";
    };
}
