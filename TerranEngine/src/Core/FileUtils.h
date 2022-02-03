#pragma once

#include <string>

namespace TerranEngine 
{
	class FileUtils 
	{
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);

		static std::string GetEnvVar(const char* name);
	};
}