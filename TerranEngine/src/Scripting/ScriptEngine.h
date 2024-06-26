#pragma once

#include "ScriptInstance.h"

#include "Core/Base.h"
#include "Core/UUID.h"

#include "Scene/Scene.h"

#include <filesystem>
#include <spdlog/spdlog.h>

namespace TerranEngine 
{
#define TR_CORE_ASSEMBLY_INDEX 0
#define TR_APP_ASSEMBLY_INDEX 1
#define TR_ASSEMBLIES ((TR_APP_ASSEMBLY_INDEX) + 1)
	
	class ScriptEngine
	{
	public:
		static void Initialize(const std::filesystem::path& scriptCoreAssemblyPath);
		static void Shutdown();

		static void ReloadAppAssembly();
		
		static bool ClassExists(const std::string& moduleName);

		static Shared<ScriptInstance> GetScriptInstance(Entity entity);
		static Shared<ScriptInstance> GetScriptInstance(const UUID& sceneID, const UUID& entityID);
		static Shared<ScriptInstance> CreateScriptInstance(Entity entity);
		static void UninitalizeScriptable(Entity entity);

		static void OnStart(Entity entity);
		static void OnUpdate(Entity entity, float deltaTime);

		static void OnPhysicsBeginContact(Entity collider, Entity collidee);
		static void OnPhysicsEndContact(Entity collider, Entity collidee);
		static void OnPhysicsUpdate(Entity entity);
		
		static const void* CreateEntityInstance(const UUID& id);
		static int32_t GetEntityIDFieldHandle();

		static const void* CreateComponentInstance(int32_t componentTypeId, const UUID& entityId);

        static bool LoadAppAssembly();

	private:
		static bool LoadCoreAssembly();		
		static void InitializeTypeConverters();

		friend class ScriptBindings;
	};
}
