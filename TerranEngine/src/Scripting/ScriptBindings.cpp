#include "trpch.h"
#include "ScriptBindings.h"

#include "ScriptString.h"
#include "ScriptEngine.h"
#include "ScriptMarshal.h"

#include "Core/Input.h"

#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Scene/SceneManager.h"

#include <glm/glm.hpp>

#include <mono/jit/jit.h>
#include <mono/metadata/object.h>

namespace TerranEngine 
{
	// ---- Entity ----
	static bool HasComponent_Internal(MonoArray* entityUUIDArr, MonoString* componentType);
	static void AddComponent_Internal(MonoArray* entityUUIDArr, MonoString* componentType);
	static void RemoveComponent_Internal(MonoArray* entityUUIDArr, MonoString* componentType);
	static MonoObject* GetScriptableComponent_Internal(MonoArray* entityUUIDArr, MonoString* moduleName);

	static MonoArray* FindEntityWithName_Internal(MonoString* monoName);
	static MonoArray* FindEntityWithID_Internal(MonoArray* monoIDArray);
	// ----------------

	// ---- Transform component ----
	static void SetTransformPosition_Internal(MonoArray* entityUUIDArr, glm::vec3 inPosition);
	static glm::vec3 GetTransformPosition_Internal(MonoArray* entityUUIDArr);

	static void SetTransformRotation_Internal(MonoArray* entityUUIDArr, glm::vec3 inRotation);
	static glm::vec3 GetTransformRotation_Internal(MonoArray* entityUUIDArr);

	static void SetTransformScale_Internal(MonoArray* entityUUIDArr, glm::vec3 inScale);
	static glm::vec3 GetTransformScale_Internal(MonoArray* entityUUIDArr);

	static bool IsDirty_Internal(MonoArray* entityUUIDArr);
	// -----------------------------

	// ---- Tag component ----
	static void SetTagName_Internal(MonoArray* entityUUIDArr, MonoString* inName);
	static MonoString* GetTagName_Internal(MonoArray* entityUUIDArr);
	// -----------------------

	// ---- Physics ----
	static bool IsFixedRotation_Internal(MonoArray* entityUUIDArr);
	static void SetFixedRotation_Internal(MonoArray* entityUUIDArr, bool fixedRotation);

	static uint8_t GetAwakeState_Internal(MonoArray* entityUUIDArr);
	static void SetAwakeState_Internal(MonoArray* entityUUIDArr, uint8_t awakeState);

	static float GetGravityScale_Internal(MonoArray* entityUUIDArr);
	static void SetGravityScale_Internal(MonoArray* entityUUIDArr, float gravityScale);
	// ------------------

	// ---- Utils ----
	static void Log_Internal(uint8_t logLevel, MonoString* monoMessage);
	// ---------------

	// ---- Input ----
	static bool KeyPressed_Internal(uint32_t keyCode);
	// ---------------

	template <typename Func>
	static void BindInternalFunc(const char* funcName, Func func)
	{
		mono_add_internal_call(funcName, func);
	}

