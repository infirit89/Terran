#include "trpch.h"
#include "FileUtils.h"

namespace TerranEngine 
{
	FileSystem::FileSystemChangeCallbackFn FileSystem::s_ChangeCallback;
	std::filesystem::path FileSystem::s_PathToWatch;

	std::filesystem::path FileSystem::GetEnvironmentVariable(const char* name)
	{
		return std::getenv(name);
	}

	void FileSystem::SetFileSystemChangeCallback(const FileSystemChangeCallbackFn& changeCallback)
	{
		s_ChangeCallback = changeCallback;
	}

	void FileSystem::SetDirectoryToWatch(const std::filesystem::path& directoryPath)
	{
		s_PathToWatch = directoryPath;
	}

	bool FileSystem::Rename(const std::filesystem::path& oldPath, const std::filesystem::path& newPath) 
	{
		std::error_code errorCode;
		std::filesystem::rename(oldPath, newPath, errorCode);

		if (errorCode) return false;

		return true;
	}
}