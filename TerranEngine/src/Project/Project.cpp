#include "Project.h"
#include "trpch.h"

namespace TerranEngine {

Terran::Core::Shared<Project> Project::s_ActiveProject;

Project::Project(std::filesystem::path const& projectPath)
{
    m_ProjectPath = projectPath;
}

Terran::Core::Shared<Project> Project::New(std::filesystem::path const& projectPath)
{
    s_ActiveProject = Terran::Core::CreateShared<Project>(projectPath);
    return s_ActiveProject;
}

void Project::SetActive(Terran::Core::Shared<Project> const& project)
{
    s_ActiveProject = project;
}

}
