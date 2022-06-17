#pragma once

#include "ScriptClass.h"

#include "Core/Base.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <filesystem>

extern "C"
{
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoAssembly MonoAssembly;
}

namespace TerranEngine 
{
	struct AssemblyInfo 
	{
		using InfoMap = std::unordered_map<std::string, std::vector<std::string>>; 
		
		InfoMap ClassInfoMap;
		InfoMap MethodInfoMap;
		InfoMap FieldInfoMap;
		
		MonoImage* CurrentImage;
	};

	class ScriptAssembly 
	{
	public:
		ScriptAssembly() = default;
		ScriptAssembly(MonoAssembly* monoAssembly);

		Shared<AssemblyInfo> GenerateAssemblyInfo();
		
		// 'moduleName' has to have the format {Namespace}.{Class} e.g. "Terran.TestScript"
		ScriptClass GetClassFromName(const std::string& moduleName);
		ScriptClass GetClassFromTypeToken(uint32_t typeToken);

		// 'methodDesc' has to have the format {Namespace}.{Class}:{MethodName}({Params}) 
		// e.g. "Terran.TestScript:Update()" or "Terran.TestScript:OnCollisionBegin(Entity)"
		ScriptMethod GetMethodFromDesc(const std::string& methodDesc);
		
		static Shared<ScriptAssembly> LoadAssembly(const std::filesystem::path& assemblyPath);
		
		inline MonoImage* GetMonoImage() const { return m_MonoImage; }
		inline MonoAssembly* GetMonoAssembly() const { return m_MonoAssembly; }

	private:
		MonoAssembly* m_MonoAssembly = nullptr;
		MonoImage* m_MonoImage = nullptr;
	};
}