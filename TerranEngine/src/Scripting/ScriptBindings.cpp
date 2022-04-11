#include "trpch.h"
#include "ScriptBindings.h"

#include "ScriptString.h"
#include "ScriptEngine.h"
#include "ScriptMarshal.h"

#include "Core/Input.h"

#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Scene/SceneManager.h"

#include "Physics/Physics.h"
#include "Physics/PhysicsBody.h"

#include <glm/glm.hpp>

#include <mono/jit/jit.h>
#include <mono/metadata/object.h>

#include <box2d/box2d.h>

namespace TerranEngine 
{
	// ---- Entity ----
	static bool Entity_HasComponent_Internal(MonoArray* entityUUIDArr, MonoString* componentType);
	static void Entity_AddComponent_Internal(MonoArray* entityUUIDArr, MonoString* componentType);
	static void Entity_RemoveComponent_Internal(MonoArray* entityUUIDArr, MonoString* componentType);
	static MonoObject* Entity_GetScriptableComponent_Internal(MonoArray* entityUUIDArr, MonoString* moduleName);

	static MonoArray* Entity_FindEntityWithName_Internal(MonoString* monoName);
	static MonoArray* Entity_FindEntityWithID_Internal(MonoArray* monoIDArray);

	static void Entity_DestroyEntity_Internal(MonoArray* entityUUIDArr);
	// ----------------

	// ---- Transform component ----
	static void Transform_SetPosition_Internal(MonoArray* entityUUIDArr, const glm::vec3& inPosition);
	static glm::vec3 Transform_GetPosition_Internal(MonoArray* entityUUIDArr);

	static void Transform_SetRotation_Internal(MonoArray* entityUUIDArr, const glm::vec3& inRotation);
	static glm::vec3 Transform_GetRotation_Internal(MonoArray* entityUUIDArr);

	static void Transform_SetScale_Internal(MonoArray* entityUUIDArr, const glm::vec3& inScale);
	static glm::vec3 Transform_GetScale_Internal(MonoArray* entityUUIDArr);

	static bool Transform_IsDirty_Internal(MonoArray* entityUUIDArr);
	// -----------------------------

	// ---- Tag component ----
	static void Tag_SetName_Internal(MonoArray* entityUUIDArr, MonoString* inName);
	static MonoString* Tag_GetName_Internal(MonoArray* entityUUIDArr);
	// -----------------------

	// ---- Physics ----

	// ---- Rigidbody 2D ----
	static bool Rigidbody_IsFixedRotation_Internal(MonoArray* entityUUIDArr);
	static void Rigidbody_SetFixedRotation_Internal(MonoArray* entityUUIDArr, bool fixedRotation);

	static uint8_t Rigidbody_GetSleepState_Internal(MonoArray* entityUUIDArr);
	static void Rigidbody_SetSleepState_Internal(MonoArray* entityUUIDArr, uint8_t awakeState);

	static float Rigidbody_GetGravityScale_Internal(MonoArray* entityUUIDArr);
	static void Rigidbody_SetGravityScale_Internal(MonoArray* entityUUIDArr, float gravityScale);

	static void Rigidbody_ApplyForce_Internal(MonoArray* entityUUIDArr, const glm::vec2& force, const glm::vec2& position, uint8_t forceMode);
	static void Rigidbody_ApplyForceAtCenter_Internal(MonoArray* entityUUIDArr, const glm::vec2& force, uint8_t forceMode);
	
	static void Rigidbody_GetLinearVelocity_Internal(MonoArray* entityUUIDArr, glm::vec2& linearVelocity);
	static void Rigidbody_SetLinearVelocity_Internal(MonoArray* entityUUIDArr, const glm::vec2& linearVelocity);
	
	static float Rigidbody_GetAngularVelocity_Internal(MonoArray* entityUUIDArr);
	static void Rigidbody_SetAngularVelocity_Internal(MonoArray* entityUUIDArr, float angularVelocity);
	// ----------------------

	// ---- Box Collider 2D ----
	static void BoxCollider_GetOffset_Internal(MonoArray* entityUUIDArr, glm::vec2& offset);
	static void BoxCollider_SetOffset_Internal(MonoArray* entityUUIDArr, const glm::vec2& offset);

