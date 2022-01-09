#pragma once

#include "Core/Log.h"
#include "Core/Base.h"
#include "ScriptClass.h"

#include <mono/jit/jit.h>

namespace TerranEngine 
{
	class ScriptingEngine 
	{

	public:
		ScriptingEngine() = delete;

		static void Init(const char* fileName);
		static void CleanUp();

		static inline MonoDomain* GetDomain() { return m_Domain; }

		static Shared<ScriptClass> GetClass(const char* namespaceName, const char* className);
	private:
		static MonoDomain* m_Domain;
		static MonoAssembly* m_Assembly;
		static MonoImage* m_Image;
	};
}