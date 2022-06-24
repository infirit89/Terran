#include "trpch.h"
#include "ScriptBindings.h"

#include "ScriptEngine.h"
#include "ScriptMarshal.h"
#include "GCManager.h"
#include "ScriptCache.h"
#include "ScriptArray.h"
#include "ScriptObject.h"
#include "ScriptMethodThunks.h"

#include "Core/Input.h"

#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Scene/SceneManager.h"

#include "Physics/Physics.h"
#include "Physics/PhysicsBody.h"
#include "Physics/Collider.h"

#include <glm/glm.hpp>

#include <mono/jit/jit.h>
#include <mono/metadata/object.h>

#include <box2d/box2d.h>

namespace TerranEngine 
{
	namespace ScriptBindings 
	{				
#define BIND_INTERNAL_FUNC(func) mono_add_internal_call("Terran.Internal::"#func, func);

		static ScriptMethodThunks<MonoArray*> s_IDClassCtor;

		void ScriptBindings::Bind()
		{
			// ---- entity -----
			BIND_INTERNAL_FUNC(Entity_HasComponent);
			BIND_INTERNAL_FUNC(Entity_AddComponent);
			BIND_INTERNAL_FUNC(Entity_RemoveComponent);

			BIND_INTERNAL_FUNC(Entity_GetScriptableComponent);

			BIND_INTERNAL_FUNC(Entity_FindEntityWithName);
			BIND_INTERNAL_FUNC(Entity_FindEntityWithID);

			BIND_INTERNAL_FUNC(Entity_DestroyEntity);

			BIND_INTERNAL_FUNC(Entity_GetChildren);
			// -----------------

			// ---- transform ----
			BIND_INTERNAL_FUNC(Transform_GetPosition);
			BIND_INTERNAL_FUNC(Transform_SetPosition);

			BIND_INTERNAL_FUNC(Transform_GetRotation);
			BIND_INTERNAL_FUNC(Transform_SetRotation);

			BIND_INTERNAL_FUNC(Transform_GetScale);
			BIND_INTERNAL_FUNC(Transform_SetScale);

			BIND_INTERNAL_FUNC(Transform_IsDirty);

			BIND_INTERNAL_FUNC(Transform_GetForward);
			BIND_INTERNAL_FUNC(Transform_GetUp);
			BIND_INTERNAL_FUNC(Transform_GetRight);
			// -------------------

			// ---- tag ----
			BIND_INTERNAL_FUNC(Tag_GetName);
			BIND_INTERNAL_FUNC(Tag_SetName);
			// -------------

			// ---- sprite renderer -----
			BIND_INTERNAL_FUNC(SpriteRenderer_GetColor);
			BIND_INTERNAL_FUNC(SpriteRenderer_SetColor);
			// --------------------------

			// ---- camera ----
			BIND_INTERNAL_FUNC(Camera_IsPrimary);
			BIND_INTERNAL_FUNC(Camera_SetPrimary);
			
			BIND_INTERNAL_FUNC(Camera_GetBackgroundColor);
			BIND_INTERNAL_FUNC(Camera_SetBackgroundColor);
			// ----------------

			// ---- circle renderer ----
			BIND_INTERNAL_FUNC(CircleRenderer_GetThickness);
			BIND_INTERNAL_FUNC(CircleRenderer_SetThickness);
			
			BIND_INTERNAL_FUNC(CircleRenderer_GetColor);
			BIND_INTERNAL_FUNC(CircleRenderer_SetColor);
			// -------------------------
			
			// ---- physics ----
			{
				// ---- physics 2d ----
				BIND_INTERNAL_FUNC(Physics2D_RayCast);
				// --------------------

				// ---- rigidbody 2d ----
				BIND_INTERNAL_FUNC(Rigidbody2D_IsFixedRotation);
				BIND_INTERNAL_FUNC(Rigidbody2D_SetFixedRotation);

				BIND_INTERNAL_FUNC(Rigidbody2D_GetSleepState);
				BIND_INTERNAL_FUNC(Rigidbody2D_SetSleepState);

				BIND_INTERNAL_FUNC(Rigidbody2D_GetGravityScale);
				BIND_INTERNAL_FUNC(Rigidbody2D_SetGravityScale);

				BIND_INTERNAL_FUNC(Rigidbody2D_ApplyForce);
				BIND_INTERNAL_FUNC(Rigidbody2D_ApplyForceAtCenter);

				BIND_INTERNAL_FUNC(Rigidbody2D_GetLinearVelocity);
				BIND_INTERNAL_FUNC(Rigidbody2D_SetLinearVelocity);

				BIND_INTERNAL_FUNC(Rigidbody2D_GetAngularVelocity);
				BIND_INTERNAL_FUNC(Rigidbody2D_SetAngularVelocity);

				BIND_INTERNAL_FUNC(Rigidbody2D_GetType);
				BIND_INTERNAL_FUNC(Rigidbody2D_SetType);
				// ----------------------
				
				// ---- collider 2d ----
				BIND_INTERNAL_FUNC(Collider2D_GetOffset);

				BIND_INTERNAL_FUNC(Collider2D_IsSensor);
				BIND_INTERNAL_FUNC(Collider2D_SetSensor);
				// ---------------------

				// ---- box collider 2d ----
				BIND_INTERNAL_FUNC(BoxCollider2D_GetSize);
				BIND_INTERNAL_FUNC(BoxCollider2D_SetSize);
				// -------------------------

				// ---- circle collider 2d ----
				BIND_INTERNAL_FUNC(CircleCollider2D_GetRadius);
				BIND_INTERNAL_FUNC(CircleCollider2D_SetRadius);
				// -----------------------------
			}

			// -----------------

			// ---- misc ----
			BIND_INTERNAL_FUNC(Log_Log);
			// --------------

			// ---- input -----
			BIND_INTERNAL_FUNC(Input_KeyPressed);

			BIND_INTERNAL_FUNC(Input_MouseButtonPressed);
			BIND_INTERNAL_FUNC(Input_GetMousePosition);

			BIND_INTERNAL_FUNC(Input_IsControllerConnected);
			BIND_INTERNAL_FUNC(Input_GetControllerName);

			BIND_INTERNAL_FUNC(Input_IsControllerButtonPressed);
			BIND_INTERNAL_FUNC(Input_GetControllerAxis);

			BIND_INTERNAL_FUNC(Input_GetConnectedControllers);
			// ----------------

			s_IDClassCtor.SetFromMethod(ScriptCache::GetCachedMethod("Terran.UUID", ":.ctor(byte[])"));
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
			CircleCollider2DComponent,
			SpriteRendererComponent,
			CameraComponent,
			CircleRendererComponent
		};

