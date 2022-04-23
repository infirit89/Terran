#pragma once

#include "ScriptClass.h"

#include "Core/Log.h"
#include "Core/Base.h"

#include "Scene/Entity.h"

namespace TerranEngine 
{
	class ScriptEngine 
	{

	public:
		static void Init(const char* fileName);
		static void CleanUp();

		static void NewDomain();
		static void UnloadDomain();

		static std::string GetAssemblyPath();

		static ScriptClass GetClass(const std::string& moduleName);

		static bool ClassExists(const std::string& moduleName);

		static void InitializeScriptable(Entity entity);
		static void UninitalizeScriptable(Entity entity);

		static void OnStart(Entity entity);
		static void OnUpdate(Entity entity);

		static void OnPhysicsBeginContact(Entity collider, Entity collidee);
		static void OnPhysicsEndContact(Entity collider, Entity collidee);
		
		static void OnPhysicsUpdate(Entity entity);

		static ScriptObject GetScriptInstanceScriptObject(const UUID& sceneUUID, const UUID& entityUUID);

		static void ClearFieldBackupMap();

		static void SetCurrentFieldStates(const UUID& sceneID);
	};
}