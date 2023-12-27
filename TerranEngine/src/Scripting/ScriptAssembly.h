#pragma once

#include "ManagedClass.h"

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
	class ScriptAssembly 
	{
	public:
		ScriptAssembly() = default;
		ScriptAssembly(MonoAssembly* monoAssembly);
		
		// 'moduleName' has to have the format {Namespace}.{Class} e.g. "Terran.TestScript"
		ManagedClass GetClassFromName(const std::string& moduleName);
		ManagedClass GetClassFromTypeToken(uint32_t typeToken);

		// 'methodDesc' has to have the format {Namespace}.{Class}:{MethodName}({Params}) 
		// e.g. "Terran.TestScript:Update()" or "Terran.TestScript:OnCollisionBegin(Entity)"
		ManagedMethod GetMethodFromDesc(const std::string& methodDesc);
		
		static Shared<ScriptAssembly> LoadAssembly(const std::filesystem::path& assemblyPath);
		
		inline MonoImage* GetMonoImage() const { return m_MonoImage; }
		inline MonoAssembly* GetMonoAssembly() const { return m_MonoAssembly; }

	private:
		MonoAssembly* m_MonoAssembly = nullptr;
		MonoImage* m_MonoImage = nullptr;
	};
}