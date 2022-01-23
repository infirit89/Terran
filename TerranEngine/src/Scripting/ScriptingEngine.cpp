#include "trpch.h"
#include "ScriptingEngine.h"

#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

namespace TerranEngine
{
	MonoDomain* ScriptingEngine::m_Domain;
	MonoAssembly* ScriptingEngine::m_Assembly;
	MonoImage* ScriptingEngine::m_Image;

	void ScriptingEngine::Init(const char* fileName)
	{
		char* monoPath = std::getenv("MONO_PATH");
		
		size_t monoLibDirLen = strlen(monoPath) + 4;

		char* libPath = new char[monoLibDirLen];
		strcpy(libPath, monoPath);
		strcat(libPath, "lib");

		char* etcPath = new char[monoLibDirLen];
		strcpy(etcPath, monoPath);
		strcat(etcPath, "etc");

		libPath[monoLibDirLen - 1] = '\0';
		etcPath[monoLibDirLen - 1] = '\0';

		mono_set_dirs(libPath, etcPath);

		delete[] libPath;
		delete[] etcPath;

		m_Domain = mono_jit_init("TestSandbox");
		TR_ASSERT(m_Domain, "Couldn't initialize the MonoDomain");

		m_Assembly = mono_domain_assembly_open(m_Domain, fileName);
		TR_ASSERT(m_Assembly, "Couldn't load the C# assembly");

		m_Image = mono_assembly_get_image(m_Assembly);

		TR_INFO("Successfuly loaded: {0}", fileName);
	}

	void ScriptingEngine::CleanUp()
	{
		mono_jit_cleanup(m_Domain);
		TR_INFO("Deinitialized the scripting core");
	}

	Shared<ScriptClass> ScriptingEngine::GetClass(const char* namespaceName, const char* className)
	{
		return CreateShared<ScriptClass>(mono_class_from_name(m_Image, namespaceName, className));
	}
}
