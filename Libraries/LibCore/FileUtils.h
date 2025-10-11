#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <vector>

namespace Terran {
namespace Core {

enum class FileAction : std::uint8_t {
    Added = 0,
    Removed,
    Modified,
    Renamed
};

struct FileSystemChangeEvent final {
    FileAction Action;
    std::filesystem::path FileName;
    std::filesystem::path OldFileName;
};

class FileSystem final {
private:
    using FileSystemChangeCallbackFn = std::function<void(std::vector<FileSystemChangeEvent> const&)>;

public:
    static std::filesystem::path OpenFileDialog(char const* filter);
    static std::filesystem::path SaveFileDialog(char const* filter);
    static bool Rename(std::filesystem::path const& oldPath, std::filesystem::path const& newPath);

    static std::filesystem::path GetEnvironmentVariable(char const* name);
    static void RevealInExplorer(std::filesystem::path const& path);
    static std::filesystem::file_time_type GetModifiedTime(std::filesystem::path const& path);

    static void StartWatch();
    static void SetDirectoryToWatch(std::filesystem::path const& directoryPath);
    static void StopWatch();
    static void SetFileSystemChangeCallback(FileSystemChangeCallbackFn const& changeCallback);

private:
    static unsigned long Watch(void* params);

    static FileSystemChangeCallbackFn s_ChangeCallback;
    static std::filesystem::path s_PathToWatch;
};

}

}
