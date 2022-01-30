#include "trpch.h"
#include "ComponentsScriptBindings.h"

#include "SceneManager.h"
#include "Entity.h"
#include "Components.h"

#include "Scripting/ScriptingEngine.h"

namespace TerranEngine 
{
	void ComponentsBindings::Bind()
	{

		ScriptingEngine::BindInternalFunc("TerranScriptCore.Transform::GetTransformPosition_Internal", GetTransformPosition_Internal);

		ScriptingEngine::BindInternalFunc("TerranScriptCore.Transform::SetTransformPosition_Internal",
			SetTransformPosition_Internal);

		
		ScriptingEngine::BindInternalFunc("TerranScriptCore.Transform::GetTransformRotation_Internal", GetTransformRotation_Internal);

		ScriptingEngine::BindInternalFunc("TerranScriptCore.Transform::SetTransformRotation_Internal",
			SetTransformRotation_Internal);


		ScriptingEngine::BindInternalFunc("TerranScriptCore.Transform::GetTransformScale_Internal", GetTransformScale_Internal);

		ScriptingEngine::BindInternalFunc("TerranScriptCore.Transform::SetTransformScale_Internal",
			SetTransformScale_Internal);

	}
	
// bullshit?
#define SET_TRANSFORM_VAR(var, entityID)\
	Entity entity((entt::entity)entityID, SceneManager::GetCurrentScene().get());\
	if(entity)\
		entity.GetComponent<TransformComponent>().var = var;\
	else\
		TR_ERROR("Invalid entity id");

// bullshit #2? 
#define GET_TRANSFORM_VAR(var, entityID)\
	Entity entity((entt::entity)entityID, SceneManager::GetCurrentScene().get());\
	if(entity)\
		var = entity.GetComponent<TransformComponent>().var;\
	else\
		TR_ERROR("Invalid entity id");

	// ---- Transform ----
	void ComponentsBindings::SetTransformPosition_Internal(uint32_t entityRuntimeID, glm::vec3& Position)
	{
		SET_TRANSFORM_VAR(Position, entityRuntimeID);
	}

	void ComponentsBindings::GetTransformPosition_Internal(uint32_t entityRuntimeID, glm::vec3& Position)
	{
		Position = { 0.0f, 0.0f, 0.0f };
		GET_TRANSFORM_VAR(Position, entityRuntimeID);
	}

	void ComponentsBindings::SetTransformRotation_Internal(uint32_t entityRuntimeID, glm::vec3& Rotation)
	{
		SET_TRANSFORM_VAR(Rotation, entityRuntimeID);
	}

	void ComponentsBindings::GetTransformRotation_Internal(uint32_t entityRuntimeID, glm::vec3& Rotation)
	{
		Rotation = { 0.0f, 0.0f, 0.0f };
		GET_TRANSFORM_VAR(Rotation, entityRuntimeID);
	}

	void ComponentsBindings::SetTransformScale_Internal(uint32_t entityRuntimeID, glm::vec3& Scale)
	{
		SET_TRANSFORM_VAR(Scale, entityRuntimeID);
	}

	void ComponentsBindings::GetTransformScale_Internal(uint32_t entityRuntimeID, glm::vec3& Scale)
	{
		Scale = { 0.0f, 0.0f, 0.0f };
		GET_TRANSFORM_VAR(Scale, entityRuntimeID);
	}
	// -------------------

}

