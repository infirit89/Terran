#pragma once

#include "ScriptClass.h"
#include "GCManager.h"

#include "Core/Log.h"
#include "Core/Base.h"
#include "Core/UUID.h"

#include "Scene/Entity.h"
#include "Scene/Scene.h"

#include <filesystem>
#include <unordered_map>

namespace TerranEngine 
{
#define TR_CORE_ASSEMBLY_INDEX 0
#define TR_APP_ASSEMBLY_INDEX 1
#define TR_ASSEMBLIES ((TR_APP_ASSEMBLY_INDEX) + 1)
	
	class ScriptEngine 
	{
		using ScriptInstanceMap = std::unordered_map<UUID, std::unordered_map<UUID, GCHandle>>;

	public:
		static void Initialize();
		static void Shutdown();

		static void ReloadAppAssembly();

		static void SetContext(Scene* context);
		static Shared<Scene>& GetContext();
		
		static void LoadCoreAssembly(const std::filesystem::path& assemblyPath);
		static void LoadAppAssembly(const std::filesystem::path& assemblyPath);
		
		static void CreateAppDomain();
		static void UnloadDomain();

		static std::filesystem::path GetAssemblyPath();

		static ScriptClass GetClassFromName(const std::string& moduleName, int assemblyIndex);
		static ScriptClass GetClassFromTypeToken(uint32_t typeToken, int assemblyIndex);
		
		static ScriptMethod GetMethodFromDesc(const std::string& methodDesc, int assemblyIndex);
		static bool ClassExists(const std::string& moduleName);

		static Shared<ScriptAssembly>& GetAssembly(int assemblyIndex);
		
		static void InitializeScriptable(Entity entity);
		static void UninitalizeScriptable(Entity entity);

		static void OnStart(Entity entity);
		static void OnUpdate(Entity entity);

		static void OnPhysicsBeginContact(Entity collider, Entity collidee);
		static void OnPhysicsEndContact(Entity collider, Entity collidee);
		
		static void OnPhysicsUpdate(Entity entity);

		static ScriptObject GetScriptInstanceScriptObject(const UUID& sceneUUID, const UUID& entityUUID);
		static GCHandle GetScriptInstanceGCHandle(const UUID& sceneUUID, const UUID& entityUUID);
	};
}