	static void BoxCollider_GetSize_Internal(MonoArray* entityUUIDArr, glm::vec2& size);
	static void BoxCollider_SetSize_Internal(MonoArray* entityUUIDArr, const glm::vec2& size);

	static bool BoxCollider_IsSensor_Internal(MonoArray* entityUUIDArr);
	static void BoxCollider_SetSensor_Internal(MonoArray* entityUUIDArr, bool isSensor);
	// -------------------------

	// ---- Circle Collider 2D ----
	static void CircleCollider_GetOffset_Internal(MonoArray* entityUUIDArr, glm::vec2& offset);
	static void CircleCollider_SetOffset_Internal(MonoArray* entityUUIDArr, const glm::vec2& offset);

	static float CircleCollider_GetRadius_Internal(MonoArray* entityUUIDArr);
	static void CircleCollider_SetRadius_Internal(MonoArray* entityUUIDArr, float radius);

	static bool CircleCollider_IsSensor_Internal(MonoArray* entityUUIDArr);
	static void CircleCollider_SetSensor_Internal(MonoArray* entityUUIDArr, bool isSensor);
	// ----------------------------
	
	// ------------------

	// ---- Utils ----
	static void Log_Internal(uint8_t logLevel, MonoString* monoMessage);
	// ---------------

	// ---- Input ----
	static bool Input_KeyPressed_Internal(uint32_t keyCode);

	static bool Input_MouseButtonPressed_Internal(uint16_t mouseButton);
	static void Input_GetMousePosition_Internal(glm::vec2& outMousePosition);

	static bool Input_IsControllerConnected_Internal(uint8_t controllerIndex);
	static MonoString* Input_GetControllerName_Internal(uint8_t controllerIndex);
	static bool Input_IsControllerButtonPressed_Internal(uint8_t controllerButton, uint8_t controllerIndex);
	static float Input_GetControllerAxis_Internal(uint8_t controllerAxis, uint8_t controllerIndex);
	// ---------------

	template <typename Func>
	static void BindInternalFunc(const char* funcName, Func func)
	{
		mono_add_internal_call(funcName, func);
	}

