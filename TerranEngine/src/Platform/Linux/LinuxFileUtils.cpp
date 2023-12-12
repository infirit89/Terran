#include "trpch.h"
#include "Core/FileUtils.h"

#ifdef TR_LINUX

namespace TerranEngine
{
    std::filesystem::path FileSystem::OpenFile(const char* filter) 
    {
        FILE* file = popen("zenity --file-selection", "r");       
        return "";
    }

    std::filesystem::path FileSystem::SaveFile(const char* filter) 
    {
        FILE* file = popen("zenity --file-selection --save", "r");
        return "";
    }

    void FileSystem::RevealInExplorer(const std::filesystem::path& path) 
    {
        // TODO:
    }

    void FileSystem::StartWatch() 
    {
        // TODO:
    }

    void FileSystem::StopWatch() 
    {
        // TODO:
    }

    unsigned long FileSystem::Watch(void* params) 
    {
        return 0;
    }
}

#endif
