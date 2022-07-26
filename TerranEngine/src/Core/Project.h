#pragma once

#include <filesystem>

namespace TerranEngine
{
    class Project
    {
    public:
        static void Init(const std::filesystem::path& projectPath);
        static void SetProjectPath(const std::filesystem::path& projectPath);
        static void Uninitialize();
        static std::filesystem::path GetAppAssemblyPath();
        static std::filesystem::path GetAssetsPath();
    };
}

