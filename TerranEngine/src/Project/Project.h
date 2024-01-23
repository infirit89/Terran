#pragma once

#include "Core/Base.h"

#include "Physics/Physics.h"

#include <filesystem>

namespace TerranEngine
{
    class Project
    {
    public:
        Project(const std::filesystem::path& projectPath);
        ~Project() = default;

        static Shared<Project> New(const std::filesystem::path& projectPath);

        static void SetActive(Shared<Project>& project);
        static Shared<Project> GetActive() { return s_ActiveProject; } 

        static std::filesystem::path GetProjectPath() 
        {
            TR_ASSERT(s_ActiveProject, "There is no active project");
            return s_ActiveProject->m_ProjectPath;
        }

        static std::filesystem::path GetAppAssemblyPath() 
        {
            return GetProjectPath() / s_ActiveProject->m_ScriptAssembliesPath / s_ActiveProject->m_AppAssemblyName;
        }

        static std::filesystem::path GetAssetPath() 
        {
            return GetProjectPath() / s_ActiveProject->m_AssetPath;
        }

        static std::filesystem::path GetAssetInfoDBPath() 
        {
            return GetProjectPath() / s_ActiveProject->m_AssetInfoDBPath;
        }

        static PhysicsSettings& GetPhysicsSettings() { return s_ActiveProject->m_PhysicsSettings; }

    private:
        static Shared<Project> s_ActiveProject;
        const std::filesystem::path m_AssetPath = "Assets";
        const std::filesystem::path m_ScriptAssembliesPath = "ScriptAssemblies/net8.0";
        const std::filesystem::path m_AppAssemblyName = "ScriptAssembly.dll";
        const std::filesystem::path m_AssetInfoDBPath = "AssetInfoDB";
        std::filesystem::path m_ProjectPath;
        PhysicsSettings m_PhysicsSettings;

        friend class ProjectSerializer;
    };
}
