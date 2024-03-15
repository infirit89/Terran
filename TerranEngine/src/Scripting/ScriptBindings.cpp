#include "trpch.h"
#include "ScriptBindings.h"

#include "ScriptEngine.h"
//#include "ScriptMarshal.h"
//#include "GCManager.h"
//#include "ScriptCache.h"
//#include "ScriptArray.h"
//#include "ManagedObject.h"
//#include "ManagedMethodThunks.h"

#include "Core/Input.h"

#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Scene/SceneManager.h"

#include "Physics/Physics.h"
#include "Physics/PhysicsBody.h"
#include "Physics/Collider.h"
#include "Physics/PhysicsLayerManager.h"

#include "Utils/Debug/OptickProfiler.h"

#include <glm/glm.hpp>

#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

#include <box2d/box2d.h>

namespace TerranEngine 
{
#define BIND_INTERNAL_FUNC(func) assembly.AddInternalCall("Terran.Internal", #func, reinterpret_cast<void*>(&func))

	void ScriptBindings::Bind(Coral::ManagedAssembly& assembly) 
	{
		//assembly.AddInternalCall("Terran.Internal", "Log_LogICall", reinterpret_cast<void*>(&Log_LogICall));
		BIND_INTERNAL_FUNC(Log_LogICall);
		assembly.UploadInternalCalls();
	}

	void ScriptBindings::Log_LogICall(uint8_t logLevel, Coral::String message)
	{
		TR_PROFILE_FUNCTION();
		std::string messageStr = message;
		TR_TRACE(messageStr);
		switch (logLevel)
		{
		case 1 << 0: TR_CLIENT_TRACE(messageStr); break;
		case 1 << 1: TR_CLIENT_WARN(messageStr); break;
		case 1 << 2: TR_CLIENT_ERROR(messageStr); break;
		}
	}

#if 0
#define BIND_INTERNAL_FUNC(func) mono_add_internal_call("Terran.Internal::"#func, (const void*)func)