	void ScriptBindings::Bind()
	{
		// ---- entity -----
		BindInternalFunc("TerranScriptCore.Entity::HasComponent_Internal", HasComponent_Internal);
		BindInternalFunc("TerranScriptCore.Entity::AddComponent_Internal", AddComponent_Internal);
		BindInternalFunc("TerranScriptCore.Entity::RemoveComponent_Internal", RemoveComponent_Internal);
		BindInternalFunc("TerranScriptCore.Entity::GetScriptableComponent_Internal", GetScriptableComponent_Internal);

		BindInternalFunc("TerranScriptCore.Entity::FindEntityWithName_Internal", FindEntityWithName_Internal);
		BindInternalFunc("TerranScriptCore.Entity::FindEntityWithID_Internal", FindEntityWithID_Internal);
		// -----------------

		// ---- transform ----
		BindInternalFunc("TerranScriptCore.Transform::GetTransformPosition_Internal", GetTransformPosition_Internal);
		BindInternalFunc("TerranScriptCore.Transform::SetTransformPosition_Internal", SetTransformPosition_Internal);

		BindInternalFunc("TerranScriptCore.Transform::GetTransformRotation_Internal", GetTransformRotation_Internal);
		BindInternalFunc("TerranScriptCore.Transform::SetTransformRotation_Internal", SetTransformRotation_Internal);

		BindInternalFunc("TerranScriptCore.Transform::GetTransformScale_Internal", GetTransformScale_Internal);
		BindInternalFunc("TerranScriptCore.Transform::SetTransformScale_Internal", SetTransformScale_Internal);
		
		BindInternalFunc("TerranScriptCore.Transform::IsDirty_Internal", IsDirty_Internal);
		// -------------------

		// ---- tag ----
		BindInternalFunc("TerranScriptCore.Tag::GetTagName_Internal", GetTagName_Internal);
		BindInternalFunc("TerranScriptCore.Tag::SetTagName_Internal", SetTagName_Internal);
		// -------------

		// ---- physics ----
		BindInternalFunc("TerranScriptCore.Rigidbody2D::IsFixedRotation_Internal", IsFixedRotation_Internal);
		BindInternalFunc("TerranScriptCore.Rigidbody2D::SetFixedRotation_Internal", SetFixedRotation_Internal);

		BindInternalFunc("TerranScriptCore.Rigidbody2D::GetAwakeState_Internal", GetAwakeState_Internal);
		BindInternalFunc("TerranScriptCore.Rigidbody2D::SetAwakeState_Internal", SetAwakeState_Internal);

		BindInternalFunc("TerranScriptCore.Rigidbody2D::GetGravityScale_Internal", GetGravityScale_Internal);
		BindInternalFunc("TerranScriptCore.Rigidbody2D::SetGravityScale_Internal", SetGravityScale_Internal);
		// -----------------

		// ---- misc ----
		BindInternalFunc("TerranScriptCore.Log::Log_Internal", Log_Internal);
		// --------------

		// ---- input -----
		BindInternalFunc("TerranScriptCore.Input::KeyPressed_Internal", KeyPressed_Internal);
		// ----------------
	}

	enum class ComponentType
	{
		None = 0,
		TransformComponent,
		TagComponent,
		ScriptableComponent,
		Rigibody2DComponent
	};

	static Scene* GetScenePtr() { return SceneManager::GetCurrentScene().get(); }

	static ComponentType GetComponentType(MonoString* componentTypeStr)
	{
		ScriptString string(componentTypeStr);

		if (strcmp(string.GetUTF8Str(), "TerranScriptCore.Transform") == 0)
			return ComponentType::TransformComponent;
		else if (strcmp(string.GetUTF8Str(), "TerranScriptCore.Tag") == 0)
			return ComponentType::TagComponent;
		else if (strcmp(string.GetUTF8Str(), "TerranScriptCore.Rigidbody2D") == 0)
			return ComponentType::Rigibody2DComponent;
		else 
		{
			ScriptClass parent = ScriptEngine::GetClass(string.GetUTF8Str()).GetParent();

			if (strcmp(parent.GetName(), "Scriptable") == 0)
				return ComponentType::ScriptableComponent;
		}

		return ComponentType::None;
	}
	
	// TODO: change this to a MonoType
	static bool HasComponent_Internal(MonoArray* entityUUIDArr, MonoString* componentTypeStr)
	{
		ComponentType type = GetComponentType(componentTypeStr);

		if (!GetScenePtr())
		{
			TR_ERROR("No active scene loaded");
			return false;
		}

		UUID entityUUID = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(entityUUID);

		switch (type)
		{
		case ComponentType::TransformComponent:     return entity.HasComponent<TransformComponent>();
		case ComponentType::TagComponent:           return entity.HasComponent<TagComponent>();
		case ComponentType::Rigibody2DComponent:    return entity.HasComponent<Rigidbody2DComponent>();
		case ComponentType::ScriptableComponent: 
		{
			if (entity.HasComponent<ScriptComponent>()) 
			{
				auto& scComponent = entity.GetComponent<ScriptComponent>();
				ScriptString string(componentTypeStr);

				return scComponent.ModuleName == string.GetUTF8Str();
			}
			return false;
		}  
		}

		return false;
	}

