#include "trpch.h"
#include "Project.h"

namespace TerranEngine
{
    struct ProjectData
    {
        std::filesystem::path ProjectPath;
        const std::filesystem::path AssetPath = "Assets";
        const std::filesystem::path ScriptAssembliesPath = "Scripts";
        const std::filesystem::path AppAssemblyName = "ScriptAssembly.dll";
    };

    static ProjectData* s_ProjectData;
    
    void Project::Init(const std::filesystem::path& projectPath)
    {
        s_ProjectData = new ProjectData;
        s_ProjectData->ProjectPath = projectPath;
    }

    void Project::Uninitialize()
    {
        delete s_ProjectData;
    }

    void Project::SetProjectPath(const std::filesystem::path& projectPath)
    {
        s_ProjectData->ProjectPath = projectPath;
    }

    std::filesystem::path Project::GetAppAssemblyPath()
    {
        return s_ProjectData->ProjectPath / 
            s_ProjectData->ScriptAssembliesPath / s_ProjectData->AppAssemblyName;
    }

    std::filesystem::path Project::GetAssetsPath()
    {
        return s_ProjectData->ProjectPath / s_ProjectData->AssetPath;
    }
}
