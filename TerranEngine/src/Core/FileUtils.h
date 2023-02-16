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
		RenamedOldName,
		RenamedNewName
	};

	struct FileSystemChangeEvent
	{
		FileAction Action;
		std::filesystem::path FileName;
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