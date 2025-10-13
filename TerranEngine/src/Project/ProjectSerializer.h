#pragma once

#include "Project.h"

namespace TerranEngine {

class ProjectSerializer final {
public:
    ProjectSerializer() = default;
    ProjectSerializer(Terran::Core::Shared<Project> const& project);

    void Serialize();
    bool Deserialize();

private:
    Terran::Core::Shared<Project> m_Project;
    std::filesystem::path m_ProjectSettingsPath = "ProjectSettings.tstg";
};

}
