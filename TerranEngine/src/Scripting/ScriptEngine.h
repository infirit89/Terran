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
		using LogFN = std::function<void(std::string_view, spdlog::level::level_enum)>;
		static void Initialize(const std::filesystem::path& scriptCoreAssemblyPath);
		static void Shutdown();

		static void ReloadAppAssembly();
		
		static bool ClassExists(const std::string& moduleName);

		//static Shared<ScriptAssembly> GetAssembly(int assemblyIndex);
		
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

		//static ManagedObject GetScriptInstanceScriptObject(const UUID& sceneUUID, const UUID& entityUUID);
		//static GCHandle GetScriptInstanceGCHandle(const UUID& sceneUUID, const UUID& entityUUID);

		static const void* CreateComponentInstance(int32_t componentTypeId, const UUID& entityId);

        static bool LoadAppAssembly();

		static void SetLogCallback(LogFN logCallback);

		// void OnSceneTransition(const Shared<Scene>& oldScene, const Shared<Scene>& newScene);
		using OnSceneTransitionFn = std::function<void(const Shared<Scene>&, const Shared<Scene>&)>;

		static void SetOnSceneTransition(const OnSceneTransitionFn& sceneTransitionFn);

	private:
		static bool LoadCoreAssembly();
		static void CallSceneTransitionCallback(const Shared<Scene>& oldScene, const Shared<Scene>& newScene);
		
		/*static void CreateAppDomain();
		static void UnloadDomain();*/
		static void InitializeTypeConverters();

		friend class ScriptBindings;
	};
}