		static ComponentType GetComponentType(MonoString* componentTypeStr)
		{
			std::string moduleName = ScriptMarshal::MonoStringToUTF8(componentTypeStr);
			ScriptClass* clazz = ScriptCache::GetCachedClassFromName(moduleName);

			if (*clazz == *TR_API_CACHED_CLASS(Transform))								return ComponentType::TransformComponent;
			if (*clazz == *TR_API_CACHED_CLASS(Tag))									return ComponentType::TagComponent;
			if (*clazz == *TR_API_CACHED_CLASS(Rigidbody2D))							return ComponentType::Rigibody2DComponent;
			if (*clazz == *TR_API_CACHED_CLASS(Collider2D))								return ComponentType::Collider2DComponent;
			if (*clazz == *TR_API_CACHED_CLASS(BoxCollider2D))							return ComponentType::BoxCollider2DComponent;
			if (*clazz == *TR_API_CACHED_CLASS(CircleCollider2D))						return ComponentType::CircleCollider2DComponent;
			if (*clazz == *TR_API_CACHED_CLASS(SpriteRenderer))							return ComponentType::SpriteRendererComponent;
			if (*clazz == *TR_API_CACHED_CLASS(Camera))									return ComponentType::CameraComponent;
			if (*clazz == *TR_API_CACHED_CLASS(CircleRenderer))							return ComponentType::CircleRendererComponent;
			if (!clazz && clazz->IsInstanceOf(TR_API_CACHED_CLASS(Scriptable)))	return ComponentType::ScriptableComponent;
		
			return ComponentType::None;
		}

