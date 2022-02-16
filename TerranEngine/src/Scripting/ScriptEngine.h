#pragma once

#include "Core/Log.h"
#include "Core/Base.h"

#include "Scene/Scene.h"

#include "ScriptClass.h"

namespace TerranEngine 
{
	struct ScriptComponent;

	class ScriptEngine 
	{

	public:
		static void Init(const char* fileName);
		static void CleanUp();

		static void NewDomain();
		static void UnloadDomain();

		static std::string GetAssemblyPath();

		static ScriptClass GetClass(const std::string& moduleName);

		static void InitializeScriptable(Entity entity);
		static void UninitalizeScriptable(Entity entity);

		static void StartScriptable(Entity entity);
		static void UpdateScriptable(Entity entity);
	};
}