#pragma once

#include "ScriptClass.h"

#include "Core/Log.h"
#include "Core/Base.h"

#include "Scene/Scene.h"

namespace TerranEngine 
{
	struct ScriptComponent;
	
	union ScriptFieldData
	{
		double dValue;
		int64_t iValue;
		bool bValue;
		void* ptr;

		// TODO: add string

		operator bool() { return bValue; }

		operator int8_t() { return static_cast<int8_t>(iValue); }
		operator int16_t() { return static_cast<int16_t>(iValue); }
		operator int32_t() { return static_cast<int32_t>(iValue); }
		operator int64_t() { return static_cast<int64_t>(iValue); }

		operator uint8_t() { return static_cast<uint8_t>(iValue); }
		operator uint16_t() { return static_cast<uint16_t>(iValue); }
		operator uint32_t() { return static_cast<uint32_t>(iValue); }
		operator uint64_t() { return static_cast<uint64_t>(iValue); }

		operator float() { return static_cast<float>(dValue); }
		operator double() { return static_cast<double>(dValue); }
		operator const char* () { return static_cast<const char*>(ptr); }
		operator glm::vec2() { return *static_cast<glm::vec2*>(ptr); }
		operator glm::vec3() { return *static_cast<glm::vec3*>(ptr); }
	};

	struct ScriptFieldBackup
	{
		ScriptFieldData Data;
		ScriptFieldType Type;
	};

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

		static void StartScriptable(Entity entity);
		static void UpdateScriptable(Entity entity);

		static void PhysicsBeginContact(Entity collider, Entity collidee);

		static ScriptObject GetScriptInstanceScriptObject(const UUID& sceneUUID, const UUID& entityUUID);

		static void ClearFieldBackupMap();

		static void SetCurrentFieldStates(const UUID& sceneID);
	};
}