#pragma once

#include <string>
#include <filesystem>

namespace TerranEngine 
{
	class FileUtils 
	{
	public:
		static std::filesystem::path OpenFile(const char* filter);
		static std::filesystem::path SaveFile(const char* filter);

		static std::filesystem::path GetEnvironmentVariable(const char* name);
		static void RevealInExplorer(const std::filesystem::path& path);
	};
}