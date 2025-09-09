#include "trpch.h"
#include "Core/FileUtils.h"

#include <filesystem>

namespace TerranEngine {

	// TODO: implement the methods, currently left blank due to having linking problems
	// add an ifdef at the top of the file so its only included in macos builds
	std::filesystem::path FileSystem::OpenFileDialog(const char* filter) 
	{
		TR_ASSERT(false, "Not implemented");
		return "";
	}

	std::filesystem::path FileSystem::SaveFileDialog(const char* filter) 
	{
		TR_ASSERT(false, "Not implemented");
		return "";
	}

	void FileSystem::RevealInExplorer(const std::filesystem::path& path)
	{
		TR_ASSERT(false, "Not implemented");
	}

	void FileSystem::StartWatch()
	{
		TR_ASSERT(false, "Not implemented");
	}

	void FileSystem::StopWatch()
	{
		TR_ASSERT(false, "Not implemented");
	}

	// this is the win32api thread proc signature
	unsigned long FileSystem::Watch(void* params)
	{
		TR_ASSERT(false, "Not implemented");

		return 1;
	}

}
