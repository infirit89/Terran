#pragma once

#include <filesystem>

namespace TerranEngine
{
    class Project
    {
    public:
        static void Init(const std::filesystem::path& projectPath, const std::string& projectName = "");
        static void Uninitialize();
        static std::filesystem::path GetAppAssemblyPath();
        static std::filesystem::path GetCoreAssemblyPath();
        static std::filesystem::path GetAssetsPath();
    };
}
