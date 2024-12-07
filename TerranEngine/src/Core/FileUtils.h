#pragma once

#include <string>
#include <filesystem>
#include <functional>

namespace TerranEngine 
{
	enum class FileAction 
	{
		Added = 0,
		Removed,
		Modified,
		Renamed
	};

	struct FileSystemChangeEvent final
	{
		FileAction Action;
		std::filesystem::path FileName;
		std::filesystem::path OldFileName;
	};

	class FileSystem final
	{
	private:
		using FileSystemChangeCallbackFn = std::function<void(const std::vector<FileSystemChangeEvent>&)>;

	public:

		static std::filesystem::path OpenFileDialog(const char* filter);
		static std::filesystem::path SaveFileDialog(const char* filter);
		static bool Rename(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);

		static std::filesystem::path GetEnvironmentVariable(const char* name);
		static void RevealInExplorer(const std::filesystem::path& path);
		static std::filesystem::file_time_type GetModifiedTime(const std::filesystem::path& path);

		static void StartWatch();
		static void SetDirectoryToWatch(const std::filesystem::path& directoryPath);
		static void StopWatch();
		static void SetFileSystemChangeCallback(const FileSystemChangeCallbackFn& changeCallback);

	private:
		static unsigned long Watch(void* params);

		static FileSystemChangeCallbackFn s_ChangeCallback;
		static std::filesystem::path s_PathToWatch;
	};
}