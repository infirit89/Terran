#pragma once

#include "Core/PlatformCheck.h"

#include <string>
#include <filesystem>
#include <functional>

namespace TerranEngine 
{
	enum class FileAction 
	{
		None = 0,
		Added,
		Removed,
		Modified,
		Renamed
	};

	struct FileSystemChangeEvent
	{
		FileAction Action;
		std::filesystem::path FileName;
		std::filesystem::path OldFileName;
	};

	class FileSystem 
	{
	private:
		using FileSystemChangeCallbackFn = std::function<void(const std::vector<FileSystemChangeEvent>&)>;

	public:

		static std::filesystem::path OpenFile(const char* filter);
		static std::filesystem::path SaveFile(const char* filter);
		static bool Rename(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);

		static std::filesystem::path GetEnvironmentVariable(const char* name);
		static void RevealInExplorer(const std::filesystem::path& path);
		static std::filesystem::file_time_type GetModifiedTime(const std::filesystem::path& path);

		static void StartWatch();
		static void SetDirectoryToWatch(const std::filesystem::path& directoryPath);
		static void StopWatch();
		static void SetFileSystemChangeCallback(const FileSystemChangeCallbackFn& changeCallback);

	private:

#if defined(TR_WINDOWS_64)
		static unsigned long Watch(void* params);
#elif defined(TR_LINUX)
		static void* Watch(void* params);
#endif

		static FileSystemChangeCallbackFn s_ChangeCallback;
		static std::filesystem::path s_PathToWatch;
	};
}