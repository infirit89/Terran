#include "trpch.h"
#include "ScriptingTest.h"

#include <filesystem>

#include <Coral/HostInstance.hpp>
#include <Coral/Type.hpp>
#include <Coral/GC.hpp>

namespace TerranEngine
{
	static const std::string s_CoralDirectory = "Resources/Scripts";
	static std::string_view s_CoreAssemblyPath = "Resources/Scripts/TerranScriptCore.dll";
	static Coral::HostInstance s_HostInstance;
	static Coral::AssemblyLoadContext s_LoadContext;

	static void OnError(std::string_view message) 
	{
		TR_ERROR(message);
	}

	static void OnMessage(std::string_view message, Coral::MessageLevel messageLevel) 
	{
		switch(messageLevel) 
		{
		case Coral::MessageLevel::Info:		TR_INFO(message); break;
		case Coral::MessageLevel::Warning:	TR_WARN(message); break;
		case Coral::MessageLevel::Error:	TR_ERROR(message); break;
		}

		TR_TRACE(message);
	}

	void ScriptingTest::Initialize()
	{
		Coral::HostSettings settings =
		{
			.CoralDirectory = s_CoralDirectory,
			.MessageCallback = OnMessage,
			.ExceptionCallback = OnError
		};

		s_HostInstance.Initialize(settings);
		s_LoadContext = s_HostInstance.CreateAssemblyLoadContext("ScriptAppAssembly");

		Coral::ManagedAssembly& coreAssembly = s_LoadContext.LoadAssembly(s_CoreAssemblyPath);
		Coral::Type& test = coreAssembly.GetType("Terran.Test");
		test.InvokeStaticMethod("T1");
	}

	void ScriptingTest::Shutdown()
	{
		Coral::GC::Collect();
		s_HostInstance.UnloadAssemblyLoadContext(s_LoadContext);
	}

	void ScriptingTest::Reload()
	{
	}
}
