#include "trpch.h"
#include "FileUtils.h"

namespace TerranEngine 
{
	std::filesystem::path FileUtils::GetEnvironmentVariable(const char* name)
	{
		return std::getenv(name);
	}
}