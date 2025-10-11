#include "FileUtils.h"
#include <filesystem>
#include <system_error>

namespace Terran {
namespace Core {

FileSystem::FileSystemChangeCallbackFn FileSystem::s_ChangeCallback;
std::filesystem::path FileSystem::s_PathToWatch;

std::filesystem::path FileSystem::GetEnvironmentVariable(char const* name)
{
    return std::getenv(name);
}

void FileSystem::SetFileSystemChangeCallback(FileSystemChangeCallbackFn const& changeCallback)
{
    s_ChangeCallback = changeCallback;
}

void FileSystem::SetDirectoryToWatch(std::filesystem::path const& directoryPath)
{
    s_PathToWatch = directoryPath;
}

bool FileSystem::Rename(std::filesystem::path const& oldPath, std::filesystem::path const& newPath)
{
    std::error_code errorCode;
    std::filesystem::rename(oldPath, newPath, errorCode);

    return !errorCode;
}

std::filesystem::file_time_type FileSystem::GetModifiedTime(std::filesystem::path const& path)
{
    return std::filesystem::last_write_time(path);
}

}

}
