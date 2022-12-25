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
		Modified
	};

	struct FileChangeEvent
	{
		FileAction Action;
		std::string FileName;
	};

	class FileSystem 
	{
	private:
		using FileSystemChangeCallbackFn = std::function<void(const std::vector<FileChangeEvent>&)>;

	public:

		static std::filesystem::path OpenFile(const char* filter);
		static std::filesystem::path SaveFile(const char* filter);

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