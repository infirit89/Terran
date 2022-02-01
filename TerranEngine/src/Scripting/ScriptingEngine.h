#pragma once

#include "Core/Log.h"
#include "Core/Base.h"
#include "ScriptClass.h"

#include <mono/jit/jit.h>

#include <unordered_map>

namespace TerranEngine 
{
	class ScriptingEngine 
	{

	public:
		ScriptingEngine() = delete;

		static void Init(const char* fileName);
		static void CleanUp();

		static inline MonoDomain* GetDomain() { return m_Domain; }

		static Shared<ScriptClass> GetClass(const std::string& moduleName);

		template <typename Func>
		static void BindInternalFunc(const char* funcName, Func func) 
		{
			mono_add_internal_call(funcName, func);
		}

	private:
		static MonoDomain* m_Domain;
		static MonoAssembly* m_Assembly;
		static MonoImage* m_Image;

		static std::unordered_map<uint32_t, Shared<ScriptClass>> m_Classes;
	};
}