#pragma once

#include <unordered_map>
#include <string>
#include <vector>

typedef struct _MonoImage MonoImage;

namespace TerranEngine 
{
	struct AssemblyInfo 
	{
		std::unordered_map<std::string, std::vector<std::string>> ClassInfoMap;
		MonoImage* CurrentImage;
	};

	class ScriptAssembly 
	{

	};
}