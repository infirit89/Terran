#include "trpch.h"
#include "FileUtils.h"

namespace TerranEngine 
{
	std::string FileUtils::GetEnvironmentVariable(const char* name)
	{
		return std::getenv(name);
	}
}