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

#include <box2d/box2d.h>

namespace TerranEngine 
{
	// ---- Entity ----
	static bool HasComponent_Internal(MonoArray* entityUUIDArr, MonoString* componentType);
	static void AddComponent_Internal(MonoArray* entityUUIDArr, MonoString* componentType);
	static void RemoveComponent_Internal(MonoArray* entityUUIDArr, MonoString* componentType);
	static MonoObject* GetScriptableComponent_Internal(MonoArray* entityUUIDArr, MonoString* moduleName);

	static MonoArray* FindEntityWithName_Internal(MonoString* monoName);
	static MonoArray* FindEntityWithID_Internal(MonoArray* monoIDArray);

	static void DestroyEntity_Internal(MonoArray* entityUUIDArr);
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

	// ---- Rigidbody 2D ----
	static bool IsFixedRotation_Internal(MonoArray* entityUUIDArr);
	static void SetFixedRotation_Internal(MonoArray* entityUUIDArr, bool fixedRotation);

	static uint8_t GetAwakeState_Internal(MonoArray* entityUUIDArr);
	static void SetAwakeState_Internal(MonoArray* entityUUIDArr, uint8_t awakeState);

	static float GetGravityScale_Internal(MonoArray* entityUUIDArr);
	static void SetGravityScale_Internal(MonoArray* entityUUIDArr, float gravityScale);

	static void ApplyForce_Internal(MonoArray* entityUUIDArr, glm::vec2 force, glm::vec2 position);
	static void ApplyForceAtCenter_Internal(MonoArray* entityUUIDArr, glm::vec2 force);
	// ----------------------

	// ---- Box Collider 2D ----
	static void GetBoxOffset_Internal(MonoArray* entityUUIDArr, glm::vec2& offset);
	static void SetBoxOffset_Internal(MonoArray* entityUUIDArr, glm::vec2 offset);

	static void GetBoxSize_Internal(MonoArray* entityUUIDArr, glm::vec2& size);
	static void SetBoxSize_Internal(MonoArray* entityUUIDArr, glm::vec2 size);

	static bool IsBoxSensor_Internal(MonoArray* entityUUIDArr);
	static void SetBoxSensor_Internal(MonoArray* entityUUIDArr, bool isSensor);
	// -------------------------

	// ---- Circle Collider 2D ----
	static void GetCircleOffset_Internal(MonoArray* entityUUIDArr, glm::vec2& offset);
	static void SetCircleOffset_Internal(MonoArray* entityUUIDArr, glm::vec2 offset);

	static float GetCircleRadius_Internal(MonoArray* entityUUIDArr);
	static void SetCircleRadius_Internal(MonoArray* entityUUIDArr, float radius);

	static bool IsCircleSensor_Internal(MonoArray* entityUUIDArr);
	static void SetCircleSensor_Internal(MonoArray* entityUUIDArr, bool isSensor);
	// ----------------------------
	
	// ------------------

	// ---- Utils ----
	static void Log_Internal(uint8_t logLevel, MonoString* monoMessage);
	// ---------------

	// ---- Input ----
	static bool KeyPressed_Internal(uint32_t keyCode);
	static bool MouseButtonPressed_Internal(uint16_t mouseButton);
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

