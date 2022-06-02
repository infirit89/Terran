#pragma once

#include "ScriptClass.h"

#include "Core/Base.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <filesystem>

typedef struct _MonoImage MonoImage;
typedef struct _MonoAssembly MonoAssembly;

namespace TerranEngine 
{
	struct AssemblyInfo 
	{
		std::unordered_map<std::string, std::vector<std::string>> ClassInfoMap;
		MonoImage* CurrentImage;
	};

	class ScriptAssembly 
	{
	public:
		ScriptAssembly(MonoAssembly* monoAssembly);

		Shared<AssemblyInfo> GenerateAssemblyInfo();

		// 'moduleName' has to have the format {Namespace}.{Class} e.g. "Terran.TestScript"
		// NOTE: this has to be freed from the user of the function
		ScriptClass* GetClassFromName(const std::string& moduleName);
		
		static Shared<ScriptAssembly> LoadScriptAssembly(const std::filesystem::path& assemblyPath);

		inline MonoImage* GetMonoImage() const { return m_MonoImage; }
		inline MonoAssembly* GetMonoAssembly() const { return m_MonoAssembly; }

	private:
		MonoAssembly* m_MonoAssembly;
		MonoImage* m_MonoImage;
	};
}