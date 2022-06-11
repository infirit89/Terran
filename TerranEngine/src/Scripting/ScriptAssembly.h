#pragma once

#include "ScriptClass.h"

#include "Core/Base.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <filesystem>
#include <unordered_set>

typedef struct _MonoImage MonoImage;
typedef struct _MonoAssembly MonoAssembly;

namespace TerranEngine 
{
	struct AssemblyInfo 
	{
		std::unordered_map<std::string, std::vector<std::string>> ClassInfoMap;
		std::unordered_map<std::string, std::vector<std::string>> MethodInfoMap;

		MonoImage* CurrentImage;
	};

	class ScriptAssembly 
	{
	public:
		ScriptAssembly(MonoAssembly* monoAssembly);

		Shared<AssemblyInfo> GenerateAssemblyInfo();

		// 'moduleName' has to have the format {Namespace}.{Class} e.g. "Terran.TestScript"
		ScriptClass GetClassFromName(const std::string& moduleName);

		// 'methodDesc' has to have the format {Namespace}.{Class}:{MethodName}({Params}) 
		// e.g. "Terran.TestScript:Update()" or "Terran.TestScript:OnCollisionBegin(Entity)"
		ScriptMethod GetMethodFromDesc(const std::string& methodDesc);
		
		static Shared<ScriptAssembly> LoadScriptAssembly(const std::filesystem::path& assemblyPath);

		inline MonoImage* GetMonoImage() const { return m_MonoImage; }
		inline MonoAssembly* GetMonoAssembly() const { return m_MonoAssembly; }

	private:
		MonoAssembly* m_MonoAssembly;
		MonoImage* m_MonoImage;
	};
}