		BindInternalFunc("TerranScriptCore.Entity::DestroyEntity_Internal", DestroyEntity_Internal);
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
		{
			// ---- rigidbody 2d ----
			BindInternalFunc("TerranScriptCore.Rigidbody2D::IsFixedRotation_Internal", IsFixedRotation_Internal);
			BindInternalFunc("TerranScriptCore.Rigidbody2D::SetFixedRotation_Internal", SetFixedRotation_Internal);

			BindInternalFunc("TerranScriptCore.Rigidbody2D::GetAwakeState_Internal", GetAwakeState_Internal);
			BindInternalFunc("TerranScriptCore.Rigidbody2D::SetAwakeState_Internal", SetAwakeState_Internal);

			BindInternalFunc("TerranScriptCore.Rigidbody2D::GetGravityScale_Internal", GetGravityScale_Internal);
			BindInternalFunc("TerranScriptCore.Rigidbody2D::SetGravityScale_Internal", SetGravityScale_Internal);

			BindInternalFunc("TerranScriptCore.Rigidbody2D::ApplyForce_Internal", ApplyForce_Internal);
			BindInternalFunc("TerranScriptCore.Rigidbody2D::ApplyForceAtCenter_Internal", ApplyForceAtCenter_Internal);
			// ----------------------

			// ---- box collider 2d ----
			BindInternalFunc("TerranScriptCore.BoxCollider2D::GetOffset_Internal", GetBoxOffset_Internal);
			BindInternalFunc("TerranScriptCore.BoxCollider2D::SetOffset_Internal", SetBoxOffset_Internal);

			BindInternalFunc("TerranScriptCore.BoxCollider2D::GetSize_Internal", GetBoxSize_Internal);
			BindInternalFunc("TerranScriptCore.BoxCollider2D::SetSize_Internal", SetBoxSize_Internal);

			BindInternalFunc("TerranScriptCore.BoxCollider2D::IsSensor_Internal", IsBoxSensor_Internal);
			BindInternalFunc("TerranScriptCore.BoxCollider2D::SetSensor_Internal", SetBoxSensor_Internal);
			// -------------------------

			// ---- circle collider 2d ----
			BindInternalFunc("TerranScriptCore.CircleCollider2D::GetOffset_Internal", GetCircleOffset_Internal);
			BindInternalFunc("TerranScriptCore.CircleCollider2D::SetOffset_Internal", SetCircleOffset_Internal);

			BindInternalFunc("TerranScriptCore.CircleCollider2D::GetRadius_Internal", GetCircleRadius_Internal);
			BindInternalFunc("TerranScriptCore.CircleCollider2D::SetRadius_Internal", SetCircleRadius_Internal);

			BindInternalFunc("TerranScriptCore.CircleCollider2D::IsSensor_Internal", IsCircleSensor_Internal);
			BindInternalFunc("TerranScriptCore.CircleCollider2D::SetSensor_Internal", SetCircleSensor_Internal);

			// -----------------------------
		}

		// -----------------

		// ---- misc ----
		BindInternalFunc("TerranScriptCore.Log::Log_Internal", Log_Internal);
		// --------------

		// ---- input -----
		BindInternalFunc("TerranScriptCore.Input::KeyPressed_Internal", KeyPressed_Internal);
		BindInternalFunc("TerranScriptCore.Input::MouseButtonPressed_Internal", MouseButtonPressed_Internal);
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
		case ComponentType::TransformComponent:			entity.RemoveComponent<TransformComponent>(); break;
		case ComponentType::TagComponent:				entity.RemoveComponent<TagComponent>(); break;
		case ComponentType::ScriptableComponent:		entity.RemoveComponent<ScriptComponent>();  break;
		case ComponentType::Rigibody2DComponent:		entity.RemoveComponent<Rigidbody2DComponent>(); break;
		case ComponentType::BoxCollider2DComponent:		entity.RemoveComponent<BoxCollider2DComponent>(); break;
		case ComponentType::CircleCollider2DComponent:	entity.RemoveComponent<CircleCollider2DComponent>(); break;
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

	static void DestroyEntity_Internal(MonoArray* entityUUIDArr) 
	{
		UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);

		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

		if (entity)
			SceneManager::GetCurrentScene()->DestroyEntity(entity, true);
		else
			TR_CLIENT_ERROR("Can't destroy entity because it doesnt exist");
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

	static void ApplyForce_Internal(MonoArray* entityUUIDArr, glm::vec2 force, glm::vec2 position)
	{
		void* RuntimeBody = nullptr;

		GET_COMPONENT_VAR(RuntimeBody, entityUUIDArr, Rigidbody2DComponent);

		if (RuntimeBody) 
		{
			b2Body* body = (b2Body*)RuntimeBody;
			body->ApplyForce({ force.x, force.y }, { position.x, position.y }, true);
		}

	}

