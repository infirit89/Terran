#include "trpch.h"
#include "ScriptBindings.h"

#include "ScriptEngine.h"
#include "ScriptTypes.h"

#include "Core/Input.h"
#include "Core/Application.h"

#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Scene/SceneManager.h"
#include "Scene/Systems/SceneRenderer.h"

#include "Physics/Physics.h"
#include "Physics/PhysicsBody.h"
#include "Physics/Collider.h"
#include "Physics/PhysicsLayerManager.h"

#include "Utils/Debug/OptickProfiler.h"

#include <functional>
#include <glm/glm.hpp>

#include <box2d/box2d.h>

#include <Coral/Type.hpp>
#include <Coral/TypeCache.hpp>

#include <imgui.h>

namespace TerranEngine 
{
#define BIND_INTERNAL_FUNC(func) assembly.AddInternalCall("Terran.Internal", #func, reinterpret_cast<void*>(&func))

	static std::unordered_map<int32_t, std::function<bool(Entity)>> s_HasComponentFuncs;
	static std::unordered_map<int32_t, std::function<void(Entity)>> s_AddComponentFuncs;
	static std::unordered_map<int32_t, std::function<void(Entity)>> s_RemoveComponentFuncs;
	
#define REGISTER_COMPONENT(ComponentType, Component)																				\
	Coral::Type type_##ComponentType = assembly.GetType("Terran."#ComponentType);													\
	TR_ASSERT(type_##ComponentType, "Invalid type");																				\
	s_HasComponentFuncs.emplace(type_##ComponentType.GetTypeId(), [](Entity entity) { return entity.HasComponent<Component>(); });	\
	s_AddComponentFuncs.emplace(type_##ComponentType.GetTypeId(), [](Entity entity) { entity.AddComponent<Component>(); });			\
	s_RemoveComponentFuncs.emplace(type_##ComponentType.GetTypeId(), [](Entity entity) { entity.RemoveComponent<Component>(); })


	void ScriptBindings::Bind(Coral::ManagedAssembly& assembly)
	{
		REGISTER_COMPONENT(Transform, TransformComponent);
		REGISTER_COMPONENT(Tag, TagComponent);
		REGISTER_COMPONENT(CapsuleCollider2D, CapsuleCollider2DComponent);
		REGISTER_COMPONENT(CircleCollider2D, CircleCollider2DComponent);
		REGISTER_COMPONENT(BoxCollider2D, BoxCollider2DComponent);
		REGISTER_COMPONENT(Rigidbody2D, Rigidbody2DComponent);
		REGISTER_COMPONENT(Camera, CameraComponent);
		REGISTER_COMPONENT(SpriteRenderer, SpriteRendererComponent);
		REGISTER_COMPONENT(CircleRenderer, CircleRendererComponent);
		REGISTER_COMPONENT(TextRenderer, TextRendererComponent);

		BIND_INTERNAL_FUNC(Log_LogICall);

		// ---- Input ----
		#pragma region Input
		BIND_INTERNAL_FUNC(Input_KeyDownICall);
		BIND_INTERNAL_FUNC(Input_KeyPressedICall);
		BIND_INTERNAL_FUNC(Input_KeyReleasedICall);

		BIND_INTERNAL_FUNC(Input_MouseButtonDownICall);
		BIND_INTERNAL_FUNC(Input_MouseButtonPressedICall);
		BIND_INTERNAL_FUNC(Input_MouseButtonReleasedICall);
		BIND_INTERNAL_FUNC(Input_GetMousePositionICall);

		BIND_INTERNAL_FUNC(Input_IsControllerConnectedICall);
		BIND_INTERNAL_FUNC(Input_GetControllerNameICall);
		BIND_INTERNAL_FUNC(Input_IsControllerButtonPressedICall);
		BIND_INTERNAL_FUNC(Input_GetControllerAxisICall);
		BIND_INTERNAL_FUNC(Input_GetConnectedControllersICall);
		#pragma endregion
		// ---------------

		// ---- Entity ----
		#pragma region Entity
		BIND_INTERNAL_FUNC(Entity_HasComponentICall);
		BIND_INTERNAL_FUNC(Entity_AddComponentICall);
		BIND_INTERNAL_FUNC(Entity_RemoveComponentICall);
		BIND_INTERNAL_FUNC(Entity_GetScriptableComponentICall);
		BIND_INTERNAL_FUNC(Entity_DestroyEntityICall);
		BIND_INTERNAL_FUNC(Entity_FindEntityWithNameICall);
		BIND_INTERNAL_FUNC(Entity_GetChildrenCountICall);
		BIND_INTERNAL_FUNC(Entity_GetChildICall);
		#pragma endregion
		// ----------------

		// ---- Tag Component ----
		#pragma region Tag Component
		BIND_INTERNAL_FUNC(Tag_GetNameICall);
		BIND_INTERNAL_FUNC(Tag_SetNameICall);
		#pragma endregion
		// -------------

		// ---- Transform Component ----
		#pragma region Transform Component
		BIND_INTERNAL_FUNC(Transform_GetPositionICall);
		BIND_INTERNAL_FUNC(Transform_SetPositionICall);
		BIND_INTERNAL_FUNC(Transform_GetRotationICall);
		BIND_INTERNAL_FUNC(Transform_SetRotationICall);
		BIND_INTERNAL_FUNC(Transform_GetScaleICall);
		BIND_INTERNAL_FUNC(Transform_SetScaleICall);
		BIND_INTERNAL_FUNC(Transform_IsDirtyICall);
		BIND_INTERNAL_FUNC(Transform_GetForwardICall);
		BIND_INTERNAL_FUNC(Transform_GetUpICall);
		BIND_INTERNAL_FUNC(Transform_GetRightICall);
		#pragma endregion
		// -----------------------------

		// ---- Sprite Renderer Component ----
		#pragma region Sprite Renderer Component
		BIND_INTERNAL_FUNC(SpriteRenderer_GetColorICall);
		BIND_INTERNAL_FUNC(SpriteRenderer_SetColorICall);
		#pragma endregion
		// -----------------------------------

		// ---- Camera Component ----
		#pragma region Camera Component
		BIND_INTERNAL_FUNC(Camera_IsPrimaryICall);
		BIND_INTERNAL_FUNC(Camera_SetPrimaryICall);
		BIND_INTERNAL_FUNC(Camera_GetBackgroundColorICall);
		BIND_INTERNAL_FUNC(Camera_SetBackgroundColorICall);
		#pragma endregion
		// --------------------------

		// ---- Circle Renderer Component ----
		#pragma region Circle Renderer Component
		BIND_INTERNAL_FUNC(CircleRenderer_GetColorICall);
		BIND_INTERNAL_FUNC(CircleRenderer_SetColorICall);
		BIND_INTERNAL_FUNC(CircleRenderer_GetThicknessICall);
		BIND_INTERNAL_FUNC(CircleRenderer_SetThicknessICall);
		#pragma endregion
		// -----------------------------------

		// ---- Text Renderer Component ----
		#pragma region Text Renderer Component
		BIND_INTERNAL_FUNC(TextRenderer_GetColorICall);
		BIND_INTERNAL_FUNC(TextRenderer_SetColorICall);
		BIND_INTERNAL_FUNC(TextRenderer_GetTextICall);
		BIND_INTERNAL_FUNC(TextRenderer_SetTextICall);
		#pragma endregion
		// ---------------------------------

		// ---- Physics ----
		#pragma region Physics
		BIND_INTERNAL_FUNC(LayerMask_GetNameICall);
		BIND_INTERNAL_FUNC(Physics2D_RayCastICall);
		BIND_INTERNAL_FUNC(Physics2D_RayCastAllICall);

		// ---- Rigidbody 2D ----
		#pragma region Rigidbody 2D
		BIND_INTERNAL_FUNC(Rigidbody2D_IsFixedRotationICall);
		BIND_INTERNAL_FUNC(Rigidbody2D_SetFixedRotationICall);

		BIND_INTERNAL_FUNC(Rigidbody2D_GetSleepStateICall);
		BIND_INTERNAL_FUNC(Rigidbody2D_SetSleepStateICall);

		BIND_INTERNAL_FUNC(Rigidbody2D_GetGravityScaleICall);
		BIND_INTERNAL_FUNC(Rigidbody2D_SetGravityScaleICall);

		BIND_INTERNAL_FUNC(Rigidbody2D_ApplyForceICall);
		BIND_INTERNAL_FUNC(Rigidbody2D_ApplyForceAtCenterICall);

		BIND_INTERNAL_FUNC(Rigidbody2D_GetLinearVelocityICall);
		BIND_INTERNAL_FUNC(Rigidbody2D_SetLinearVelocityICall);

		BIND_INTERNAL_FUNC(Rigidbody2D_GetAngularVelocityICall);
		BIND_INTERNAL_FUNC(Rigidbody2D_SetAngularVelocityICall);

		BIND_INTERNAL_FUNC(Rigidbody2D_GetTypeICall);
		BIND_INTERNAL_FUNC(Rigidbody2D_SetTypeICall);
		#pragma endregion
		// ----------------------

		// ---- Collider 2D ----
		#pragma region Collider 2D
		BIND_INTERNAL_FUNC(Collider2D_GetOffsetICall);
		BIND_INTERNAL_FUNC(Collider2D_SetOffsetICall);

		BIND_INTERNAL_FUNC(Collider2D_IsSensorICall);
		BIND_INTERNAL_FUNC(Collider2D_SetSensorICall);
		#pragma endregion
		// ---------------------

		// ---- Box Collider 2D ----
		#pragma region Box Collider 2D
		BIND_INTERNAL_FUNC(BoxCollider2D_GetSizeICall);
		BIND_INTERNAL_FUNC(BoxCollider2D_SetSizeICall);
		#pragma endregion
		// -------------------------

		// ---- Circle Collider 2D ----
		#pragma region Circle Collider 2D
		BIND_INTERNAL_FUNC(CircleCollider2D_GetRadiusICall);
		BIND_INTERNAL_FUNC(CircleCollider2D_SetRadiusICall);
		#pragma endregion
		// ----------------------------

		// ---- Capsule Collider 2D ----
		#pragma region Capsule Collider 2D
		BIND_INTERNAL_FUNC(CapsuleCollider2D_GetSizeICall);
		BIND_INTERNAL_FUNC(CapsuleCollider2D_SetSizeICall);
		#pragma endregion
		// -----------------------------

		#pragma endregion
		// -----------------

		// ---- Window ----
		#pragma region Window
		BIND_INTERNAL_FUNC(Window_GetWidthICall);
		BIND_INTERNAL_FUNC(Window_GetHeightICall);
		BIND_INTERNAL_FUNC(Window_IsVSyncICall);
		BIND_INTERNAL_FUNC(Window_GetContentScaleICall);
		#pragma endregion
		// ----------------

		// ---- Scene ----
		#pragma region Scene
		BIND_INTERNAL_FUNC(Scene_GetMainCameraICall);
		#pragma endregion
		// ---------------

		assembly.UploadInternalCalls();
	}

	void ScriptBindings::Unbind()
	{
		s_HasComponentFuncs.clear();
		s_AddComponentFuncs.clear();
		s_RemoveComponentFuncs.clear();
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

	// ---- Input ----
	#pragma region Input
	void ScriptBindings::Input_KeyPressedICall(Key keyCode, bool& isPressed)
	{
		isPressed = Input::IsKeyPressed(keyCode);
	}

	void ScriptBindings::Input_KeyDownICall(Key keyCode, bool& isDown)
	{
		isDown = Input::IsKeyDown(keyCode);
	}

	void ScriptBindings::Input_KeyReleasedICall(Key keyCode, bool& isReleased)
	{
		isReleased = Input::IsKeyReleased(keyCode);
	}

	bool ScriptBindings::Input_MouseButtonPressedICall(MouseButton mouseButton)
	{
		return Input::IsMouseButtonPressed(mouseButton);
	}

	bool ScriptBindings::Input_MouseButtonDownICall(MouseButton mouseButton)
	{
		return Input::IsMouseButtonDown(mouseButton);
	}

	bool ScriptBindings::Input_MouseButtonReleasedICall(MouseButton mouseButton)
	{
		return Input::IsMouseButtonReleased(mouseButton);
	}

	void ScriptBindings::Input_GetMousePositionICall(glm::vec2& outMousePosition)
	{
		glm::vec2 viewportPosition = SceneManager::GetCurrentScene()->GetViewportPosition();
		outMousePosition = Input::GetMousePos() - viewportPosition;
	}

	bool ScriptBindings::Input_IsControllerConnectedICall(uint8_t controllerIndex)
	{
		return Input::IsControllerConnected(controllerIndex);
	}

	Coral::String ScriptBindings::Input_GetControllerNameICall(uint8_t controllerIndex)
	{
		return Coral::String::New(Input::GetControllerName(controllerIndex));
	}
	bool ScriptBindings::Input_IsControllerButtonPressedICall(uint8_t controllerIndex, ControllerButton controllerButton)
	{
		return Input::IsControllerButtonPressed(controllerButton, controllerIndex);
	}
	float ScriptBindings::Input_GetControllerAxisICall(uint8_t controllerIndex, ControllerAxis controllerAxis)
	{
		return Input::GetControllerAxis(controllerAxis, controllerIndex);
	}
	void* ScriptBindings::Input_GetConnectedControllersICall()
	{
		std::vector<uint8_t> connectedControllers = Input::GetConnectedControllers();
		Coral::Type* byteType = Coral::TypeCache::Get().GetTypeByName("System.Byte");
		Coral::ManagedArray connectedControllersArray = Coral::ManagedArray::New(*byteType, connectedControllers.size());
		for (size_t i = 0; i < connectedControllers.size(); i++)
			connectedControllersArray.SetValue(static_cast<int32_t>(i), connectedControllers.at(i));

		return connectedControllersArray.GetHandle();
	}

	#pragma endregion
	// ---------------

	// ---- Entity ----
	#pragma region Entity
#define GET_ENTITY()															\
	Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id)

	bool ScriptBindings::Entity_HasComponentICall(const UUID& id, int32_t typeId)
	{
		TR_PROFILE_FUNCTION();
		GET_ENTITY();
		if (!entity)
			return false;

		if (s_HasComponentFuncs.find(typeId) != s_HasComponentFuncs.end())
			return s_HasComponentFuncs.at(typeId)(entity);

		Coral::Type* type = Coral::TypeCache::Get().GetTypeByID(typeId);
		if (!type)
			return false;

		if (type->IsSubclassOf(*ScriptTypes::ScriptableType) && entity.HasComponent<ScriptComponent>()) 
		{
			Coral::ScopedString typeName = type->GetFullName();
			return entity.GetComponent<ScriptComponent>().ModuleName == typeName;
		}

		return false;
	}

	void ScriptBindings::Entity_AddComponentICall(const UUID& id, int32_t typeId)
	{
		TR_PROFILE_FUNCTION();
		GET_ENTITY();
		if (!entity)
			return;

		if (s_AddComponentFuncs.find(typeId) != s_AddComponentFuncs.end()) 
		{
			s_AddComponentFuncs.at(typeId)(entity);
			return;
		}

		Coral::Type* type = Coral::TypeCache::Get().GetTypeByID(typeId);
		if (type->IsSubclassOf(*ScriptTypes::ScriptableType))
		{
			Coral::ScopedString typeName = type->GetFullName();
			entity.AddComponent<ScriptComponent>((std::string)typeName);
		}
	}

	void ScriptBindings::Entity_RemoveComponentICall(const UUID& id, int32_t typeId)
	{
		TR_PROFILE_FUNCTION();
		GET_ENTITY();
		if (!entity)
			return;

		if (s_RemoveComponentFuncs.find(typeId) != s_RemoveComponentFuncs.end())
			s_RemoveComponentFuncs.at(typeId)(entity);

		// TODO: handle removing script component
	}

	void* ScriptBindings::Entity_GetScriptableComponentICall(const UUID& id)
	{
		TR_PROFILE_FUNCTION();
		GET_ENTITY();
		if (!entity)
			return nullptr;

		Shared<ScriptInstance> scriptInstance = ScriptEngine::GetScriptInstance(entity);
		return (void*)scriptInstance->GetHandle();
	}

	bool ScriptBindings::Entity_FindEntityWithNameICall(Coral::String entityName, UUID& id)
	{
		Entity entity = SceneManager::GetCurrentScene()->FindEntityWithName(entityName);

		if (!entity)
			return false;

		TR_TRACE(entity.GetID());
		id = entity.GetID();
		return true;
	}

	void ScriptBindings::Entity_DestroyEntityICall(const UUID& id)
	{
		TR_PROFILE_FUNCTION();
		GET_ENTITY();

		if (entity)
			SceneManager::GetCurrentScene()->DestroyEntity(entity, true);
		else
			TR_CLIENT_ERROR("Can't destroy entity because it doesnt exist");
	}

	int ScriptBindings::Entity_GetChildrenCountICall(const UUID& id)
	{
		TR_PROFILE_FUNCTION();
		GET_ENTITY();
		if (!entity)
			return 0;

		return entity.GetChildCount();
		/*if (entity.GetChildCount() <= 0)
			return nullptr;

		Coral::ManagedArray childrenIds = Coral::ManagedArray::New(*ScriptTypes::IdType, entity.GetChildCount());
		
		int i = 0;
		for (const UUID& id : entity.GetChildren())
		{
			childrenIds.SetValueRaw(i, (UUID*)&id);
			i++;
		}

		return childrenIds.GetHandle();*/
	}

	UUID ScriptBindings::Entity_GetChildICall(const UUID& id, int index)
	{
		TR_PROFILE_FUNCTION();
		GET_ENTITY();
		if (!entity)
			return UUID::Invalid();

		return entity.GetChild(index).GetID();
	}
	#pragma endregion
	// ----------------

	// ---- Tag Component ----
	#pragma region Tag Component
	Coral::String ScriptBindings::Tag_GetNameICall(const UUID& id)
	{
		GET_ENTITY();
		if (!entity)
			return Coral::String::New("");

		return Coral::String::New(entity.GetName());
	}

	void ScriptBindings::Tag_SetNameICall(const UUID& id, Coral::String name)
	{
		GET_ENTITY();
		if (!entity)
			return;

		entity.GetComponent<TagComponent>().Name = name;
	}
	#pragma endregion
	// -----------------------

#define SET_TRANSFORM_COMPONENT_PROPERTY(Property, Value)\
	Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);\
	if(!entity)\
	{\
		TR_ERROR("Invalid entity id");\
		return;\
	}\
	TransformComponent& component = entity.GetComponent<TransformComponent>();\
	component.Property = Value;\
	component.IsDirty = true

// bullshit?
#define SET_COMPONENT_PROPERTY(ComponentType, Property, Value)					\
	Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);	\
	if(!entity)																	\
	{																			\
		TR_ERROR("Invalid entity id");											\
		return;																	\
	}																			\
	entity.GetComponent<ComponentType>().Property = Value

// bullshit #2? 
#define GET_COMPONENT_PROPERTY(ComponentType, Property, DefaultValue)			\
	Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);	\
	if(entity)																	\
		return entity.GetComponent<ComponentType>().Property;					\
																				\
	TR_ERROR("Invalid entity id");												\
	return DefaultValue;

#define GET_COMPONENT_PROPERTY_NORET(ComponentType, Property, Value)			\
	Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);	\
	if(entity)																	\
	{																			\
		Value = entity.GetComponent<ComponentType>().Property;					\
		return;																	\
	}																			\
																				\
	TR_ERROR("Invalid entity id")

#define SET_TRANSFORM_COMPONENT_PROPERTY(Property, Value)\
	GET_ENTITY();\
	if(!entity)\
	{\
		TR_ERROR("Invalid entity id");\
		return;\
	}\
	TransformComponent& tc = entity.GetComponent<TransformComponent>();\
	tc.Property = Value;\
	tc.IsDirty = true

