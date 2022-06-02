#pragma once

#include "ScriptClass.h"
#include "GCManager.h"

#include "Core/Log.h"
#include "Core/Base.h"
#include "Core/UUID.h"

#include "Scene/Entity.h"

#include <filesystem>
#include <unordered_map>

namespace TerranEngine 
{
	#define API_NAMESPACE "Terran"

	class ScriptEngine 
	{
		using ScriptInstanceMap = std::unordered_map<UUID, std::unordered_map<UUID, GCHandle>>;

	public:
		static void Initialize(const std::filesystem::path& asseblyPath);
		static void Shutdown();

		static void NewDomain();
		static void UnloadDomain();

		static std::filesystem::path GetAssemblyPath();

		static ScriptClass* GetClassFromName(const std::string& moduleName);
		static bool ClassExists(const std::string& moduleName);

		static void InitializeScriptable(Entity entity);
		static void UninitalizeScriptable(Entity entity);

		static void OnStart(Entity entity);
		static void OnUpdate(Entity entity);

		static void OnPhysicsBeginContact(Entity collider, Entity collidee);
		static void OnPhysicsEndContact(Entity collider, Entity collidee);
		
		static void OnPhysicsUpdate(Entity entity);

		static ScriptObject GetScriptInstanceScriptObject(const UUID& sceneUUID, const UUID& entityUUID);
		static GCHandle GetScriptInstanceGCHandle(const UUID& sceneUUID, const UUID& entityUUID);

		static void ClearFieldBackupMap();

		static void SetCurrentFieldStates(const UUID& sceneID);
	};
}