	static void ApplyForceAtCenter_Internal(MonoArray* entityUUIDArr, glm::vec2 force) 
	{
		void* RuntimeBody = nullptr;

		GET_COMPONENT_VAR(RuntimeBody, entityUUIDArr, Rigidbody2DComponent);

		if (RuntimeBody) 
		{
			b2Body* body = (b2Body*)RuntimeBody;
			body->ApplyForceToCenter({ force.x, force.y }, true);
		}
	}

	static void GetBoxOffset_Internal(MonoArray* entityUUIDArr, glm::vec2& offset)
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		GET_COMPONENT_VAR(Offset, entityUUIDArr, BoxCollider2DComponent);
		offset = Offset;
	}

	static void SetBoxOffset_Internal(MonoArray* entityUUIDArr, glm::vec2 offset)
	{
		glm::vec2 Offset = offset;
		SET_COMPONENT_VAR(Offset, entityUUIDArr, BoxCollider2DComponent);
	}

	static void GetCircleOffset_Internal(MonoArray* entityUUIDArr, glm::vec2& offset) 
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		GET_COMPONENT_VAR(Offset, entityUUIDArr, CircleCollider2DComponent);
		offset = Offset;
	}

	static void SetCircleOffset_Internal(MonoArray* entityUUIDArr, glm::vec2 offset) 
	{
		glm::vec2 Offset = offset;
		SET_COMPONENT_VAR(Offset, entityUUIDArr, CircleCollider2DComponent);
	}

	static void GetBoxSize_Internal(MonoArray* entityUUIDArr, glm::vec2& size) 
	{
		glm::vec2 Size = { 0.0f, 0.0f };
		GET_COMPONENT_VAR(Size, entityUUIDArr, BoxCollider2DComponent);
		size = Size;
	}

	static void SetBoxSize_Internal(MonoArray* entityUUIDArr, glm::vec2 size) 
	{
		glm::vec2 Size = size;
		SET_COMPONENT_VAR(Size, entityUUIDArr, BoxCollider2DComponent);
	}

	static float GetCircleRadius_Internal(MonoArray* entityUUIDArr) 
	{
		float Radius = 0.0f;
		GET_COMPONENT_VAR(Radius, entityUUIDArr, CircleCollider2DComponent);
		return Radius;
	}

	static void SetCircleRadius_Internal(MonoArray* entityUUIDArr, float radius) 
	{
		float Radius = radius;
		SET_COMPONENT_VAR(Radius, entityUUIDArr, CircleCollider2DComponent);
	}

	static bool IsBoxSensor_Internal(MonoArray* entityUUIDArr) 
	{
		bool IsSensor = false;
		GET_COMPONENT_VAR(IsSensor, entityUUIDArr, BoxCollider2DComponent);
		return IsSensor;
	}

	static void SetBoxSensor_Internal(MonoArray* entityUUIDArr, bool isSensor) 
	{
		bool IsSensor = isSensor;
		SET_COMPONENT_VAR(IsSensor, entityUUIDArr, BoxCollider2DComponent);
	}

	static bool IsCircleSensor_Internal(MonoArray* entityUUIDArr) 
	{
		bool IsSensor = false;
		GET_COMPONENT_VAR(IsSensor, entityUUIDArr, CircleCollider2DComponent);
		return IsSensor;
	}

	static void SetCircleSensor_Internal(MonoArray* entityUUIDArr, bool isSensor) 
	{
		bool IsSensor = isSensor;
		SET_COMPONENT_VAR(IsSensor, entityUUIDArr, CircleCollider2DComponent);
	}

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

	static bool KeyPressed_Internal(uint32_t keyCode) 
	{
		return Input::IsKeyPressed((Key)keyCode);
	}

	static bool MouseButtonPressed_Internal(uint16_t mouseButton) 
	{
		return Input::IsMouseButtonPressed((MouseButton)mouseButton);
	}
}
