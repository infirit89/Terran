#include "trpch.h"
#include "Project.h"

namespace TerranEngine
{
    struct ProjectData
    {
        std::filesystem::path ProjectPath;
        std::string ProjectName;
        std::filesystem::path AssetPath = "Assets";
        std::filesystem::path ResourcesPath = "Resources";
        std::filesystem::path ScriptAssembliesPath = "Scripts";
    };

    static ProjectData* s_ProjectData;
    
    void Project::Init(const std::filesystem::path& projectPath, const std::string& projectName)
    {
        s_ProjectData = new ProjectData;
        if(projectName == "")
            s_ProjectData->ProjectName = projectPath.parent_path().filename().string();
        else
            s_ProjectData->ProjectName = projectName;
        
        s_ProjectData->ProjectPath = projectPath;
    }

    void Project::Uninitialize()
    {
        delete s_ProjectData;
    }

    std::filesystem::path Project::GetAppAssemblyPath()
    {
        const std::string appAssemblyName = s_ProjectData->ProjectName + ".dll";
        return s_ProjectData->ProjectPath / s_ProjectData->ResourcesPath / s_ProjectData->ScriptAssembliesPath / appAssemblyName;
    }

    std::filesystem::path Project::GetCoreAssemblyPath()
    {
        return s_ProjectData->ProjectPath / s_ProjectData->ResourcesPath / s_ProjectData->ScriptAssembliesPath / "TerranScriptCore.dll";
    }

    std::filesystem::path Project::GetAssetsPath()
    {
        return s_ProjectData->ProjectPath / s_ProjectData->AssetPath;
    }
}