	// ---- Transform Component ----
	#pragma region Transform Component
	glm::vec3 ScriptBindings::Transform_GetPositionICall(const UUID& id) 
	{
		GET_COMPONENT_PROPERTY(TransformComponent, Position, glm::vec3(0.0f, 0.0f, 0.0f));
	}

	void ScriptBindings::Transform_SetPositionICall(const UUID& id, const glm::vec3& position)
	{
		//SET_COMPONENT_PROPERTY(TransformComponent, Position, position);
		SET_TRANSFORM_COMPONENT_PROPERTY(Position, position);
	}

	glm::vec3 ScriptBindings::Transform_GetRotationICall(const UUID& id) 
	{
		GET_COMPONENT_PROPERTY(TransformComponent, Rotation, glm::vec3(0.0f, 0.0f, 0.0f));
	}

	void ScriptBindings::Transform_SetRotationICall(const UUID& id, const glm::vec3& rotation)
	{
		SET_TRANSFORM_COMPONENT_PROPERTY(Rotation, rotation);
	}

	glm::vec3 ScriptBindings::Transform_GetScaleICall(const UUID& id) 
	{
		GET_COMPONENT_PROPERTY(TransformComponent, Scale, glm::vec3(1.0f, 1.0f, 1.0f));
	}

	void ScriptBindings::Transform_SetScaleICall(const UUID& id, const glm::vec3& scale)
	{
		SET_TRANSFORM_COMPONENT_PROPERTY(Scale, scale);
	}

