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

		static std::string GetEnvironmentVariable(const char* name);
	};
}