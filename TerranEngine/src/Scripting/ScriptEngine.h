#pragma once

#include "ScriptClass.h"
#include "GCManager.h"

#include "Core/Base.h"
#include "Core/UUID.h"

#include "Scene/Scene.h"

#include <filesystem>

namespace TerranEngine 
{
#define TR_CORE_ASSEMBLY_INDEX 0
#define TR_APP_ASSEMBLY_INDEX 1
#define TR_ASSEMBLIES ((TR_APP_ASSEMBLY_INDEX) + 1)
	class ScriptAssembly;
	class ScriptEngine 
	{
		using LogFN = std::function<void(std::string, spdlog::level::level_enum)>;
	public:
		static void Initialize(const std::filesystem::path& scriptCoreAssemblyPath);
		static void Shutdown();

		static void ReloadAppAssembly();
		
		static ScriptClass GetClassFromName(const std::string& moduleName, int assemblyIndex);
		static ScriptClass GetClassFromTypeToken(uint32_t typeToken, int assemblyIndex);
		
		static ScriptMethod GetMethodFromDesc(const std::string& methodDesc, int assemblyIndex);
		static bool ClassExists(const std::string& moduleName);

		static Shared<ScriptAssembly>& GetAssembly(int assemblyIndex);
		
		static void InitializeScriptable(Entity entity);
		static void UninitalizeScriptable(Entity entity);

		static void OnStart(Entity entity);
		static void OnUpdate(Entity entity, float deltaTime);

		static void OnPhysicsBeginContact(Entity collider, Entity collidee);
		static void OnPhysicsEndContact(Entity collider, Entity collidee);
		
		static void OnPhysicsUpdate(Entity entity);

		static ScriptObject GetScriptInstanceScriptObject(const UUID& sceneUUID, const UUID& entityUUID);
		static GCHandle GetScriptInstanceGCHandle(const UUID& sceneUUID, const UUID& entityUUID);

        static bool LoadAppAssembly();

		static void SetLogCallback(LogFN logCallback);
	private:
		static bool LoadCoreAssembly();
		
		static void CreateAppDomain();
		static void UnloadDomain();
	};
}
