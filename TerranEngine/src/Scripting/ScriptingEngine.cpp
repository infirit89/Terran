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
		std::string libPath = std::string(monoPath) + "/lib";
		std::string configPath = std::string(monoPath) + "/etc";

		mono_set_dirs(libPath.c_str(), configPath.c_str());

		m_Domain = mono_jit_init("TestSandbox");
		TR_ASSERT(m_Domain, "Couldn't initialize the MonoDomain");

		m_Assembly = mono_domain_assembly_open(m_Domain, fileName);
		if(!m_Assembly)
			TR_ASSERT(false, "Couldn't load the C# assembly");

		m_Image = mono_assembly_get_image(m_Assembly);
	}

	void ScriptingEngine::CleanUp()
	{
		mono_jit_cleanup(m_Domain);
	}

	Shared<ScriptClass> ScriptingEngine::GetClass(const char* namespaceName, const char* className)
	{
		MonoClass* klass = mono_class_from_name(m_Image, namespaceName, className);
		return CreateShared<ScriptClass>(klass);
	}
}
