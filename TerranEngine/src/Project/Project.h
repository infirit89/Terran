#pragma once

#include "Core/Base.h"

#include "Physics/Physics.h"

#include <filesystem>

namespace TerranEngine
{
    class Project
    {
    public:
        static void SetActive(Shared<Project>& project);
        static Shared<Project> GetActive() { return s_ActiveProject; } 

        static std::filesystem::path GetAppAssemblyPath();
        static std::filesystem::path GetAssetPath();
        static PhysicsSettings GetPhysicsSettings() { return s_ActiveProject->Settings; }

    public:
        std::filesystem::path ProjectPath;
        PhysicsSettings Settings;

    private:
        static Shared<Project> s_ActiveProject;
        const std::filesystem::path m_AssetPath = "Assets";
        const std::filesystem::path m_ScriptAssembliesPath = "ScriptAssemblies";
        const std::filesystem::path m_AppAssemblyName = "ScriptAssembly.dll";
    };
}

