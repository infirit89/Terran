#include "trpch.h"
#include "FileUtils.h"

namespace TerranEngine 
{
	std::string FileUtils::GetEnvVar(const char* name) 
	{
		return std::getenv(name);
	}
}