	static void AddComponent_Internal(MonoArray* entityUUIDArr, MonoString* componentTypeStr)
	{
		ComponentType type = GetComponentType(componentTypeStr);

		if (!GetScenePtr())
		{
			TR_ERROR("No active scene loaded");
			return;
		}

		UUID entityUUID = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(entityUUID);

		switch (type)
		{
		case ComponentType::TransformComponent:         entity.AddComponent<TransformComponent>(); break;
		case ComponentType::TagComponent:               entity.AddComponent<TagComponent>(); break;
		case ComponentType::Rigibody2DComponent:        entity.AddComponent<Rigidbody2DComponent>(); break;
		case ComponentType::ScriptableComponent: 
		{
			ScriptString sString(componentTypeStr);
			entity.AddComponent<ScriptComponent>(sString.GetUTF8Str());

			break;
		}
		}
	}

	static void RemoveComponent_Internal(MonoArray* entityUUIDArr, MonoString* componentTypeStr)
	{
		ComponentType type = GetComponentType(componentTypeStr);

		if (!GetScenePtr())
		{
			TR_ERROR("No active scene loaded");
			return;
		}

		UUID entityUUID = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(entityUUID);

		switch (type)
		{
		case ComponentType::TransformComponent:     entity.RemoveComponent<TransformComponent>(); break;
		case ComponentType::TagComponent:           entity.RemoveComponent<TagComponent>(); break;
		case ComponentType::ScriptableComponent:    entity.RemoveComponent<ScriptComponent>();  break;
		case ComponentType::Rigibody2DComponent:    entity.RemoveComponent<Rigidbody2DComponent>(); break;
		}
	}

	static MonoObject* GetScriptableComponent_Internal(MonoArray* entityUUIDArr, MonoString* moduleName) 
	{
		if (!GetScenePtr())
		{
			TR_ERROR("No active scene!");
			return nullptr;
		}

		UUID uuid = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(uuid);
		ScriptObject scriptObject = ScriptEngine::GetScriptInstanceScriptObject(entity.GetSceneID(), entity.GetID());

		return (MonoObject*)scriptObject.GetNativeObject();
	}

	static MonoArray* FindEntityWithName_Internal(MonoString* monoName)
	{
		ScriptString name(monoName);

		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithName(name.GetUTF8Str());

		if (entity) 
		{
			MonoArray* monoArray = ScriptMarshal::UUIDToMonoArray(entity.GetID());
			return monoArray;
		}

		return nullptr;
	}

// bullshit?
#define SET_COMPONENT_VAR(var, entityID, componentType)\
	UUID entityUUID = ScriptMarshal::MonoArrayToUUID(entityID);\
	Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(entityUUID);\
	if(entity)\
		entity.GetComponent<componentType>().var = var;\
	else\
		TR_ERROR("Invalid entity id");

// bullshit #2? 
#define GET_COMPONENT_VAR(var, entityID, componentType)\
	UUID entityUUID = ScriptMarshal::MonoArrayToUUID(entityID);\
	Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(entityUUID);\
	if(entity)\
		var = entity.GetComponent<componentType>().var;\
	else\
		TR_ERROR("Invalid entity id");

	// ---- Transform ----
	static void SetTransformPosition_Internal(MonoArray* entityUUIDArr, glm::vec3 Position)
	{
		SET_COMPONENT_VAR(Position, entityUUIDArr, TransformComponent);

		if (entity)  
			entity.GetComponent<TransformComponent>().IsDirty = true;
	}

	static glm::vec3 GetTransformPosition_Internal(MonoArray* entityUUIDArr)
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		GET_COMPONENT_VAR(Position, entityUUIDArr, TransformComponent);