	void ScriptBindings::Bind()
	{
		// ---- entity -----
		BindInternalFunc("TerranScriptCore.Internal::Entity_HasComponent_Internal", Entity_HasComponent_Internal);
		BindInternalFunc("TerranScriptCore.Internal::Entity_AddComponent_Internal", Entity_AddComponent_Internal);
		BindInternalFunc("TerranScriptCore.Internal::Entity_RemoveComponent_Internal", Entity_RemoveComponent_Internal);
												   
		BindInternalFunc("TerranScriptCore.Internal::Entity_GetScriptableComponent_Internal", Entity_GetScriptableComponent_Internal);
												   
		BindInternalFunc("TerranScriptCore.Internal::Entity_FindEntityWithName_Internal", Entity_FindEntityWithName_Internal);
		BindInternalFunc("TerranScriptCore.Internal::Entity_FindEntityWithID_Internal", Entity_FindEntityWithID_Internal);
												   
		BindInternalFunc("TerranScriptCore.Internal::Entity_DestroyEntity_Internal", Entity_DestroyEntity_Internal);
		// -----------------

		// ---- transform ----
		BindInternalFunc("TerranScriptCore.Internal::Transform_GetPosition_Internal", Transform_GetPosition_Internal);
		BindInternalFunc("TerranScriptCore.Internal::Transform_SetPosition_Internal", Transform_SetPosition_Internal);
												   
		BindInternalFunc("TerranScriptCore.Internal::Transform_GetRotation_Internal", Transform_GetRotation_Internal);
		BindInternalFunc("TerranScriptCore.Internal::Transform_SetRotation_Internal", Transform_SetRotation_Internal);
												   
		BindInternalFunc("TerranScriptCore.Internal::Transform_GetScale_Internal", Transform_GetScale_Internal);
		BindInternalFunc("TerranScriptCore.Internal::Transform_SetScale_Internal", Transform_SetScale_Internal);
												   
		BindInternalFunc("TerranScriptCore.Internal::Transform_IsDirty_Internal", Transform_IsDirty_Internal);
		// -------------------

		// ---- tag ----
		BindInternalFunc("TerranScriptCore.Internal::Tag_GetName_Internal", Tag_GetName_Internal);
		BindInternalFunc("TerranScriptCore.Internal::Tag_SetName_Internal", Tag_SetName_Internal);
		// -------------

		// ---- physics ----
		{
			// ---- rigidbody 2d ----
			BindInternalFunc("TerranScriptCore.Internal::Rigidbody2D_IsFixedRotation_Internal", Rigidbody_IsFixedRotation_Internal);
			BindInternalFunc("TerranScriptCore.Internal::Rigidbody2D_SetFixedRotation_Internal", Rigidbody_SetFixedRotation_Internal);
													   
			BindInternalFunc("TerranScriptCore.Internal::Rigidbody2D_GetAwakeState_Internal", Rigidbody_GetSleepState_Internal);
			BindInternalFunc("TerranScriptCore.Internal::Rigidbody2D_SetAwakeState_Internal", Rigidbody_SetSleepState_Internal);
													   
			BindInternalFunc("TerranScriptCore.Internal::Rigidbody2D_GetGravityScale_Internal", Rigidbody_GetGravityScale_Internal);
			BindInternalFunc("TerranScriptCore.Internal::Rigidbody2D_SetGravityScale_Internal", Rigidbody_SetGravityScale_Internal);
													   
			BindInternalFunc("TerranScriptCore.Internal::Rigidbody2D_ApplyForce_Internal", Rigidbody_ApplyForce_Internal);
			BindInternalFunc("TerranScriptCore.Internal::Rigidbody2D_ApplyForceAtCenter_Internal", Rigidbody_ApplyForceAtCenter_Internal);

			BindInternalFunc("TerranScriptCore.Internal::Rigidbody2D_GetLinearVelocity_Internal", Rigidbody_GetLinearVelocity_Internal);
			BindInternalFunc("TerranScriptCore.Internal::Rigidbody2D_SetLinearVelocity_Internal", Rigidbody_SetLinearVelocity_Internal);

			BindInternalFunc("TerranScriptCore.Internal::Rigidbody2D_GetAngularVelocity_Internal", Rigidbody_GetAngularVelocity_Internal);
			BindInternalFunc("TerranScriptCore.Internal::Rigidbody2D_SetAngularVelocity_Internal", Rigidbody_SetAngularVelocity_Internal);
			// ----------------------

			// ---- box collider 2d ----
			BindInternalFunc("TerranScriptCore.Internal::BoxCollider2D_GetOffset_Internal", BoxCollider_GetOffset_Internal);
			BindInternalFunc("TerranScriptCore.Internal::BoxCollider2D_SetOffset_Internal", BoxCollider_SetOffset_Internal);
													   
			BindInternalFunc("TerranScriptCore.Internal::BoxCollider2D_GetSize_Internal", BoxCollider_GetSize_Internal);
			BindInternalFunc("TerranScriptCore.Internal::BoxCollider2D_SetSize_Internal", BoxCollider_SetSize_Internal);
													   
			BindInternalFunc("TerranScriptCore.Internal::BoxCollider2D_IsSensor_Internal", BoxCollider_IsSensor_Internal);
			BindInternalFunc("TerranScriptCore.Internal::BoxCollider2D_SetSensor_Internal", BoxCollider_SetSensor_Internal);
			// -------------------------

			// ---- circle collider 2d ----
			BindInternalFunc("TerranScriptCore.Internal::CircleCollider2D_GetOffset_Internal", CircleCollider_GetOffset_Internal);
			BindInternalFunc("TerranScriptCore.Internal::CircleCollider2D_SetOffset_Internal", CircleCollider_SetOffset_Internal);
													   
			BindInternalFunc("TerranScriptCore.Internal::CircleCollider2D_GetRadius_Internal", CircleCollider_GetRadius_Internal);
			BindInternalFunc("TerranScriptCore.Internal::CircleCollider2D_SetRadius_Internal", CircleCollider_SetRadius_Internal);

			BindInternalFunc("TerranScriptCore.Internal::CircleCollider2D_IsSensor_Internal", CircleCollider_IsSensor_Internal);
			BindInternalFunc("TerranScriptCore.Internal::CircleCollider2D_SetSensor_Internal", CircleCollider_SetSensor_Internal);

			// -----------------------------
		}

		// -----------------

		// ---- misc ----
		BindInternalFunc("TerranScriptCore.Internal::Log_Internal", Log_Internal);
		// --------------

		// ---- input -----
		BindInternalFunc("TerranScriptCore.Internal::Input_KeyPressed_Internal", Input_KeyPressed_Internal);
												   
		BindInternalFunc("TerranScriptCore.Internal::Input_MouseButtonPressed_Internal", Input_MouseButtonPressed_Internal);
		BindInternalFunc("TerranScriptCore.Internal::Input_GetMousePosition_Internal", Input_GetMousePosition_Internal);
												   
		BindInternalFunc("TerranScriptCore.Internal::Input_IsControllerConnected_Internal", Input_IsControllerConnected_Internal);
		BindInternalFunc("TerranScriptCore.Internal::Input_GetControllerName_Internal", Input_GetControllerName_Internal);
												   
		BindInternalFunc("TerranScriptCore.Internal::Input_IsControllerButtonPressed_Internal", Input_IsControllerButtonPressed_Internal);
		BindInternalFunc("TerranScriptCore.Internal::Input_GetControllerAxis_Internal", Input_GetControllerAxis_Internal);
		// ----------------
	}