		// ---- Entity Utils ----
		bool Entity_HasComponent(MonoArray* entityUUIDArr, MonoString* componentTypeStr)
		{
			ComponentType type = GetComponentType(componentTypeStr);
			
			Scene* scenePtr = SceneManager::GetCurrentScene()->GetRaw();
			if (!scenePtr)
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
			case ComponentType::ScriptableComponent:		return entity.HasComponent<ScriptComponent>();
			case ComponentType::SpriteRendererComponent:	return entity.HasComponent<SpriteRendererComponent>();
			case ComponentType::CameraComponent:			return entity.HasComponent<CameraComponent>();
			case ComponentType::CircleRendererComponent:	return entity.HasComponent<CircleRendererComponent>();
			}

			return false;
		}

		void Entity_AddComponent(MonoArray* entityUUIDArr, MonoString* componentTypeStr)
		{
			ComponentType type = GetComponentType(componentTypeStr);
			Scene* scenePtr = SceneManager::GetCurrentScene()->GetRaw();
			if (!scenePtr)
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
			case ComponentType::ScriptableComponent:		entity.AddComponent<ScriptComponent>(ScriptMarshal::MonoStringToUTF8(componentTypeStr)); break;
			case ComponentType::SpriteRendererComponent:	entity.AddComponent<SpriteRendererComponent>(); break;
			case ComponentType::CameraComponent:			entity.AddComponent<CameraComponent>(); break;
			case ComponentType::CircleRendererComponent:	entity.AddComponent<CircleRendererComponent>(); break;
			}
		}

		void Entity_RemoveComponent(MonoArray* entityUUIDArr, MonoString* componentTypeStr)
		{
			ComponentType type = GetComponentType(componentTypeStr);
			Scene* scenePtr = SceneManager::GetCurrentScene()->GetRaw();
			if (!scenePtr)
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
			case ComponentType::SpriteRendererComponent:	entity.RemoveComponent<SpriteRendererComponent>(); break;
			case ComponentType::CameraComponent:			entity.RemoveComponent<CameraComponent>(); break;
			case ComponentType::CircleRendererComponent:	entity.RemoveComponent<CircleRendererComponent>(); break;
			}
		}

		MonoObject* Entity_GetScriptableComponent(MonoArray* entityUUIDArr, MonoString* moduleName)
		{
			Scene* scenePtr = SceneManager::GetCurrentScene()->GetRaw();
			if (!scenePtr)
			{
				TR_ERROR("No active scene!");
				return nullptr;
			}

			UUID uuid = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
			Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(uuid);
			GCHandle objectHandle = ScriptEngine::GetScriptInstanceGCHandle(entity.GetSceneID(), entity.GetID());
			return GCManager::GetManagedObject(objectHandle);
		}

		MonoArray* Entity_FindEntityWithID(MonoArray* monoIDArray)
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

		MonoArray* Entity_FindEntityWithName(MonoString* monoEntityName)
		{
			std::string entityName = ScriptMarshal::MonoStringToUTF8(monoEntityName);
			Entity entity = SceneManager::GetCurrentScene()->FindEntityWithName(entityName);

			if (entity)
			{
				MonoArray* monoArray = ScriptMarshal::UUIDToMonoArray(entity.GetID());
				return monoArray;
			}
			return nullptr;
		}

		void Entity_DestroyEntity(MonoArray* entityUUIDArr)
		{
			UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
			Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

			if (entity)
				SceneManager::GetCurrentScene()->DestroyEntity(entity, true);
			else
				TR_CLIENT_ERROR("Can't destroy entity because it doesnt exist");
		}

		// TODO: find a better way to do this
		MonoArray* Entity_GetChildren(MonoArray* entityUUIDArr)
		{
			UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
			Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

			if(!entity)
				return nullptr;

			if(entity.GetChildCount() <= 0)
				return nullptr;
			
			ScriptArray childrenIDs = ScriptArray::Create<UUID>(entity.GetChildCount());
			ScriptClass* idClass = TR_API_CACHED_CLASS(UUID);
			
			int i = 0;
			for (const UUID& id : entity.GetChildren())
			{
				ScriptObject idObject = ScriptObject::CreateInstace(*idClass);
				MonoArray* uuidArray = ScriptMarshal::UUIDToMonoArray(id);

				MonoException* exc = nullptr;
				s_IDClassCtor.Invoke(idObject.GetMonoObject(), uuidArray, &exc);
				childrenIDs.Set(i, idObject.GetMonoObject());
				i++;
			}

			return childrenIDs.GetMonoArray();
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
		void Transform_SetPosition(MonoArray* entityUUIDArr, const glm::vec3& Position)
		{
			SET_COMPONENT_VAR(Position, entityUUIDArr, TransformComponent);
			if (entity)
				entity.GetComponent<TransformComponent>().IsDirty = true;
		}

		glm::vec3 Transform_GetPosition(MonoArray* entityUUIDArr)
		{
			glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
			GET_COMPONENT_VAR(Position, entityUUIDArr, TransformComponent);
			return Position;
		}

		void Transform_SetRotation(MonoArray* entityUUIDArr, const glm::vec3& Rotation)
		{
			SET_COMPONENT_VAR(Rotation, entityUUIDArr, TransformComponent);
			if (entity)
				entity.GetComponent<TransformComponent>().IsDirty = true;
		}

		glm::vec3 Transform_GetRotation(MonoArray* entityUUIDArr)
		{
			glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
			GET_COMPONENT_VAR(Rotation, entityUUIDArr, TransformComponent);
			return Rotation;
		}

		void Transform_SetScale(MonoArray* entityUUIDArr, const glm::vec3& Scale)
		{
			SET_COMPONENT_VAR(Scale, entityUUIDArr, TransformComponent);
			if (entity)
				entity.GetComponent<TransformComponent>().IsDirty = true;
		}

		glm::vec3 Transform_GetScale(MonoArray* entityUUIDArr)
		{
			glm::vec3 Scale = { 0.0f, 0.0f, 0.0f };
			GET_COMPONENT_VAR(Scale, entityUUIDArr, TransformComponent);
			return Scale;
		}

		bool Transform_IsDirty(MonoArray* entityUUIDArr)
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

		glm::vec3 Transform_GetForward(MonoArray* entityUUIDArr)
		{
			glm::vec3 Forward = { 0.0f, 0.0f, 1.0f };
			GET_COMPONENT_VAR(Forward, entityUUIDArr, TransformComponent);
			return Forward;
		}

		glm::vec3 Transform_GetUp(MonoArray* entityUUIDArr)
		{
			glm::vec3 Up = { 0.0f, 1.0f, 0.0f };
			GET_COMPONENT_VAR(Up, entityUUIDArr, TransformComponent);
			return Up;
		}

		glm::vec3 Transform_GetRight(MonoArray* entityUUIDArr)
		{
			glm::vec3 Right = { 1.0f, 0.0f, 0.0f };
			GET_COMPONENT_VAR(Right, entityUUIDArr, TransformComponent);
			return Right;
		}
		// -------------------

		// ---- Sprite Renderer ----
		glm::vec4 SpriteRenderer_GetColor(MonoArray* entityUUIDArr)
		{
			glm::vec4 Color = {0.0f, 0.0f, 0.0f, 1.0f};
			GET_COMPONENT_VAR(Color, entityUUIDArr, SpriteRendererComponent);
			return Color;
		}

		void SpriteRenderer_SetColor(MonoArray* entityUUIDArr, const glm::vec4& color)
		{
			glm::vec4 Color = color;
			SET_COMPONENT_VAR(Color, entityUUIDArr, SpriteRendererComponent);
		}
		// -------------------------

		// ---- Camera ----
		bool Camera_IsPrimary(MonoArray* entityUUIDArr)
		{
			bool Primary = false;
			GET_COMPONENT_VAR(Primary, entityUUIDArr, CameraComponent);
			return Primary;
		}

		void Camera_SetPrimary(MonoArray* entityUUIDArr, bool togglePrimary)
		{
			bool Primary = togglePrimary;
			SET_COMPONENT_VAR(Primary, entityUUIDArr, CameraComponent);
		}

		glm::vec4 Camera_GetBackgroundColor(MonoArray* entityUUIDArr)
		{
			glm::vec4 BackgroundColor = { 0.1f, 0.1f, 0.1f, 1.0f };
			GET_COMPONENT_VAR(BackgroundColor, entityUUIDArr, CameraComponent);
			return BackgroundColor;
		}

		void Camera_SetBackgroundColor(MonoArray* entityUUIDArr, const glm::vec4& color)
		{
			glm::vec4 BackgroundColor = color;
			SET_COMPONENT_VAR(BackgroundColor, entityUUIDArr, CameraComponent);
		}
		// ----------------

		// ---- Circle Renderer ----
		glm::vec4 CircleRenderer_GetColor(MonoArray* entityUUIDArr)
		{
			glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
			GET_COMPONENT_VAR(Color, entityUUIDArr, CircleRendererComponent);
			return Color;
		}

		void CircleRenderer_SetColor(MonoArray* entityUUIDArr, const glm::vec4& color)
		{
			glm::vec4 Color = color;
			SET_COMPONENT_VAR(Color, entityUUIDArr, CircleRendererComponent);
		}

		float CircleRenderer_GetThickness(MonoArray* entityUUIDArr)
		{
			float Thickness = 1.0f;
			GET_COMPONENT_VAR(Thickness, entityUUIDArr, CircleRendererComponent);
			return Thickness;
		}

		void CircleRenderer_SetThickness(MonoArray* entityUUIDArr, float thickness)
		{
			float Thickness = thickness;
			SET_COMPONENT_VAR(Thickness, entityUUIDArr, CircleRendererComponent);
		}
		// -------------------------

		// ---- Tag ----
		void Tag_SetName(MonoArray* entityUUIDArr, MonoString* name)
		{
			std::string Name = ScriptMarshal::MonoStringToUTF8(name);
			SET_COMPONENT_VAR(Name, entityUUIDArr, TagComponent);
		}

		MonoString* Tag_GetName(MonoArray* entityUUIDArr)
		{
			std::string Name = "";
			GET_COMPONENT_VAR(Name, entityUUIDArr, TagComponent);
			return ScriptMarshal::UTF8ToMonoString(Name);
		}
		// -------------

		// ---- Physics 2D ----
		bool Physics2D_RayCast(const glm::vec2& origin, const glm::vec2& direction, float length, RayCastHitInfo2D_Internal& outHitInfo)
		{
			RayCastHitInfo2D hitInfo;
			bool hasHit = Physics2D::RayCast(origin, direction, length, hitInfo);

			outHitInfo.Point = hitInfo.Point;
			outHitInfo.Normal = hitInfo.Normal;
			
			UUID id({ 0 });

			if (hitInfo.PhysicsBody.GetEntity()) 
			{
				Entity entity = hitInfo.PhysicsBody.GetEntity();
				id = entity.GetID();
			}

			outHitInfo.UUID = ScriptMarshal::UUIDToMonoArray(id);
			return hasHit;
		}
		// --------------------

		// ---- Rigidbody 2D ----
		static bool Rigidbody2D_IsFixedRotation(MonoArray* entityUUIDArr)
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

		static void Rigidbody2D_SetFixedRotation(MonoArray* entityUUIDArr, bool fixedRotation)
		{
			bool FixedRotation = fixedRotation;
			SET_COMPONENT_VAR(FixedRotation, entityUUIDArr, Rigidbody2DComponent);

			if (entity)
			{
				PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
				physicsBody.SetFixedRotation(FixedRotation);
			}
		}

		static uint8_t Rigidbody2D_GetSleepState(MonoArray* entityUUIDArr)
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

		static void Rigidbody2D_SetSleepState(MonoArray* entityUUIDArr, uint8_t sleepState)
		{
			PhysicsBodySleepState SleepState = (PhysicsBodySleepState)sleepState;
			SET_COMPONENT_VAR(SleepState, entityUUIDArr, Rigidbody2DComponent);

			if (entity)
			{
				PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
				physicsBody.SetSleepState(SleepState);
			}
		}

		static float Rigidbody2D_GetGravityScale(MonoArray* entityUUIDArr)
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

		static void Rigidbody2D_SetGravityScale(MonoArray* entityUUIDArr, float gravityScale)
		{
			float GravityScale = gravityScale;
			SET_COMPONENT_VAR(GravityScale, entityUUIDArr, Rigidbody2DComponent);

			if (entity)
			{
				PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
				physicsBody.SetGravityScale(GravityScale);
			}
		}

		static void Rigidbody2D_ApplyForce(MonoArray* entityUUIDArr, const glm::vec2& force, const glm::vec2& position, uint8_t forceMode)
		{
			UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
			Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

			if (entity)
			{
				PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
				physicsBody.ApplyForce(force, position, (ForceMode2D)forceMode);
			}
		}

		static void Rigidbody2D_ApplyForceAtCenter(MonoArray* entityUUIDArr, const glm::vec2& force, uint8_t forceMode)
		{
			UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
			Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

			if (entity)
			{
				PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
				physicsBody.ApplyForceAtCenter(force, (ForceMode2D)forceMode);
			}
		}

		static void Rigidbody2D_GetLinearVelocity(MonoArray* entityUUIDArr, glm::vec2& linearVelocity)
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

		static void Rigidbody2D_SetLinearVelocity(MonoArray* entityUUIDArr, const glm::vec2& linearVelocity)
		{
			UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
			Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

			if (entity)
			{
				PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
				physicsBody.SetLinearVelocity(linearVelocity);
			}
		}

		static float Rigidbody2D_GetAngularVelocity(MonoArray* entityUUIDArr)
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

		static void Rigidbody2D_SetAngularVelocity(MonoArray* entityUUIDArr, float angularVelocity)
		{
			UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
			Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

			if (entity)
			{
				PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
				physicsBody.SetAngularVelocity(angularVelocity);
			}
		}

		static uint8_t Rigidbody2D_GetType(MonoArray* entityUUIDArr) 
		{
			UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
			Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

			if (entity) 
			{
				PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
				return (uint8_t)physicsBody.GetBodyType();
			}
			return (uint8_t)PhysicsBodyType::Dynamic;
		}

		static void Rigidbody2D_SetType(MonoArray* entityUUIDArr, uint8_t bodyType) 
		{
			PhysicsBodyType BodyType = (PhysicsBodyType)bodyType;
			SET_COMPONENT_VAR(BodyType, entityUUIDArr, Rigidbody2DComponent);

			if (entity) 
			{
				PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
				physicsBody.SetBodyType(BodyType);
			}
		}

		// ----------------------
		
		// ---- Collider 2D ----
		static bool Collider2D_IsSensor(MonoArray* entityUUIDArr, uint8_t colliderType) 
		{
			bool IsSensor = false;

			switch ((ColliderType2D)colliderType)
			{
			case ColliderType2D::Box:
			{
				GET_COMPONENT_VAR(IsSensor, entityUUIDArr, BoxCollider2DComponent);
				break;
			}
			case ColliderType2D::Circle:
			{
				GET_COMPONENT_VAR(IsSensor, entityUUIDArr, CircleCollider2DComponent);
				break;
			}
			case ColliderType2D::None:
			{
				UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
				Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);
				PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);

				if (physicsBody)
				{
					Shared<Collider2D> collider = physicsBody.GetColliders()[0];

					if (collider)
						IsSensor = collider->IsSensor();
				}
				break;
			}
			}
			return IsSensor;
		}
		
		static void Collider2D_SetSensor(MonoArray* entityUUIDArr, uint8_t colliderType, bool isSensor) 
		{
			UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
			Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);
			PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);

			if (physicsBody) 
			{
				switch ((ColliderType2D)colliderType)
				{
				case ColliderType2D::Box:
				{
					BoxCollider2DComponent& bcComponent = entity.GetComponent<BoxCollider2DComponent>();
					Shared<Collider2D> collider = physicsBody.GetColliders()[bcComponent.ColliderIndex];
					bcComponent.IsSensor = isSensor;
					collider->SetSensor(isSensor);
					break;
				}
				case ColliderType2D::Circle:
				{
					CircleCollider2DComponent ccComponent = entity.GetComponent<CircleCollider2DComponent>();
					Shared<Collider2D> collider = physicsBody.GetColliders()[ccComponent.ColliderIndex];
					ccComponent.IsSensor = isSensor;
					collider->SetSensor(isSensor);
					break;
				}
				case ColliderType2D::None:
				{
					Shared<Collider2D> collider = physicsBody.GetColliders()[0];

					if (collider)
						collider->SetSensor(isSensor);

					switch (collider->GetType())
					{
					case ColliderType2D::Box: 
					{
						BoxCollider2DComponent& bcComponent = entity.GetComponent<BoxCollider2DComponent>();
						bcComponent.IsSensor = isSensor;
						break;
					}
					case ColliderType2D::Circle: 
					{
						CircleCollider2DComponent& ccComponent = entity.GetComponent<CircleCollider2DComponent>();
						ccComponent.IsSensor = isSensor;
						break;
					}
					}

					break;
				}
				}
			}
		}

		static void Collider2D_GetOffset(MonoArray* entityUUIDArr, uint8_t colliderType, glm::vec2& outOffset) 
		{
			glm::vec2 Offset = { 0.0f, 0.0f };

			switch ((ColliderType2D)colliderType)
			{
			case ColliderType2D::Box:
			{
				GET_COMPONENT_VAR(Offset, entityUUIDArr, BoxCollider2DComponent);
				break;
			}
			case ColliderType2D::Circle:
			{
				GET_COMPONENT_VAR(Offset, entityUUIDArr, CircleCollider2DComponent);
				break;
			}
			case ColliderType2D::None:
			{
				UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
				Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

				PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
				if (physicsBody)
				{

					Shared<Collider2D> collider;
					if (physicsBody.GetColliders().size() > 0)
						collider = physicsBody.GetColliders()[0];

					if (collider)
					{
						// nested switch statements; fucking disgusting
						switch (collider->GetType())
						{
						case ColliderType2D::Box:
						{
							Shared<BoxCollider2D> boxCollider = std::dynamic_pointer_cast<BoxCollider2D>(collider);
							if (boxCollider)
								Offset = boxCollider->GetOffset();
							break;
						}
						case ColliderType2D::Circle:
						{
							Shared<CircleCollider2D> circleCollider = std::dynamic_pointer_cast<CircleCollider2D>(collider);
							if (circleCollider)
								Offset = circleCollider->GetOffset();

							break;
						}
						}
					}
				}

				break;
			}
			}
			outOffset = Offset;
		}

		static void Collider2D_SetOffset(MonoArray* entityUUIDArr, uint8_t colliderType, const glm::vec2& inOffset) 
		{
			glm::vec2 Offset = inOffset;
			UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
			Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);
			PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);

			if (physicsBody) 
			{
				switch ((ColliderType2D)colliderType)
				{
				case ColliderType2D::Box: 
				{
					BoxCollider2DComponent& bcComponent = entity.GetComponent<BoxCollider2DComponent>();
					Shared<Collider2D> collider = physicsBody.GetColliders()[bcComponent.ColliderIndex];
					
					if(collider)
						collider->SetOffset(Offset);
					break;
				}
				case ColliderType2D::Circle: 
				{
					CircleCollider2DComponent& ccComponent = entity.GetComponent<CircleCollider2DComponent>();
					Shared<Collider2D> collider = physicsBody.GetColliders()[ccComponent.ColliderIndex];
					if (collider)
						collider->SetOffset(Offset);

					break;
				}
				case ColliderType2D::None: 
				{
					Shared<Collider2D> collider = physicsBody.GetColliders()[0];

					if(collider)
						collider->SetOffset(Offset);

					// NOTE: this does not scale if in the future an entity can have multiple colliders of the same type;
					// but it fucking works for now
					switch (collider->GetType())
					{
					case ColliderType2D::Box: 
					{
						BoxCollider2DComponent& bcComponent = entity.GetComponent<BoxCollider2DComponent>();
						bcComponent.Offset = Offset;
						break;
					}
					case ColliderType2D::Circle: 
					{
						CircleCollider2DComponent& ccComponent = entity.GetComponent<CircleCollider2DComponent>();
						ccComponent.Offset = Offset;
						break;
					}
					}
					break;
				}
				}
			}
		}

		// ---------------------

		// ---- Box Collider 2D ----
		void BoxCollider2D_GetSize(MonoArray* entityUUIDArr, glm::vec2& size)
		{
			glm::vec2 Size = { 0.0f, 0.0f };
			GET_COMPONENT_VAR(Size, entityUUIDArr, BoxCollider2DComponent);
			size = Size;
		}

		void BoxCollider2D_SetSize(MonoArray* entityUUIDArr, const glm::vec2& size)
		{
			glm::vec2 Size = size;
			SET_COMPONENT_VAR(Size, entityUUIDArr, BoxCollider2DComponent);
			PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);

			if (physicsBody) 
			{
				BoxCollider2DComponent& bcComponent = entity.GetComponent<BoxCollider2DComponent>();
				Shared<Collider2D> collider = physicsBody.GetColliders()[bcComponent.ColliderIndex];
				Shared<BoxCollider2D> boxCollider = DynamicCast<BoxCollider2D>(collider);

				if (boxCollider)
					boxCollider->SetSize(Size);
			}
		}
		// -------------------------

		// ---- Circle Collider 2D ----
		float CircleCollider2D_GetRadius(MonoArray* entityUUIDArr)
		{
			float Radius = 0.0f;
			GET_COMPONENT_VAR(Radius, entityUUIDArr, CircleCollider2DComponent);
			return Radius;
		}

		void CircleCollider2D_SetRadius(MonoArray* entityUUIDArr, float radius)
		{
			float Radius = radius;
			SET_COMPONENT_VAR(Radius, entityUUIDArr, CircleCollider2DComponent);
			PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);

			if (physicsBody) 
			{
				CircleCollider2DComponent& ccComponent = entity.GetComponent<CircleCollider2DComponent>();
				Shared<Collider2D> collider = physicsBody.GetColliders()[ccComponent.ColliderIndex];
				Shared<CircleCollider2D> circleCollider = DynamicCast<CircleCollider2D>(collider);

				if (circleCollider)
					circleCollider->SetRadius(Radius);
			}
		}
		// ----------------------------

		// ---- Log ----
		void Log_Log(uint8_t logLevel, MonoString* monoMessage)
		{
			std::string message = ScriptMarshal::MonoStringToUTF8(monoMessage);
			switch (logLevel)
			{
			case 1 << 0: TR_CLIENT_TRACE(message); break;
			case 1 << 1: TR_CLIENT_WARN(message); break;
			case 1 << 2: TR_CLIENT_ERROR(message); break;
			}
		}
		// -------------

		// ---- Input ----
		bool Input_KeyPressed(Key keyCode) { return Input::IsKeyPressed(keyCode); }
		bool Input_MouseButtonPressed(MouseButton mouseButton) { return Input::IsMouseButtonPressed(mouseButton); }
		void Input_GetMousePosition(glm::vec2& outMousePosition) { outMousePosition = Input::GetMousePos(); }
		bool Input_IsControllerConnected(uint8_t controllerIndex) { return Input::IsControllerConnected(controllerIndex); }

		MonoString* Input_GetControllerName(uint8_t controllerIndex)
		{
			const char* controllerName = Input::GetControllerName(controllerIndex);
			return ScriptMarshal::UTF8ToMonoString(controllerName);
		}

		bool Input_IsControllerButtonPressed(ControllerButton controllerButton, uint8_t controllerIndex) { return Input::IsControllerButtonPressed(controllerButton, controllerIndex); }
		float Input_GetControllerAxis(ControllerAxis controllerAxis, uint8_t controllerIndex) { return Input::GetControllerAxis(controllerAxis, controllerIndex); }

		MonoArray* Input_GetConnectedControllers()
		{
			std::vector<uint8_t> connectedControllers = Input::GetConnectedControllers();
			ScriptArray connectedControllersArr = ScriptArray::Create<uint8_t>(connectedControllers.size());

			for (size_t i = 0; i < connectedControllers.size(); i++)
				connectedControllersArr.Set<uint8_t>(i, i);

			return connectedControllersArr.GetMonoArray();
		}

		// ---------------
	}
}
