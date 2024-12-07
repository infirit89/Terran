#include "trpch.h"
#include "Project.h"

namespace TerranEngine
{
    Shared<Project> Project::s_ActiveProject;

    Project::Project(const std::filesystem::path& projectPath)
    {
        m_ProjectPath = projectPath;
    }

    Shared<Project> Project::New(const std::filesystem::path& projectPath)
    {
        s_ActiveProject = CreateShared<Project>(projectPath);
        return s_ActiveProject;
    }

    void Project::SetActive(const Shared<Project>& project)
    {
        s_ActiveProject = project;
    }
}