	enum class ComponentType
	{
		None = 0,
		TransformComponent,
		TagComponent,
		ScriptableComponent,
		Rigibody2DComponent,
		Collider2DComponent,
		BoxCollider2DComponent,
		CircleCollider2DComponent
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
		else if (strcmp(string.GetUTF8Str(), "TerranScriptCore.Collider2D") == 0)
			return ComponentType::Collider2DComponent;
		else if (strcmp(string.GetUTF8Str(), "TerranScriptCore.BoxCollider2D") == 0)
			return ComponentType::BoxCollider2DComponent;
		else if (strcmp(string.GetUTF8Str(), "TerranScriptCore.CircleCollider2D") == 0)
			return ComponentType::CircleCollider2DComponent;
		else 
		{
			ScriptClass parent = ScriptEngine::GetClass(string.GetUTF8Str()).GetParent();

			if (strcmp(parent.GetName(), "Scriptable") == 0)
				return ComponentType::ScriptableComponent;
		}

		return ComponentType::None;
	}

	// ---- Entity Utils ----
	static bool Entity_HasComponent_Internal(MonoArray* entityUUIDArr, MonoString* componentTypeStr)
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
		case ComponentType::TransformComponent:			return entity.HasComponent<TransformComponent>();
		case ComponentType::TagComponent:				return entity.HasComponent<TagComponent>();
		case ComponentType::Rigibody2DComponent:		return entity.HasComponent<Rigidbody2DComponent>();
		case ComponentType::Collider2DComponent:		return entity.HasComponent<BoxCollider2DComponent>() || entity.HasComponent<CircleCollider2DComponent>();
		case ComponentType::BoxCollider2DComponent:		return entity.HasComponent<BoxCollider2DComponent>();
		case ComponentType::CircleCollider2DComponent:	return entity.HasComponent<CircleCollider2DComponent>();
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

