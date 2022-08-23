#include "trpch.h"
#include "Project.h"

namespace TerranEngine
{
    Shared<Project> Project::s_ActiveProject;
    void Project::SetActive(Shared<Project>& project)
    {
        s_ActiveProject = project;
    }

    std::filesystem::path Project::GetAppAssemblyPath()
    {
        return s_ActiveProject->ProjectPath / 
            s_ActiveProject->m_ScriptAssembliesPath / s_ActiveProject->m_AppAssemblyName;
    }

    std::filesystem::path Project::GetAssetPath()
    {
        return s_ActiveProject->ProjectPath / s_ActiveProject->m_AssetPath;
    }
}