	bool ScriptBindings::Transform_IsDirtyICall(const UUID& id) 
	{
		GET_COMPONENT_PROPERTY(TransformComponent, IsDirty, false);
	}

	glm::vec3 ScriptBindings::Transform_GetForwardICall(const UUID& id) 
	{
		GET_COMPONENT_PROPERTY(TransformComponent, Forward, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	glm::vec3 ScriptBindings::Transform_GetUpICall(const UUID& id) 
	{
		GET_COMPONENT_PROPERTY(TransformComponent, Up, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	glm::vec3 ScriptBindings::Transform_GetRightICall(const UUID& id) 
	{
		GET_COMPONENT_PROPERTY(TransformComponent, Right, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	#pragma endregion
	// -----------------------------

	// ---- Sprite Renderer Component ----
	#pragma region Sprite Renderer Component
	glm::vec4 ScriptBindings::SpriteRenderer_GetColorICall(const UUID& id) 
	{
		GET_COMPONENT_PROPERTY(SpriteRendererComponent, Color, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	void ScriptBindings::SpriteRenderer_SetColorICall(const UUID& id, const glm::vec4& color)
	{
		SET_COMPONENT_PROPERTY(SpriteRendererComponent, Color, color);
	}
	#pragma endregion
	// -----------------------------------

	// ---- Camera Component ----
	#pragma region Camera Component
	bool ScriptBindings::Camera_IsPrimaryICall(const UUID& id) 
	{
		GET_COMPONENT_PROPERTY(CameraComponent, Primary, false);
	}
	void ScriptBindings::Camera_SetPrimaryICall(const UUID& id, bool primary)
	{
		SET_COMPONENT_PROPERTY(CameraComponent, Primary, primary);
	}

	glm::vec4 ScriptBindings::Camera_GetBackgroundColorICall(const UUID& id) 
	{
		GET_COMPONENT_PROPERTY(CameraComponent, BackgroundColor, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
	}
	void ScriptBindings::Camera_SetBackgroundColorICall(const UUID& id, const glm::vec4& backgroundColor)
	{
		SET_COMPONENT_PROPERTY(CameraComponent, BackgroundColor, backgroundColor);
	}
	glm::vec3 ScriptBindings::Camera_ScreenToWorldPointICall(const UUID& id, const glm::vec3& point)
	{
		GET_ENTITY();
		auto& tc = entity.GetComponent<TransformComponent>();
		auto& camera = entity.GetComponent<CameraComponent>();

		return camera.Camera.ScreenToWorld(tc.WorldSpaceTransformMatrix, point);
	}
	#pragma endregion
	// ----------------

	// ---- Circle Renderer Component ----
	#pragma region Circle Renderer Component
	glm::vec4 ScriptBindings::CircleRenderer_GetColorICall(const UUID& id) 
	{
		GET_COMPONENT_PROPERTY(CircleRendererComponent, Color, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	void ScriptBindings::CircleRenderer_SetColorICall(const UUID& id, const glm::vec4& color) 
	{
		SET_COMPONENT_PROPERTY(CircleRendererComponent, Color, color);
	}

	float ScriptBindings::CircleRenderer_GetThicknessICall(const UUID& id) 
	{
		GET_COMPONENT_PROPERTY(CircleRendererComponent, Thickness, 1.0f);
	}

	void ScriptBindings::CircleRenderer_SetThicknessICall(const UUID& id, float thickness) 
	{
		SET_COMPONENT_PROPERTY(CircleRendererComponent, Thickness, thickness);
	}
	#pragma endregion
	// -----------------------------------

	// ---- Text Renderer Component ----
	#pragma region Text Renderer Component
	glm::vec4 ScriptBindings::TextRenderer_GetColorICall(const UUID& id)
	{
		GET_COMPONENT_PROPERTY(TextRendererComponent, TextColor, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	void ScriptBindings::TextRenderer_SetColorICall(const UUID& id, const glm::vec4& color) 
	{
		SET_COMPONENT_PROPERTY(TextRendererComponent, TextColor, color);
	}

	Coral::String ScriptBindings::TextRenderer_GetTextICall(const UUID& id) 
	{
		GET_ENTITY();
		if (!entity)
			return Coral::String::New("");

		return Coral::String::New(entity.GetComponent<TextRendererComponent>().Text);
	}
	void ScriptBindings::TextRenderer_SetTextICall(const UUID& id, Coral::String text) 
	{
		GET_ENTITY();
		if (!entity)
			return;

		entity.GetComponent<TextRendererComponent>().Text = text;
	}
	#pragma endregion
	// ---------------------------------

	// ---- Window ----
	#pragma region Window
	float ScriptBindings::Window_GetWidthICall() 
	{
		const Shared<Scene>& currentScene = SceneManager::GetCurrentScene();
		return currentScene->GetViewportWidth();
	}

	float ScriptBindings::Window_GetHeightICall()
	{
		const Shared<Scene>& currentScene = SceneManager::GetCurrentScene();
		return currentScene->GetViewportHeight();
	}

	bool ScriptBindings::Window_IsVSyncICall()
	{
		return Application::Get()->GetWindow().IsVsync();
	}

	glm::vec2 ScriptBindings::Window_GetContentScaleICall()
	{
		return Application::Get()->GetWindow().GetContentScale();
	}
	#pragma endregion
	// ----------------

	// ---- Scene ----
	#pragma region Scene
	UUID ScriptBindings::Scene_GetMainCameraICall()
	{
		const Shared<Scene>& currentScene = SceneManager::GetCurrentScene();
		Entity primaryCamera = currentScene->GetPrimaryCamera();
		return primaryCamera.GetID();
	}
	#pragma endregion
	// ---------------

	// ---- Physics ----
	Coral::String ScriptBindings::LayerMask_GetNameICall(uint16_t layer)
	{
		int index = layer >> 1;
		PhysicsLayer& physicsLayer = PhysicsLayerManager::GetLayer(index);
		return Coral::String::New(physicsLayer.Name);
	}


	// ---- Physics 2D ----
	#pragma region Physics 2D
	
	bool ScriptBindings::Physics2D_RayCastICall(const glm::vec2& origin, const glm::vec2& direction, float length, uint16_t layerMask, RayCastHitInfo2D_Internal& outHitInfo)
	{
		TR_PROFILE_FUNCTION();
		RayCastHitInfo2D hitInfo;
		bool hasHit = Physics2D::RayCast(origin, direction, length, hitInfo, layerMask);

		outHitInfo.Point = hitInfo.Point;
		outHitInfo.Normal = hitInfo.Normal;

		if (!hitInfo.PhysicsBody)
			return hasHit;

		Entity hitEntity = hitInfo.PhysicsBody->GetEntity();
		outHitInfo.Rigidbody = ScriptTypes::RigidbodyType->CreateInstance(hitEntity.GetID());

		return hasHit;
	}

	void* ScriptBindings::Physics2D_RayCastAllICall(const glm::vec2& origin, const glm::vec2& direction, float length, uint16_t layerMask) 
	{
		TR_PROFILE_FUNCTION();
		Shared<std::vector<RayCastHitInfo2D>> hitInfos = Physics2D::RayCastAll(origin, direction, length, layerMask);

		Coral::ManagedArray hitInfosArray = Coral::ManagedArray::New(*ScriptTypes::HitInfoType, hitInfos->size());
		for (size_t i = 0; i < hitInfos->size(); i++) 
		{
			RayCastHitInfo2D& hitInfo = hitInfos->at(i);
			RayCastHitInfo2D_Internal hitInfo_Internal;
			hitInfo_Internal.Normal = hitInfo.Normal;
			hitInfo_Internal.Point = hitInfo.Point;

			Entity hitEntity = hitInfo.PhysicsBody->GetEntity();
			hitInfo_Internal.Rigidbody = ScriptTypes::RigidbodyType->CreateInstance(hitEntity.GetID());
			
			hitInfosArray.SetValue(static_cast<int32_t>(i), hitInfo_Internal);
		}

		return hitInfosArray.GetHandle();
	}
	#pragma endregion
	// --------------------


	// ---- Rigidbody 2D ----
#define GET_RIGIDBODY_PROPERTY(Property, DefaultValue)						\
	GET_ENTITY();															\
	if (!entity)															\
		return DefaultValue;												\
	Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);	\
	return physicsBody->Get##Property()

#define SET_RIGIDBODY_COMPONENT_PROPERTY(Property, Value)						\
	do																			\
	{																			\
		SET_COMPONENT_PROPERTY(Rigidbody2DComponent, Property, Value);			\
		if (!entity)															\
			return;																\
		Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);	\
		physicsBody->Set##Property(Value);										\
	} while(0)

#define SET_RIGIDBODY_PROPERTY(Property, ...)									\
	do																			\
	{																			\
		GET_ENTITY();															\
		if (!entity)															\
			return;																\
		Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);	\
		physicsBody->Property(__VA_ARGS__);										\
	} while(0)


	#pragma region Rigidbody 2D
	bool ScriptBindings::Rigidbody2D_IsFixedRotationICall(const UUID& id) 
	{
		TR_PROFILE_FUNCTION();
		GET_RIGIDBODY_PROPERTY(FixedRotation, false);
	}

	void ScriptBindings::Rigidbody2D_SetFixedRotationICall(const UUID& id, bool fixedRotation) 
	{
		TR_PROFILE_FUNCTION();
		SET_RIGIDBODY_COMPONENT_PROPERTY(FixedRotation, fixedRotation);
	}

	PhysicsBodySleepState ScriptBindings::Rigidbody2D_GetSleepStateICall(const UUID& id) 
	{
		TR_PROFILE_FUNCTION();
		GET_RIGIDBODY_PROPERTY(SleepState, PhysicsBodySleepState::Awake);
	}
	void ScriptBindings::Rigidbody2D_SetSleepStateICall(const UUID& id, PhysicsBodySleepState sleepState)
	{
		TR_PROFILE_FUNCTION();
		SET_RIGIDBODY_COMPONENT_PROPERTY(SleepState, sleepState);
	}

	float ScriptBindings::Rigidbody2D_GetGravityScaleICall(const UUID& id) 
	{
		TR_PROFILE_FUNCTION();
		GET_RIGIDBODY_PROPERTY(GravityScale, 0.0f);
	}
	void ScriptBindings::Rigidbody2D_SetGravityScaleICall(const UUID& id, float gravityScale) 
	{
		TR_PROFILE_FUNCTION();
		SET_RIGIDBODY_COMPONENT_PROPERTY(GravityScale, gravityScale);
	}

	void ScriptBindings::Rigidbody2D_ApplyForceICall(const UUID& id, const glm::vec2& force, const glm::vec2& position, ForceMode2D forceMode)
	{
		TR_PROFILE_FUNCTION();
		SET_RIGIDBODY_PROPERTY(ApplyForce, force, position, forceMode);
	}
	void ScriptBindings::Rigidbody2D_ApplyForceAtCenterICall(const UUID& id, const glm::vec2& force, ForceMode2D forceMode)
	{
		TR_PROFILE_FUNCTION();
		SET_RIGIDBODY_PROPERTY(ApplyForceAtCenter, force, forceMode);
	}

	glm::vec2 ScriptBindings::Rigidbody2D_GetLinearVelocityICall(const UUID& id) 
	{
		TR_PROFILE_FUNCTION();
		GET_RIGIDBODY_PROPERTY(LinearVelocity, glm::vec2(0.0f, 0.0f));
	}
	void ScriptBindings::Rigidbody2D_SetLinearVelocityICall(const UUID& id, const glm::vec2& linearVelocity) 
	{
		TR_PROFILE_FUNCTION();
		SET_RIGIDBODY_PROPERTY(SetLinearVelocity, linearVelocity);
	}

	float ScriptBindings::Rigidbody2D_GetAngularVelocityICall(const UUID& id) 
	{
		TR_PROFILE_FUNCTION();
		GET_RIGIDBODY_PROPERTY(AngularVelocity, 0.0f);
	}
	void ScriptBindings::Rigidbody2D_SetAngularVelocityICall(const UUID& id, float angularVelocity) 
	{
		SET_RIGIDBODY_PROPERTY(SetAngularVelocity, angularVelocity);
	}

	PhysicsBodyType ScriptBindings::Rigidbody2D_GetTypeICall(const UUID& id)
	{
		GET_RIGIDBODY_PROPERTY(BodyType, PhysicsBodyType::Dynamic);
	}
	void ScriptBindings::Rigidbody2D_SetTypeICall(const UUID& id, PhysicsBodyType bodyType)
	{
		SET_RIGIDBODY_COMPONENT_PROPERTY(BodyType, bodyType);
	}
	#pragma endregion
	// ----------------------

#define GET_COLLIDER2D_PROPERTY(Property, ColliderProperty, Value, DefaultValue)	\
	switch (colliderType)															\
	{																				\
	case ColliderType2D::Box:														\
	{																				\
		GET_COMPONENT_PROPERTY_NORET(BoxCollider2DComponent, Property, Value);		\
		break;																		\
	}																				\
	case ColliderType2D::Circle:													\
	{																				\
		GET_COMPONENT_PROPERTY_NORET(CircleCollider2DComponent, Property, Value);	\
		break;																		\
	}																				\
	case ColliderType2D::Capsule:													\
	{																				\
		GET_COMPONENT_PROPERTY_NORET(CapsuleCollider2DComponent, Property, Value);	\
		break;																		\
	}																				\
	case ColliderType2D::None:														\
	{																				\
		GET_ENTITY();																\
		Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);		\
		if (!physicsBody)															\
			break;																	\
		if (physicsBody->GetColliders().size() > 0)									\
			Value = physicsBody->GetCollider(0)->ColliderProperty();				\
																					\
		break;																		\
	}																				\
	}																				\
	Value = DefaultValue

#define SET_COLLIDER2D_PROPERTY(Property, ColliderPropery, Value)										\
	GET_ENTITY();																						\
	Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);								\
	if (!physicsBody)																					\
		return;																							\
																										\
	uint32_t colliderIndex = 0;																			\
	Shared<Collider2D> collider = nullptr;																\
																										\
	if (colliderType == ColliderType2D::None)															\
		collider = physicsBody->GetCollider(0);															\
																										\
	switch (colliderType)																				\
	{																									\
	case ColliderType2D::Box:																			\
	{																									\
		BoxCollider2DComponent& bcComponent = entity.GetComponent<BoxCollider2DComponent>();			\
		colliderIndex = bcComponent.ColliderIndex;														\
		bcComponent.Property = Value;																	\
		break;																							\
	}																									\
	case ColliderType2D::Circle:																		\
	{																									\
		CircleCollider2DComponent& ccComponent = entity.GetComponent<CircleCollider2DComponent>();		\
		colliderIndex = ccComponent.ColliderIndex;														\
		ccComponent.Property = Value;																	\
		break;																							\
	}																									\
	case ColliderType2D::Capsule:																		\
	{																									\
		CapsuleCollider2DComponent& ccComponent = entity.GetComponent<CapsuleCollider2DComponent>();	\
		colliderIndex = ccComponent.ColliderIndex;														\
		ccComponent.Property = Value;																	\
		break;																							\
	}																									\
	}																									\
																										\
	if (colliderType != ColliderType2D::None)															\
		collider = physicsBody->GetCollider(colliderIndex);												\
																										\
	if (collider)																						\
		collider->ColliderPropery(Value)

	// ---- Collider 2D ----
	#pragma region Collider 2D
	void ScriptBindings::Collider2D_GetOffsetICall(const UUID& id, ColliderType2D colliderType, glm::vec2& offset) 
	{
		TR_PROFILE_FUNCTION();
		constexpr glm::vec2 defaultValue = { 0.0f, 0.0f };
		GET_COLLIDER2D_PROPERTY(Offset, GetOffset, offset, defaultValue);
	}
	void ScriptBindings::Collider2D_SetOffsetICall(const UUID& id, ColliderType2D colliderType, const glm::vec2& offset) 
	{
		TR_PROFILE_FUNCTION();
		SET_COLLIDER2D_PROPERTY(Offset, SetOffset, offset);
	}

	void ScriptBindings::Collider2D_IsSensorICall(const UUID& id, ColliderType2D colliderType, bool& sensor)
	{
		TR_PROFILE_FUNCTION();
		GET_COLLIDER2D_PROPERTY(Sensor, IsSensor, sensor, false);
	}

	void ScriptBindings::Collider2D_SetSensorICall(const UUID& id, ColliderType2D colliderType, bool isSensor) 
	{
		TR_PROFILE_FUNCTION();
		SET_COLLIDER2D_PROPERTY(Sensor, SetSensor, isSensor);
	}
	#pragma endregion
	// ---------------------

#define SET_COLLIDER_PROPERY(ColliderComponent, Property, Value)														\
	SET_COMPONENT_PROPERTY(ColliderComponent##Component, Property, Value);												\
	Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);												\
	if (!physicsBody)																									\
		return;																											\
																														\
	ColliderComponent##Component& bc = entity.GetComponent<ColliderComponent##Component>();								\
	Shared<ColliderComponent> collider = DynamicCast<ColliderComponent>(physicsBody->GetCollider(bc.ColliderIndex));	\
	if (!collider)																										\
		return;																											\
																														\
	collider->Set##Property(Value)

	// ---- Box Collider 2D ----
	#pragma region Box Collider 2D
	void ScriptBindings::BoxCollider2D_GetSizeICall(const UUID& id, glm::vec2& size)
	{
		TR_PROFILE_FUNCTION();
		GET_COMPONENT_PROPERTY_NORET(BoxCollider2DComponent, Size, size);
		size = { 0.0f, 0.0f };
	}
	void ScriptBindings::BoxCollider2D_SetSizeICall(const UUID& id, const glm::vec2& size) 
	{
		TR_PROFILE_FUNCTION();
		SET_COLLIDER_PROPERY(BoxCollider2D, Size, size);
	}
	#pragma endregion
	// -------------------------

	// ---- Circle Collider 2D ----
	#pragma region Circle Collider 2D
	float ScriptBindings::CircleCollider2D_GetRadiusICall(const UUID& id) 
	{
		TR_PROFILE_FUNCTION();
		GET_COMPONENT_PROPERTY(CircleCollider2DComponent, Radius, 0.0f);
	}
	void ScriptBindings::CircleCollider2D_SetRadiusICall(const UUID& id, float radius) 
	{
		TR_PROFILE_FUNCTION();
		SET_COLLIDER_PROPERY(CircleCollider2D, Radius, radius);
	}
	#pragma endregion
	// ----------------------------

	// ---- Capsule Collider 2D ----
	#pragma region Capsule Collider 2D
	void ScriptBindings::CapsuleCollider2D_GetSizeICall(const UUID& id, glm::vec2& size) 
	{
		TR_PROFILE_FUNCTION();
		GET_COMPONENT_PROPERTY_NORET(CapsuleCollider2DComponent, Size, size);
		size = { 0.0f, 0.0f };
	}
	void ScriptBindings::CapsuleCollider2D_SetSizeICall(const UUID& id, const glm::vec2& size) 
	{
		TR_PROFILE_FUNCTION();
		SET_COLLIDER_PROPERY(CapsuleCollider2D, Size, size);
	}
	#pragma endregion
	// -----------------------------

	// ------------------
}