	static void Entity_AddComponent_Internal(MonoArray* entityUUIDArr, MonoString* componentTypeStr)
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
		case ComponentType::BoxCollider2DComponent:		entity.AddComponent<BoxCollider2DComponent>(); break;
		case ComponentType::CircleCollider2DComponent:	entity.AddComponent<CircleCollider2DComponent>(); break;
		case ComponentType::ScriptableComponent: 
		{
			ScriptString sString(componentTypeStr);
			entity.AddComponent<ScriptComponent>(sString.GetUTF8Str());

			break;
		}
		}
	}

	static void Entity_RemoveComponent_Internal(MonoArray* entityUUIDArr, MonoString* componentTypeStr)
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
		case ComponentType::TransformComponent:			entity.RemoveComponent<TransformComponent>(); break;
		case ComponentType::TagComponent:				entity.RemoveComponent<TagComponent>(); break;
		case ComponentType::ScriptableComponent:		entity.RemoveComponent<ScriptComponent>();  break;
		case ComponentType::Rigibody2DComponent:		entity.RemoveComponent<Rigidbody2DComponent>(); break;
		case ComponentType::BoxCollider2DComponent:		entity.RemoveComponent<BoxCollider2DComponent>(); break;
		case ComponentType::CircleCollider2DComponent:	entity.RemoveComponent<CircleCollider2DComponent>(); break;
		}
	}

	static MonoObject* Entity_GetScriptableComponent_Internal(MonoArray* entityUUIDArr, MonoString* moduleName)
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

	static MonoArray* Entity_FindEntityWithID_Internal(MonoArray* monoIDArray)
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

	static MonoArray* Entity_FindEntityWithName_Internal(MonoString* monoName)
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

	static void Entity_DestroyEntity_Internal(MonoArray* entityUUIDArr)
	{
		UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);

		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

		if (entity)
			SceneManager::GetCurrentScene()->DestroyEntity(entity, true);
		else
			TR_CLIENT_ERROR("Can't destroy entity because it doesnt exist");
	}
	// ----------------------

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
	static void Transform_SetPosition_Internal(MonoArray* entityUUIDArr, const glm::vec3& Position)
	{
		SET_COMPONENT_VAR(Position, entityUUIDArr, TransformComponent);

		if (entity)  
			entity.GetComponent<TransformComponent>().IsDirty = true;
	}

	static glm::vec3 Transform_GetPosition_Internal(MonoArray* entityUUIDArr)
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		GET_COMPONENT_VAR(Position, entityUUIDArr, TransformComponent);

		return Position;
	}

	static void Transform_SetRotation_Internal(MonoArray* entityUUIDArr, const glm::vec3& Rotation)
	{
		SET_COMPONENT_VAR(Rotation, entityUUIDArr, TransformComponent);
		
		if(entity)
			entity.GetComponent<TransformComponent>().IsDirty = true;
	}

	static glm::vec3 Transform_GetRotation_Internal(MonoArray* entityUUIDArr)
	{
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		GET_COMPONENT_VAR(Rotation, entityUUIDArr, TransformComponent);

		return Rotation;
	}

	static void Transform_SetScale_Internal(MonoArray* entityUUIDArr, const glm::vec3& Scale)
	{
		SET_COMPONENT_VAR(Scale, entityUUIDArr, TransformComponent);

		if(entity)
			entity.GetComponent<TransformComponent>().IsDirty = true;
	}

	static glm::vec3 Transform_GetScale_Internal(MonoArray* entityUUIDArr)
	{
		glm::vec3 Scale = { 0.0f, 0.0f, 0.0f };
		GET_COMPONENT_VAR(Scale, entityUUIDArr, TransformComponent);

		return Scale;
	}

	static bool Transform_IsDirty_Internal(MonoArray* entityUUIDArr)
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

	// ---- Tag ----
	static void Tag_SetName_Internal(MonoArray* entityUUIDArr, MonoString* inName)
	{
		ScriptString nameStr(inName);
		const char* Name = nameStr.GetUTF8Str();

		SET_COMPONENT_VAR(Name, entityUUIDArr, TagComponent);
	}

	static MonoString* Tag_GetName_Internal(MonoArray* entityUUIDArr)
	{
		std::string Name = "";
		GET_COMPONENT_VAR(Name, entityUUIDArr, TagComponent);
		return ScriptString(Name.c_str()).GetStringInternal();
	}
	// -------------
	
	// ---- Rigidbody 2D ----
	static bool Rigidbody_IsFixedRotation_Internal(MonoArray* entityUUIDArr) 
	{
		UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);
		if (entity) 
		{
			PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
			return physicsBody.GetFixedRotation();
		}

		return false;
	}

	static void Rigidbody_SetFixedRotation_Internal(MonoArray* entityUUIDArr, bool fixedRotation)
	{
		bool FixedRotation = fixedRotation;
		SET_COMPONENT_VAR(FixedRotation, entityUUIDArr, Rigidbody2DComponent);

		if (entity) 
		{
			PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
			physicsBody.SetFixedRotation(FixedRotation);
		}
	}

	static uint8_t Rigidbody_GetSleepState_Internal(MonoArray* entityUUIDArr)
	{
		UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

		if (entity) 
		{
			PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
			return (uint8_t)physicsBody.GetSleepState();
		}

		return (uint8_t)PhysicsBodySleepState::Awake;
	}

	static void Rigidbody_SetSleepState_Internal(MonoArray* entityUUIDArr, uint8_t sleepState)
	{
		PhysicsBodySleepState SleepState = (PhysicsBodySleepState)sleepState;
		SET_COMPONENT_VAR(SleepState, entityUUIDArr, Rigidbody2DComponent);

		if (entity) 
		{
			PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
			physicsBody.SetSleepState(SleepState);
		}
	}

	static float Rigidbody_GetGravityScale_Internal(MonoArray* entityUUIDArr)
	{
		float GravityScale = 0.0f;

		UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

		if (entity) 
		{
			PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
			return physicsBody.GetGravityScale();
		}

		return GravityScale;
	}

	static void Rigidbody_SetGravityScale_Internal(MonoArray* entityUUIDArr, float gravityScale)
	{
		float GravityScale = gravityScale;

		SET_COMPONENT_VAR(GravityScale, entityUUIDArr, Rigidbody2DComponent);

		if (entity) 
		{
			PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
			physicsBody.SetGravityScale(GravityScale);
		}
	}

	static void Rigidbody_ApplyForce_Internal(MonoArray* entityUUIDArr, const glm::vec2& force, const glm::vec2& position, uint8_t forceMode)
	{
		UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

		if (entity) 
		{
			PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
			physicsBody.ApplyForce(force, position, (ForceMode2D)forceMode);
		}
	}

	static void Rigidbody_ApplyForceAtCenter_Internal(MonoArray* entityUUIDArr, const glm::vec2& force, uint8_t forceMode)
	{
		UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);
		
		if (entity) 
		{
			PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
			physicsBody.ApplyForceAtCenter(force, (ForceMode2D)forceMode);
		}
	}

	static void Rigidbody_GetLinearVelocity_Internal(MonoArray* entityUUIDArr, glm::vec2& linearVelocity) 
	{
		UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

		if (entity) 
		{
			PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
			linearVelocity = physicsBody.GetLinearVelocity();
			return;
		}

		linearVelocity = { 0.0f, 0.0f };
	}

	static void Rigidbody_SetLinearVelocity_Internal(MonoArray* entityUUIDArr, const glm::vec2& linearVelocity) 
	{
		UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

		if (entity) 
		{
			PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
			physicsBody.SetLinearVelocity(linearVelocity);
		}
	}

	static float Rigidbody_GetAngularVelocity_Internal(MonoArray* entityUUIDArr) 
	{
		UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

		if (entity) 
		{
			PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
			return physicsBody.GetAngularVelocity();
		}

		return 0.0f;
	}

	static void Rigidbody_SetAngularVelocity_Internal(MonoArray* entityUUIDArr, float angularVelocity) 
	{
		UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

		if (entity)
		{
			PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
			physicsBody.SetAngularVelocity(angularVelocity);
		}
	}

	// ----------------------

	// ---- Box Collider 2D ----
	static void BoxCollider_GetOffset_Internal(MonoArray* entityUUIDArr, glm::vec2& offset)
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		GET_COMPONENT_VAR(Offset, entityUUIDArr, BoxCollider2DComponent);
		offset = Offset;
	}

	static void BoxCollider_SetOffset_Internal(MonoArray* entityUUIDArr, const glm::vec2& offset)
	{
		glm::vec2 Offset = offset;
		//SET_COMPONENT_VAR(Offset, entityUUIDArr, BoxCollider2DComponent);
	}

	static void BoxCollider_GetSize_Internal(MonoArray* entityUUIDArr, glm::vec2& size)
	{
		glm::vec2 Size = { 0.0f, 0.0f };
		GET_COMPONENT_VAR(Size, entityUUIDArr, BoxCollider2DComponent);
		size = Size;
	}

	static void BoxCollider_SetSize_Internal(MonoArray* entityUUIDArr, const glm::vec2& size)
	{
		glm::vec2 Size = size;
		//SET_COMPONENT_VAR(Size, entityUUIDArr, BoxCollider2DComponent);
	}

	static bool BoxCollider_IsSensor_Internal(MonoArray* entityUUIDArr)
	{
		bool IsSensor = false;
		GET_COMPONENT_VAR(IsSensor, entityUUIDArr, BoxCollider2DComponent);
		return IsSensor;
	}

	static void BoxCollider_SetSensor_Internal(MonoArray* entityUUIDArr, bool isSensor)
	{
		bool IsSensor = isSensor;
		//		SET_COMPONENT_VAR(IsSensor, entityUUIDArr, BoxCollider2DComponent);
	}
	// -------------------------

	// ---- Circle Collider 2D ----
	static void CircleCollider_GetOffset_Internal(MonoArray* entityUUIDArr, glm::vec2& offset)
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		GET_COMPONENT_VAR(Offset, entityUUIDArr, CircleCollider2DComponent);
		offset = Offset;
	}

	static void CircleCollider_SetOffset_Internal(MonoArray* entityUUIDArr, const glm::vec2& offset)
	{
		glm::vec2 Offset = offset;
		//SET_COMPONENT_VAR(Offset, entityUUIDArr, CircleCollider2DComponent);
	}

	static float CircleCollider_GetRadius_Internal(MonoArray* entityUUIDArr) 
	{
		float Radius = 0.0f;
		GET_COMPONENT_VAR(Radius, entityUUIDArr, CircleCollider2DComponent);
		return Radius;
	}

	static void CircleCollider_SetRadius_Internal(MonoArray* entityUUIDArr, float radius) 
	{
		float Radius = radius;
		//SET_COMPONENT_VAR(Radius, entityUUIDArr, CircleCollider2DComponent);
	}

	static bool CircleCollider_IsSensor_Internal(MonoArray* entityUUIDArr) 
	{
		bool IsSensor = false;
		GET_COMPONENT_VAR(IsSensor, entityUUIDArr, CircleCollider2DComponent);
		return IsSensor;
	}

	static void CircleCollider_SetSensor_Internal(MonoArray* entityUUIDArr, bool isSensor) 
	{
		bool IsSensor = isSensor;
//		SET_COMPONENT_VAR(IsSensor, entityUUIDArr, CircleCollider2DComponent);
	}
	// ----------------------------

	// ---- Log ----
	static void Log_Internal(uint8_t logLevel, MonoString* monoMessage)
	{
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
	// -------------

	// ---- Input ----
	static bool Input_KeyPressed_Internal(uint32_t keyCode)
	{
		return Input::IsKeyPressed((Key)keyCode);
	}

	static bool Input_MouseButtonPressed_Internal(uint16_t mouseButton)
	{
		return Input::IsMouseButtonPressed((MouseButton)mouseButton);
	}

	static void Input_GetMousePosition_Internal(glm::vec2& outMousePosition)
	{
		outMousePosition = Input::GetMousePos();
	}

	static bool Input_IsControllerConnected_Internal(uint8_t controllerIndex)
	{
		return Input::IsControllerConnected((ControllerIndex)controllerIndex);
	}

	static MonoString* Input_GetControllerName_Internal(uint8_t controllerIndex)
	{
		const char* controllerName = Input::GetControllerName((ControllerIndex)controllerIndex);
		ScriptString string(controllerName);

		return string.GetStringInternal();
	}

	static bool Input_IsControllerButtonPressed_Internal(uint8_t controllerButton, uint8_t controllerIndex) 
	{
		return Input::IsControllerButtonPressed((ControllerButton)controllerButton, (ControllerIndex)controllerIndex);
	}

	static float Input_GetControllerAxis_Internal(uint8_t controllerAxis, uint8_t controllerIndex) 
	{
		return Input::GetControllerAxis((ControllerAxis)controllerAxis, (ControllerIndex)controllerIndex);
	}
	// ---------------
}
