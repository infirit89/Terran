#include "trpch.h"
#include "ScriptingTest.h"

#include <filesystem>

#include <Coral/HostInstance.hpp>
#include <Coral/Type.hpp>
#include <Coral/GC.hpp>
#include <Coral/Array.hpp>

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

	struct ID 
	{
		std::array<uint8_t, 4> Arr;
	};

	struct TestStruct 
	{
		uint8_t Arr[4];
	};

	static void PrintICall(Coral::String message)
	{
		TR_TRACE(std::string(message));
	}

	static UUID s_Id1;
	static UUID s_Id2;

	static Coral::Array<UUID> GetIDs() 
	{
		TR_TRACE(s_Id1);
		TR_TRACE(s_Id2);
		return Coral::Array<UUID>::New({s_Id1, s_Id2});
	}

	static void Cum(const UUID& id)
	{
		TR_TRACE(id);
		if (id != s_Id1 && id != s_Id2)
			TR_ERROR("fail");
		else
			TR_INFO("success");
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
		coreAssembly.AddInternalCall("Terran.Test", "PrintICall", reinterpret_cast<void*>(&PrintICall));
		coreAssembly.AddInternalCall("Terran.Test", "GetIDs", reinterpret_cast<void*>(&GetIDs));
		coreAssembly.AddInternalCall("Terran.Test", "Cum", reinterpret_cast<void*>(&Cum));
		coreAssembly.UploadInternalCalls();

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
		Coral::GC::Collect();
		s_HostInstance.UnloadAssemblyLoadContext(s_LoadContext);
		s_LoadContext = s_HostInstance.CreateAssemblyLoadContext("ScriptAppAssembly");
	}
}