		return Position;
	}

	static void SetTransformRotation_Internal(MonoArray* entityUUIDArr, glm::vec3 Rotation)
	{
		SET_COMPONENT_VAR(Rotation, entityUUIDArr, TransformComponent);
		
		if(entity)
			entity.GetComponent<TransformComponent>().IsDirty = true;
	}

	static glm::vec3 GetTransformRotation_Internal(MonoArray* entityUUIDArr)
	{
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		GET_COMPONENT_VAR(Rotation, entityUUIDArr, TransformComponent);

		return Rotation;
	}

	static void SetTransformScale_Internal(MonoArray* entityUUIDArr, glm::vec3 Scale)
	{
		SET_COMPONENT_VAR(Scale, entityUUIDArr, TransformComponent);

		if(entity)
			entity.GetComponent<TransformComponent>().IsDirty = true;
	}

	static glm::vec3 GetTransformScale_Internal(MonoArray* entityUUIDArr)
	{
		glm::vec3 Scale = { 0.0f, 0.0f, 0.0f };
		GET_COMPONENT_VAR(Scale, entityUUIDArr, TransformComponent);

		return Scale;
	}

	static bool IsDirty_Internal(MonoArray* entityUUIDArr)
	{
		UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);

		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

		if (entity)
		{
			auto& transform = entity.GetTransform();
			return transform.IsDirty;
		}

		return false;
	}

	// -------------------

	static void SetTagName_Internal(MonoArray* entityUUIDArr, MonoString* inName)
	{
		ScriptString nameStr(inName);
		const char* Name = nameStr.GetUTF8Str();

		SET_COMPONENT_VAR(Name, entityUUIDArr, TagComponent);
	}

	static MonoString* GetTagName_Internal(MonoArray* entityUUIDArr)
	{
		std::string Name = "";
		GET_COMPONENT_VAR(Name, entityUUIDArr, TagComponent);
		return ScriptString(Name.c_str()).GetStringInternal();
	}
	
	static bool IsFixedRotation_Internal(MonoArray* entityUUIDArr) 
	{
		bool FixedRotation = false;
		GET_COMPONENT_VAR(FixedRotation, entityUUIDArr, Rigidbody2DComponent);

		return FixedRotation;
	}

	static void SetFixedRotation_Internal(MonoArray* entityUUIDArr, bool FixedRotation) 
	{
		SET_COMPONENT_VAR(FixedRotation, entityUUIDArr, Rigidbody2DComponent);
	}

	static uint8_t GetAwakeState_Internal(MonoArray* entityUUIDArr) 
	{
		RigidbodyAwakeState AwakeState = RigidbodyAwakeState::Awake;

		GET_COMPONENT_VAR(AwakeState, entityUUIDArr, Rigidbody2DComponent);

		return (uint8_t)AwakeState;
	}

	static void SetAwakeState_Internal(MonoArray* entityUUIDArr, uint8_t awakeState) 
	{
		RigidbodyAwakeState AwakeState = (RigidbodyAwakeState)awakeState;

		SET_COMPONENT_VAR(AwakeState, entityUUIDArr, Rigidbody2DComponent);
	}

	static float GetGravityScale_Internal(MonoArray* entityUUIDArr) 
	{
		float GravityScale = 0.0f;

		GET_COMPONENT_VAR(GravityScale, entityUUIDArr, Rigidbody2DComponent);

		return GravityScale;
	}

	static void SetGravityScale_Internal(MonoArray* entityUUIDArr, float gravityScale) 
	{
		float GravityScale = gravityScale;

		SET_COMPONENT_VAR(GravityScale, entityUUIDArr, Rigidbody2DComponent);
	}


	static void Log_Internal(uint8_t logLevel, MonoString* monoMessage)
	{
		// TODO: for now it logs to the console,
		// should make it log to a ui debug console inside the editor
		ScriptString message(monoMessage);

		switch (logLevel)
		{
		case 1 << 0:
			TR_CLIENT_TRACE(message.GetUTF8Str());
			break;
		case 1 << 1:
			TR_CLIENT_WARN(message.GetUTF8Str());
			break;
		case 1 << 2:
			TR_CLIENT_ERROR(message.GetUTF8Str());
			break;
		}
	}

	static bool KeyPressed_Internal(uint32_t keyCode) 
	{
		return Input::IsKeyPressed((Key)keyCode);
	}

	static MonoArray* FindEntityWithID_Internal(MonoArray* monoIDArray) 
	{
		UUID id = ScriptMarshal::MonoArrayToUUID(monoIDArray);

		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

		if (entity) 
		{
			MonoArray* idArray = ScriptMarshal::UUIDToMonoArray(entity.GetID());
			return idArray;
		}

		return nullptr;
	}
}