		static ManagedMethodThunks<MonoArray*> s_IDClassCtor;
		static std::unordered_map <MonoType*, std::function<bool(Entity)>> s_HasComponentFuncs;
		static std::unordered_map <MonoType*, std::function<void(Entity)>> s_AddComponentFuncs;
		static std::unordered_map <MonoType*, std::function<void(Entity)>> s_RemoveComponentFuncs;

#define REGISTER_COMPONENT(type, component)\
	MonoType* monoType_##type = mono_reflection_type_from_name((char*)("Terran."#type), ScriptEngine::GetAssembly(TR_CORE_ASSEMBLY_INDEX)->GetMonoImage());\
	TR_TRACE((void*)monoType_##type);\
	TR_ASSERT(monoType_##type, "Invaled type");\
	s_HasComponentFuncs[monoType_##type] = [](Entity entity) { return entity.HasComponent<component>(); };\
	s_AddComponentFuncs[monoType_##type] = [](Entity entity) { return entity.AddComponent<component>(); };\
	s_RemoveComponentFuncs[monoType_##type] = [](Entity entity) { return entity.RemoveComponent<component>(); }\

		void Bind()
		{
			REGISTER_COMPONENT(Transform,			TransformComponent);
			REGISTER_COMPONENT(Tag,					TagComponent);
			REGISTER_COMPONENT(CapsuleCollider2D,	CapsuleCollider2DComponent);
			REGISTER_COMPONENT(CircleCollider2D,	CircleCollider2DComponent);
			REGISTER_COMPONENT(BoxCollider2D,		BoxCollider2DComponent);
			REGISTER_COMPONENT(Rigidbody2D,			Rigidbody2DComponent);
			REGISTER_COMPONENT(Camera,				CameraComponent);
			REGISTER_COMPONENT(CircleRenderer,		CircleRendererComponent);
			REGISTER_COMPONENT(TextRenderer,		TextRendererComponent);

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

			// ---- text renderer ---
			BIND_INTERNAL_FUNC(TextRenderer_GetColor);
			BIND_INTERNAL_FUNC(TextRenderer_SetColor);

			BIND_INTERNAL_FUNC(TextRenderer_GetText);
			BIND_INTERNAL_FUNC(TextRenderer_SetText);
			// ----------------------
			
			// ---- physics ----
			{
				// ---- physics 2d ----
				BIND_INTERNAL_FUNC(Physics2D_RayCast);
				BIND_INTERNAL_FUNC(Physics2D_RayCastAll);
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
				BIND_INTERNAL_FUNC(Collider2D_SetOffset);

				BIND_INTERNAL_FUNC(Collider2D_IsSensor);
				BIND_INTERNAL_FUNC(Collider2D_SetSensor);
				// ---------------------

				// ---- box collider 2d ----
				BIND_INTERNAL_FUNC(BoxCollider2D_GetSize);
				BIND_INTERNAL_FUNC(BoxCollider2D_SetSize);
				// -------------------------

				// ---- circle collider 2d ----
				BIND_INTERNAL_FUNC(CircleCollider2D_GetRadius);
				BIND_INTERNAL_FUNC(CapsuleCollider2D_SetSize);
				// -----------------------------

				// ---- capsule collider 2d ----
				BIND_INTERNAL_FUNC(CapsuleCollider2D_GetSize);
				BIND_INTERNAL_FUNC(BoxCollider2D_SetSize);
				// -----------------------------

                // ---- layer mask ----
                BIND_INTERNAL_FUNC(LayerMask_GetName);
                // --------------------
			}

			// -----------------

			// ---- misc ----
			BIND_INTERNAL_FUNC(Log_Log);
			// --------------

			// ---- input -----
			BIND_INTERNAL_FUNC(Input_KeyPressed);
			BIND_INTERNAL_FUNC(Input_KeyDown);
			BIND_INTERNAL_FUNC(Input_KeyReleased);

			BIND_INTERNAL_FUNC(Input_MouseButtonPressed);
			BIND_INTERNAL_FUNC(Input_MouseButtonDown);
			BIND_INTERNAL_FUNC(Input_MouseButtonReleased);
			BIND_INTERNAL_FUNC(Input_GetMousePosition);

			BIND_INTERNAL_FUNC(Input_IsControllerConnected);
			BIND_INTERNAL_FUNC(Input_GetControllerName);

			BIND_INTERNAL_FUNC(Input_IsControllerButtonPressed);
			BIND_INTERNAL_FUNC(Input_GetControllerAxis);

			BIND_INTERNAL_FUNC(Input_GetConnectedControllers);
			// ----------------

			s_IDClassCtor.SetFromMethod(ScriptCache::GetCachedMethod("Terran.UUID", ":.ctor(byte[])"));
		}

		void Unbind()
		{
			s_HasComponentFuncs.clear();
			s_AddComponentFuncs.clear();
			s_RemoveComponentFuncs.clear();
		}

        Entity GetEntityFromMonoArray(MonoArray* id)
        {
			TR_PROFILE_FUNCTION();
            if(!SceneManager::GetCurrentScene())
                return {};

            UUID entityUUID = ScriptMarshal::MonoArrayToUUID(id);
            return SceneManager::GetCurrentScene()->FindEntityWithUUID(entityUUID);
        }

		// ---- Entity Utils ----
		bool Entity_HasComponent(MonoArray* uuidData, MonoReflectionType* monoRefType)
		{
			TR_PROFILE_FUNCTION();

            Entity entity = GetEntityFromMonoArray(uuidData);
            if(!entity)
                return false;

			MonoType* monoType = mono_reflection_type_get_type(monoRefType);
			
			if (s_HasComponentFuncs.find(monoType) != s_HasComponentFuncs.end())
				return s_HasComponentFuncs[monoType](entity);

			ManagedClass klass(mono_class_from_mono_type(monoType));
			if (klass.IsSubclassOf(TR_API_CACHED_CLASS(Scriptable))) return entity.HasComponent<ScriptComponent>() && 
																	entity.GetComponent<ScriptComponent>().ModuleName == mono_type_get_name(monoType);
			return false;
		}

		void Entity_AddComponent(MonoArray* entityUUIDArr, MonoReflectionType* monoRefType)
		{
			TR_PROFILE_FUNCTION();
			
            Entity entity = GetEntityFromMonoArray(entityUUIDArr);
            if(!entity)
                return;

			MonoType* monoType = mono_reflection_type_get_type(monoRefType);


			if (s_AddComponentFuncs.find(monoType) != s_AddComponentFuncs.end()) 
			{
				s_AddComponentFuncs[monoType](entity);
				return;
			}

			ManagedClass klass (mono_class_from_mono_type(monoType));
			if (klass.IsSubclassOf(TR_API_CACHED_CLASS(Scriptable))) entity.AddComponent<ScriptComponent>(mono_type_get_name(monoType));

			/*switch (type)
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
			case ComponentType::CapsuleCollider2DComponent:	entity.AddComponent<CapsuleCollider2DComponent>(); break;
			case ComponentType::TextRendererComponent:		entity.AddComponent<TextRendererComponent>(); break;
			}*/
		}

		void Entity_RemoveComponent(MonoArray* entityUUIDArr, MonoReflectionType* monoRefType)
		{
			TR_PROFILE_FUNCTION();
			
            Entity entity = GetEntityFromMonoArray(entityUUIDArr);
            if(!entity)
                return;

			MonoType* monoType = mono_reflection_type_get_type(monoRefType);

			if (s_RemoveComponentFuncs.find(monoType) != s_RemoveComponentFuncs.end())
				s_RemoveComponentFuncs[monoType](entity);

			// TODO: handle removing script component
		}

		MonoObject* Entity_GetScriptableComponent(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
            Entity entity = GetEntityFromMonoArray(entityUUIDArr);
            if(!entity)
                return nullptr;

			GCHandle objectHandle = ScriptEngine::GetScriptInstanceGCHandle(entity.GetSceneID(), entity.GetID());
			return GCManager::GetManagedObject(objectHandle);
		}

		bool Entity_FindEntityWithID(MonoArray* monoIDArray)
		{
			TR_PROFILE_FUNCTION();
            Entity entity = GetEntityFromMonoArray(monoIDArray);
			return (bool)entity;
		}

		MonoArray* Entity_FindEntityWithName(MonoString* monoEntityName)
		{
			TR_PROFILE_FUNCTION();
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
			TR_PROFILE_FUNCTION();
            Entity entity = GetEntityFromMonoArray(entityUUIDArr);

			if (entity)
				SceneManager::GetCurrentScene()->DestroyEntity(entity, true);
			else
				TR_CLIENT_ERROR("Can't destroy entity because it doesnt exist");
		}

		// TODO: find a better way to do this
		MonoArray* Entity_GetChildren(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
            Entity entity = GetEntityFromMonoArray(entityUUIDArr);
            if(!entity)
                return nullptr;

			if(entity.GetChildCount() <= 0)
				return nullptr;
			
			ScriptArray childrenIDs = ScriptArray::Create<UUID>(static_cast<uint32_t>(entity.GetChildCount()));
			ManagedClass* idClass = TR_API_CACHED_CLASS(UUID);
			
			int i = 0;
			for (const UUID& id : entity.GetChildren())
			{
				ManagedObject idObject = idClass->CreateInstance();
				MonoArray* uuidData = ScriptMarshal::UUIDToMonoArray(id);

				MonoException* exc = nullptr;
				s_IDClassCtor.Invoke(idObject.GetMonoObject(), uuidData, &exc);
				childrenIDs.Set(i, idObject.GetMonoObject());
				i++;
			}

			return childrenIDs.GetMonoArray();
		}

		// ----------------------

// bullshit?
#define SET_COMPONENT_VAR(var, entityID, componentType)\
    Entity entity = GetEntityFromMonoArray(entityUUIDArr);\
	if(entity)\
		entity.GetComponent<componentType>().var = var;\
	else\
		TR_ERROR("Invalid entity id");

// bullshit #2? 
#define GET_COMPONENT_VAR(var, entityID, componentType)\
    Entity entity = GetEntityFromMonoArray(entityUUIDArr);\
	if(entity)\
		var = entity.GetComponent<componentType>().var;\
	else\
		TR_ERROR("Invalid entity id");

		// ---- Transform ----
		void Transform_SetPosition(MonoArray* entityUUIDArr, const glm::vec3& Position)
		{
			TR_PROFILE_FUNCTION();
			SET_COMPONENT_VAR(Position, entityUUIDArr, TransformComponent);
			if (entity)
				entity.GetComponent<TransformComponent>().IsDirty = true;
		}

		glm::vec3 Transform_GetPosition(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
			glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
			GET_COMPONENT_VAR(Position, entityUUIDArr, TransformComponent);
			return Position;
		}

		void Transform_SetRotation(MonoArray* entityUUIDArr, const glm::vec3& Rotation)
		{
			TR_PROFILE_FUNCTION();
			SET_COMPONENT_VAR(Rotation, entityUUIDArr, TransformComponent);
			if (entity)
				entity.GetComponent<TransformComponent>().IsDirty = true;
		}

		glm::vec3 Transform_GetRotation(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
			glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
			GET_COMPONENT_VAR(Rotation, entityUUIDArr, TransformComponent);
			return Rotation;
		}

		void Transform_SetScale(MonoArray* entityUUIDArr, const glm::vec3& Scale)
		{
			TR_PROFILE_FUNCTION();
			SET_COMPONENT_VAR(Scale, entityUUIDArr, TransformComponent);
			if (entity)
				entity.GetComponent<TransformComponent>().IsDirty = true;
		}

		glm::vec3 Transform_GetScale(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
			glm::vec3 Scale = { 0.0f, 0.0f, 0.0f };
			GET_COMPONENT_VAR(Scale, entityUUIDArr, TransformComponent);
			return Scale;
		}

		bool Transform_IsDirty(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
            bool IsDirty = false;
            GET_COMPONENT_VAR(IsDirty, entityUUIDArr, TransformComponent);
            return IsDirty;
		}

		glm::vec3 Transform_GetForward(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
			glm::vec3 Forward = { 0.0f, 0.0f, 1.0f };
			GET_COMPONENT_VAR(Forward, entityUUIDArr, TransformComponent);
			return Forward;
		}

		glm::vec3 Transform_GetUp(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
			glm::vec3 Up = { 0.0f, 1.0f, 0.0f };
			GET_COMPONENT_VAR(Up, entityUUIDArr, TransformComponent);
			return Up;
		}

		glm::vec3 Transform_GetRight(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
			glm::vec3 Right = { 1.0f, 0.0f, 0.0f };
			GET_COMPONENT_VAR(Right, entityUUIDArr, TransformComponent);
			return Right;
		}
		// -------------------

		// ---- Sprite Renderer ----
		glm::vec4 SpriteRenderer_GetColor(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
			glm::vec4 Color = {0.0f, 0.0f, 0.0f, 1.0f};
			GET_COMPONENT_VAR(Color, entityUUIDArr, SpriteRendererComponent);
			return Color;
		}

		void SpriteRenderer_SetColor(MonoArray* entityUUIDArr, const glm::vec4& color)
		{
			TR_PROFILE_FUNCTION();
			glm::vec4 Color = color;
			SET_COMPONENT_VAR(Color, entityUUIDArr, SpriteRendererComponent);
		}
		// -------------------------

		// ---- Camera ----
		bool Camera_IsPrimary(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
			bool Primary = false;
			GET_COMPONENT_VAR(Primary, entityUUIDArr, CameraComponent);
			return Primary;
		}

		void Camera_SetPrimary(MonoArray* entityUUIDArr, bool togglePrimary)
		{
			TR_PROFILE_FUNCTION();
			bool Primary = togglePrimary;
			SET_COMPONENT_VAR(Primary, entityUUIDArr, CameraComponent);
		}

		glm::vec4 Camera_GetBackgroundColor(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
			glm::vec4 BackgroundColor = { 0.1f, 0.1f, 0.1f, 1.0f };
			GET_COMPONENT_VAR(BackgroundColor, entityUUIDArr, CameraComponent);
			return BackgroundColor;
		}

		void Camera_SetBackgroundColor(MonoArray* entityUUIDArr, const glm::vec4& color)
		{
			TR_PROFILE_FUNCTION();
			glm::vec4 BackgroundColor = color;
			SET_COMPONENT_VAR(BackgroundColor, entityUUIDArr, CameraComponent);
		}
		// ----------------

		// ---- Circle Renderer ----
		glm::vec4 CircleRenderer_GetColor(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
			glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
			GET_COMPONENT_VAR(Color, entityUUIDArr, CircleRendererComponent);
			return Color;
		}

		void CircleRenderer_SetColor(MonoArray* entityUUIDArr, const glm::vec4& color)
		{
			TR_PROFILE_FUNCTION();
			glm::vec4 Color = color;
			SET_COMPONENT_VAR(Color, entityUUIDArr, CircleRendererComponent);
		}

		float CircleRenderer_GetThickness(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
			float Thickness = 1.0f;
			GET_COMPONENT_VAR(Thickness, entityUUIDArr, CircleRendererComponent);
			return Thickness;
		}

		void CircleRenderer_SetThickness(MonoArray* entityUUIDArr, float thickness)
		{
			TR_PROFILE_FUNCTION();
			float Thickness = thickness;
			SET_COMPONENT_VAR(Thickness, entityUUIDArr, CircleRendererComponent);
		}
		// -------------------------

		// ---- Text Renderer ----
		glm::vec4 TextRenderer_GetColor(MonoArray* entityUUIDArr) 
		{
			TR_PROFILE_FUNCTION();
			glm::vec4 TextColor = {1.0f, 1.0f, 1.0f, 1.0f};
			GET_COMPONENT_VAR(TextColor, entityUUIDArr, TextRendererComponent);
			return TextColor;
		}

		void TextRenderer_SetColor(MonoArray* entityUUIDArr, const glm::vec4& color) 
		{
			TR_PROFILE_FUNCTION();
			glm::vec4 TextColor = color;
			SET_COMPONENT_VAR(TextColor, entityUUIDArr, TextRendererComponent);
		}

		MonoString* TextRenderer_GetText(MonoArray* entityUUIDArr) 
		{
			TR_PROFILE_FUNCTION();
			std::string Text = "";
			GET_COMPONENT_VAR(Text, entityUUIDArr, TextRendererComponent);
			return ScriptMarshal::UTF8ToMonoString(Text);
		}

		void TextRenderer_SetText(MonoArray* entityUUIDArr, MonoString* text) 
		{
			TR_PROFILE_FUNCTION();
			std::string Text = ScriptMarshal::MonoStringToUTF8(text);
			SET_COMPONENT_VAR(Text, entityUUIDArr, TextRendererComponent);
		}
		// -----------------------

		// ---- Tag ----
		void Tag_SetName(MonoArray* entityUUIDArr, MonoString* name)
		{
			TR_PROFILE_FUNCTION();
			std::string Name = ScriptMarshal::MonoStringToUTF8(name);
			SET_COMPONENT_VAR(Name, entityUUIDArr, TagComponent);
		}

		MonoString* Tag_GetName(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
			std::string Name = "";
			GET_COMPONENT_VAR(Name, entityUUIDArr, TagComponent);
			return ScriptMarshal::UTF8ToMonoString(Name);
		}
		// -------------

		// ---- Physics 2D ----
		bool Physics2D_RayCast(const glm::vec2& origin, const glm::vec2& direction, float length, RayCastHitInfo2D_Internal& outHitInfo, uint16_t layerMask)
		{
			TR_PROFILE_FUNCTION();
			RayCastHitInfo2D hitInfo;
			bool hasHit = Physics2D::RayCast(origin, direction, length, hitInfo, layerMask);

			outHitInfo.Point = hitInfo.Point;
			outHitInfo.Normal = hitInfo.Normal;
			
			UUID id({ 0 });

			if (!hitInfo.PhysicsBody) return false;

			if (hitInfo.PhysicsBody->GetEntity()) 
			{
				Entity entity = hitInfo.PhysicsBody->GetEntity();
				id = entity.GetID();
			}

			//outHitInfo.UUID = ScriptMarshal::UUIDToMonoArray(id).GetMonoArray();
			MonoArray* uuidData = ScriptMarshal::UUIDToMonoArray(id);

			void* entityCtorArgs[] = { uuidData };
			ManagedObject entityObj = TR_API_CACHED_CLASS(Entity)->CreateInstance();
			ManagedMethod* entityConstructor = ScriptCache::GetCachedMethod("Terran.Entity", ":.ctor(byte[])");
			entityConstructor->Invoke(entityObj, entityCtorArgs);

			void* rigidbodyCtorArgs[] = { entityObj.GetMonoObject() };
			ManagedObject rigidbodyObj = TR_API_CACHED_CLASS(Rigidbody2D)->CreateInstance();
			ManagedMethod* rigidbodyConstructor = ScriptCache::GetCachedMethod("Terran.Rigidbody2D", ":.ctor(Entity)");
			rigidbodyConstructor->Invoke(rigidbodyObj, rigidbodyCtorArgs);
			outHitInfo.UUID = rigidbodyObj.GetMonoObject();

			return hasHit;
		}

		MonoArray* Physics2D_RayCastAll(const glm::vec2& origin, const glm::vec2& direction, float length, uint16_t layerMask)
		{
			TR_PROFILE_FUNCTION();
			std::vector<RayCastHitInfo2D> hitInfos = Physics2D::RayCastAll(origin, direction, length, layerMask);

			ScriptArray hitInfos_Internal(TR_API_CACHED_CLASS(RayCastHitInfo2D)->GetMonoClass(), static_cast<uint32_t>(hitInfos.size()));

			for (size_t i = 0; i < hitInfos.size(); i++)
			{
				RayCastHitInfo2D_Internal hitInfo;
				hitInfo.Normal = hitInfos[i].Normal;
				hitInfo.Point = hitInfos[i].Point;

				UUID id({ 0 });

				if (!hitInfos[i].PhysicsBody) return nullptr;

				if (hitInfos[i].PhysicsBody->GetEntity())
				{
					Entity entity = hitInfos[i].PhysicsBody->GetEntity();
					id = entity.GetID();
				}

				MonoArray* uuidData = ScriptMarshal::UUIDToMonoArray(id);

				void* entityCtorArgs[] = { uuidData };
				ManagedObject entityObj = TR_API_CACHED_CLASS(Entity)->CreateInstance();
				ManagedMethod* entityConstructor = ScriptCache::GetCachedMethod("Terran.Entity", ":.ctor(byte[])");
				entityConstructor->Invoke(entityObj, entityCtorArgs);

				void* rigidbodyCtorArgs[] = { entityObj.GetMonoObject() };
				ManagedObject rigidbodyObj = TR_API_CACHED_CLASS(Rigidbody2D)->CreateInstance();
				ManagedMethod* rigidbodyConstructor = ScriptCache::GetCachedMethod("Terran.Rigidbody2D", ":.ctor(Entity)");
				rigidbodyConstructor->Invoke(rigidbodyObj, rigidbodyCtorArgs);
				hitInfo.UUID = rigidbodyObj.GetMonoObject();

				hitInfos_Internal.Set<RayCastHitInfo2D_Internal>(static_cast<uint32_t>(i), hitInfo);
			}

			return hitInfos_Internal.GetMonoArray();
		}
		// --------------------

		// ---- Rigidbody 2D ----
		static bool Rigidbody2D_IsFixedRotation(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
            Entity entity = GetEntityFromMonoArray(entityUUIDArr);

			if (entity)
			{
				Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
				return physicsBody->GetFixedRotation();
			}
			return false;
		}

		static void Rigidbody2D_SetFixedRotation(MonoArray* entityUUIDArr, bool fixedRotation)
		{
			TR_PROFILE_FUNCTION();
			bool FixedRotation = fixedRotation;
			SET_COMPONENT_VAR(FixedRotation, entityUUIDArr, Rigidbody2DComponent);

			if (entity)
			{
				Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
				physicsBody->SetFixedRotation(FixedRotation);
			}
		}

		static uint8_t Rigidbody2D_GetSleepState(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
            Entity entity = GetEntityFromMonoArray(entityUUIDArr);

			if (entity)
			{
				Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
				return (uint8_t)physicsBody->GetSleepState();
			}
			return (uint8_t)PhysicsBodySleepState::Awake;
		}

		static void Rigidbody2D_SetSleepState(MonoArray* entityUUIDArr, uint8_t sleepState)
		{
			TR_PROFILE_FUNCTION();
			PhysicsBodySleepState SleepState = (PhysicsBodySleepState)sleepState;
			SET_COMPONENT_VAR(SleepState, entityUUIDArr, Rigidbody2DComponent);

			if (entity)
			{
				Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
				physicsBody->SetSleepState(SleepState);
			}
		}

		static float Rigidbody2D_GetGravityScale(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
			float GravityScale = 0.0f;
            Entity entity = GetEntityFromMonoArray(entityUUIDArr);

			if (entity)
			{
				Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
				return physicsBody->GetGravityScale();
			}
			return GravityScale;
		}

		static void Rigidbody2D_SetGravityScale(MonoArray* entityUUIDArr, float gravityScale)
		{
			TR_PROFILE_FUNCTION();
			float GravityScale = gravityScale;
			SET_COMPONENT_VAR(GravityScale, entityUUIDArr, Rigidbody2DComponent);

			if (entity)
			{
				Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
				physicsBody->SetGravityScale(GravityScale);
			}
		}

		static void Rigidbody2D_ApplyForce(MonoArray* entityUUIDArr, const glm::vec2& force, const glm::vec2& position, uint8_t forceMode)
		{
			TR_PROFILE_FUNCTION();
            Entity entity = GetEntityFromMonoArray(entityUUIDArr);

			if (entity)
			{
				Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
				physicsBody->ApplyForce(force, position, (ForceMode2D)forceMode);
			}
		}

		static void Rigidbody2D_ApplyForceAtCenter(MonoArray* entityUUIDArr, const glm::vec2& force, uint8_t forceMode)
		{
			TR_PROFILE_FUNCTION();
            Entity entity = GetEntityFromMonoArray(entityUUIDArr);

			if (entity)
			{
				Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
				physicsBody->ApplyForceAtCenter(force, (ForceMode2D)forceMode);
			}
		}

		static void Rigidbody2D_GetLinearVelocity(MonoArray* entityUUIDArr, glm::vec2& linearVelocity)
		{
			TR_PROFILE_FUNCTION();
            Entity entity = GetEntityFromMonoArray(entityUUIDArr);

			if (entity)
			{
				Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
				linearVelocity = physicsBody->GetLinearVelocity();
				return;
			}
			linearVelocity = { 0.0f, 0.0f };
		}

		static void Rigidbody2D_SetLinearVelocity(MonoArray* entityUUIDArr, const glm::vec2& linearVelocity)
		{
			TR_PROFILE_FUNCTION();
            Entity entity = GetEntityFromMonoArray(entityUUIDArr);

			if (entity)
			{
				Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
				physicsBody->SetLinearVelocity(linearVelocity);
			}
		}

		static float Rigidbody2D_GetAngularVelocity(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
            Entity entity = GetEntityFromMonoArray(entityUUIDArr);

			if (entity)
			{
				Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
				return physicsBody->GetAngularVelocity();
			}
			return 0.0f;
		}

		static void Rigidbody2D_SetAngularVelocity(MonoArray* entityUUIDArr, float angularVelocity)
		{
			TR_PROFILE_FUNCTION();
            Entity entity = GetEntityFromMonoArray(entityUUIDArr);

			if (entity)
			{
				Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
				physicsBody->SetAngularVelocity(angularVelocity);
			}
		}

		static uint8_t Rigidbody2D_GetType(MonoArray* entityUUIDArr) 
		{
			TR_PROFILE_FUNCTION();
            Entity entity = GetEntityFromMonoArray(entityUUIDArr);

			if (entity) 
			{
				Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
				return (uint8_t)physicsBody->GetBodyType();
			}
			return (uint8_t)PhysicsBodyType::Dynamic;
		}

		static void Rigidbody2D_SetType(MonoArray* entityUUIDArr, uint8_t bodyType) 
		{
			TR_PROFILE_FUNCTION();
			PhysicsBodyType BodyType = (PhysicsBodyType)bodyType;
			SET_COMPONENT_VAR(BodyType, entityUUIDArr, Rigidbody2DComponent);

			if (entity) 
			{
				Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
				physicsBody->SetBodyType(BodyType);
			}
		}

		// ----------------------
		
		// ---- Collider 2D ----
		static bool Collider2D_IsSensor(MonoArray* entityUUIDArr, uint8_t colliderType) 
		{
			TR_PROFILE_FUNCTION();
			bool Sensor = false;

			switch ((ColliderType2D)colliderType)
			{
			case ColliderType2D::Box:
			{
				GET_COMPONENT_VAR(Sensor, entityUUIDArr, BoxCollider2DComponent);
				break;
			}
			case ColliderType2D::Circle:
			{
				GET_COMPONENT_VAR(Sensor, entityUUIDArr, CircleCollider2DComponent);
				break;
			}
			case ColliderType2D::Capsule: 
			{
				GET_COMPONENT_VAR(Sensor, entityUUIDArr, CapsuleCollider2DComponent);
				break;
			}
			case ColliderType2D::None:
			{
				UUID id = ScriptMarshal::MonoArrayToUUID(entityUUIDArr);
				Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);
				Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);

				if (physicsBody)
				{
					Shared<Collider2D> collider = physicsBody->GetColliders()[0];

					if (collider)
						Sensor = collider->IsSensor();
				}
				break;
			}
			}
			return Sensor;
		}
		
		static void Collider2D_SetSensor(MonoArray* entityUUIDArr, uint8_t colliderType, bool isSensor) 
		{
            Entity entity = GetEntityFromMonoArray(entityUUIDArr);
			Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);

			if (physicsBody) 
			{
				switch ((ColliderType2D)colliderType)
				{
				case ColliderType2D::Box:
				{
					BoxCollider2DComponent& bcComponent = entity.GetComponent<BoxCollider2DComponent>();
					Shared<Collider2D> collider = physicsBody->GetColliders()[bcComponent.ColliderIndex];
					bcComponent.Sensor = isSensor;
					collider->SetSensor(isSensor);
					break;
				}
				case ColliderType2D::Circle:
				{
					CircleCollider2DComponent& ccComponent = entity.GetComponent<CircleCollider2DComponent>();
					Shared<Collider2D> collider = physicsBody->GetColliders()[ccComponent.ColliderIndex];
					ccComponent.Sensor = isSensor;
					collider->SetSensor(isSensor);
					break;
				}
				case ColliderType2D::Capsule:
				{
					CapsuleCollider2DComponent& ccComponent = entity.GetComponent<CapsuleCollider2DComponent>();
					Shared<Collider2D> collider = physicsBody->GetColliders()[ccComponent.ColliderIndex];
					ccComponent.Sensor = isSensor;
					collider->SetSensor(isSensor);
					break;
				}
				case ColliderType2D::None:
				{
					Shared<Collider2D> collider = physicsBody->GetColliders()[0];

					if (collider)
						collider->SetSensor(isSensor);

					switch (collider->GetType())
					{
					case ColliderType2D::Box: 
					{
						BoxCollider2DComponent& bcComponent = entity.GetComponent<BoxCollider2DComponent>();
						bcComponent.Sensor = isSensor;
						break;
					}
					case ColliderType2D::Circle: 
					{
						CircleCollider2DComponent& ccComponent = entity.GetComponent<CircleCollider2DComponent>();
						ccComponent.Sensor = isSensor;
						break;
					}
					case ColliderType2D::Capsule:
					{
						CapsuleCollider2DComponent& ccComponent = entity.GetComponent<CapsuleCollider2DComponent>();
						ccComponent.Sensor = isSensor;
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
			TR_PROFILE_FUNCTION();
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
			case ColliderType2D::Capsule: 
			{
				GET_COMPONENT_VAR(Offset, entityUUIDArr, CapsuleCollider2DComponent);
				break;
			}
			case ColliderType2D::None:
			{
                Entity entity = GetEntityFromMonoArray(entityUUIDArr);

				Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
				if (physicsBody)
				{

					Shared<Collider2D> collider;
					if (physicsBody->GetColliders().size() > 0)
						collider = physicsBody->GetColliders()[0];

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
						case ColliderType2D::Capsule:
						{
							Shared<CapsuleCollider2D> capsuleCollider = std::dynamic_pointer_cast<CapsuleCollider2D>(collider);
							if (capsuleCollider)
								Offset = capsuleCollider->GetOffset();

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
			TR_PROFILE_FUNCTION();
			glm::vec2 Offset = inOffset;
            Entity entity = GetEntityFromMonoArray(entityUUIDArr);
			Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);

			if (physicsBody) 
			{
				switch ((ColliderType2D)colliderType)
				{
				case ColliderType2D::Box: 
				{
					BoxCollider2DComponent& bcComponent = entity.GetComponent<BoxCollider2DComponent>();
					Shared<Collider2D> collider = physicsBody->GetColliders()[bcComponent.ColliderIndex];
					
					if(collider)
						collider->SetOffset(Offset);
					break;
				}
				case ColliderType2D::Circle: 
				{
					CircleCollider2DComponent& ccComponent = entity.GetComponent<CircleCollider2DComponent>();
					Shared<Collider2D> collider = physicsBody->GetColliders()[ccComponent.ColliderIndex];
					if (collider)
						collider->SetOffset(Offset);

					break;
				}
				case ColliderType2D::Capsule:
				{
					CapsuleCollider2DComponent& ccComponent = entity.GetComponent<CapsuleCollider2DComponent>();
					Shared<Collider2D> collider = physicsBody->GetColliders()[ccComponent.ColliderIndex];
					if (collider)
						collider->SetOffset(Offset);

					break;
				}
				case ColliderType2D::None: 
				{
					Shared<Collider2D> collider = physicsBody->GetColliders()[0];

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
					case ColliderType2D::Capsule:
					{
						CapsuleCollider2DComponent& ccComponent = entity.GetComponent<CapsuleCollider2DComponent>();
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
			TR_PROFILE_FUNCTION();
			glm::vec2 Size = { 0.0f, 0.0f };
			GET_COMPONENT_VAR(Size, entityUUIDArr, BoxCollider2DComponent);
			size = Size;
		}

		void BoxCollider2D_SetSize(MonoArray* entityUUIDArr, const glm::vec2& size)
		{
			TR_PROFILE_FUNCTION();
			glm::vec2 Size = size;
			SET_COMPONENT_VAR(Size, entityUUIDArr, BoxCollider2DComponent);
			Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);

			if (physicsBody) 
			{
				BoxCollider2DComponent& bcComponent = entity.GetComponent<BoxCollider2DComponent>();
				Shared<Collider2D> collider = physicsBody->GetColliders()[bcComponent.ColliderIndex];
				Shared<BoxCollider2D> boxCollider = DynamicCast<BoxCollider2D>(collider);

				if (boxCollider)
					boxCollider->SetSize(Size);
			}
		}
		// -------------------------

		// ---- Circle Collider 2D ----
		float CircleCollider2D_GetRadius(MonoArray* entityUUIDArr)
		{
			TR_PROFILE_FUNCTION();
			float Radius = 0.0f;
			GET_COMPONENT_VAR(Radius, entityUUIDArr, CircleCollider2DComponent);
			return Radius;
		}

		void CircleCollider2D_SetRadius(MonoArray* entityUUIDArr, float radius)
		{
			TR_PROFILE_FUNCTION();
			float Radius = radius;
			SET_COMPONENT_VAR(Radius, entityUUIDArr, CircleCollider2DComponent);
			Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);

			if (physicsBody) 
			{
				CircleCollider2DComponent& ccComponent = entity.GetComponent<CircleCollider2DComponent>();
				Shared<Collider2D> collider = physicsBody->GetColliders()[ccComponent.ColliderIndex];
				Shared<CircleCollider2D> circleCollider = DynamicCast<CircleCollider2D>(collider);

				if (circleCollider)
					circleCollider->SetRadius(Radius);
			}
		}
		// ----------------------------
        
		// ---- Capsule Collider 2D ----
		void CapsuleCollider2D_GetSize(MonoArray* entityUUIDArr, glm::vec2& size) 
		{
			TR_PROFILE_FUNCTION();
			glm::vec2 Size = { 0.0f, 0.0f };
			GET_COMPONENT_VAR(Size, entityUUIDArr, CapsuleCollider2DComponent);
			size = Size;
		}

		void CapsuleCollider2D_SetSize(MonoArray* entityUUIDArr, const glm::vec2& size) 
		{
			TR_PROFILE_FUNCTION();
			glm::vec2 Size = size;
			SET_COMPONENT_VAR(Size, entityUUIDArr, CapsuleCollider2DComponent);
			Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);

			if (physicsBody) 
			{
				CapsuleCollider2DComponent& ccComponent = entity.GetComponent<CapsuleCollider2DComponent>();
				Shared<Collider2D> collider = physicsBody->GetCollider(ccComponent.ColliderIndex);

				Shared<CapsuleCollider2D> capsuleCollider = DynamicCast<CapsuleCollider2D>(collider);

				if (capsuleCollider)
					capsuleCollider->SetSize(size);
			}
		}

		// -----------------------------

        // ---- Layer Mask ----
        MonoString* LayerMask_GetName(uint16_t layer)
        {
			TR_PROFILE_FUNCTION();
            int index = layer >> 1;
            PhysicsLayer& physicsLayer = PhysicsLayerManager::GetLayer(index);
            return ScriptMarshal::UTF8ToMonoString(physicsLayer.Name);
        }
        // --------------------

		// ---- Log ----
		void Log_Log(uint8_t logLevel, MonoString* monoMessage)
		{
			TR_PROFILE_FUNCTION();
			std::string message = ScriptMarshal::MonoStringToUTF8(monoMessage);
			TR_TRACE(message);
			switch (logLevel)
			{
			case 1 << 0: TR_CLIENT_TRACE(message); break;
			case 1 << 1: TR_CLIENT_WARN(message); break;
			case 1 << 2: TR_CLIENT_ERROR(message); break;
			}
		}
		// -------------

		// ---- Input ----
		bool Input_KeyPressed(Key keyCode)	{ TR_PROFILE_FUNCTION(); return Input::IsKeyPressed(keyCode); }
		bool Input_KeyDown(Key keyCode)		{ TR_PROFILE_FUNCTION(); return Input::IsKeyDown(keyCode); }
		bool Input_KeyReleased(Key keyCode) { TR_PROFILE_FUNCTION(); return Input::IsKeyReleased(keyCode); }

		bool Input_MouseButtonPressed(MouseButton mouseButton)		{ TR_PROFILE_FUNCTION(); return Input::IsMouseButtonPressed(mouseButton); }
		bool Input_MouseButtonDown(MouseButton mouseButton)			{ TR_PROFILE_FUNCTION(); return Input::IsMouseButtonDown(mouseButton); }
		bool Input_MouseButtonReleased(MouseButton mouseButton)		{ TR_PROFILE_FUNCTION(); return Input::IsMouseButtonReleased(mouseButton); }
		void Input_GetMousePosition(glm::vec2& outMousePosition)	{ TR_PROFILE_FUNCTION(); outMousePosition = Input::GetMousePos(); }

		bool Input_IsControllerConnected(uint8_t controllerIndex) { TR_PROFILE_FUNCTION(); return Input::IsControllerConnected(controllerIndex); }

		MonoString* Input_GetControllerName(uint8_t controllerIndex)
		{
			TR_PROFILE_FUNCTION();
			const char* controllerName = Input::GetControllerName(controllerIndex);
			return ScriptMarshal::UTF8ToMonoString(controllerName);
		}

		bool Input_IsControllerButtonPressed(ControllerButton controllerButton, uint8_t controllerIndex) 
		{ TR_PROFILE_FUNCTION(); return Input::IsControllerButtonPressed(controllerButton, controllerIndex); }
		float Input_GetControllerAxis(ControllerAxis controllerAxis, uint8_t controllerIndex) 
		{ TR_PROFILE_FUNCTION(); return Input::GetControllerAxis(controllerAxis, controllerIndex); }

		MonoArray* Input_GetConnectedControllers()
		{
			TR_PROFILE_FUNCTION();
			std::vector<uint8_t> connectedControllers = Input::GetConnectedControllers();
			ScriptArray connectedControllersArr = ScriptArray::Create<uint8_t>(static_cast<uint32_t>(connectedControllers.size()));

			for (size_t i = 0; i < connectedControllers.size(); i++)
				connectedControllersArr.Set<uint8_t>(static_cast<uint32_t>(i), static_cast<uint8_t>(i));

			return connectedControllersArr.GetMonoArray();
		}

		// ---------------
#endif
}

