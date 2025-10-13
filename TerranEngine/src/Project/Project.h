#pragma once

#include "LibCore/Assert.h"
#include "LibCore/Base.h"

#include "Physics/Physics.h"

#include <filesystem>

namespace TerranEngine {

class Project final {
public:
    Project(std::filesystem::path const& projectPath);
    ~Project() = default;

    static Terran::Core::Shared<Project> New(std::filesystem::path const& projectPath);

    static void SetActive(Terran::Core::Shared<Project> const& project);
    static Terran::Core::Shared<Project> GetActive() { return s_ActiveProject; }

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
    static Terran::Core::Shared<Project> s_ActiveProject;
    std::filesystem::path const m_AssetPath = "Assets";
    std::filesystem::path const m_ScriptAssembliesPath = "ScriptAssemblies/net8.0";
    std::filesystem::path const m_AppAssemblyName = "ScriptAssembly.dll";
    std::filesystem::path const m_AssetInfoDBPath = "AssetInfoDB";
    std::filesystem::path m_ProjectPath;
    PhysicsSettings m_PhysicsSettings;

    friend class ProjectSerializer;
};

}
