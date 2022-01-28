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

	void ComponentsBindings::SetTransformPosition_Internal(uint32_t entityRuntimeID, glm::vec3& position)
	{
	}

	void ComponentsBindings::GetTransformPosition_Internal(uint32_t entityRuntimeID, glm::vec3& position)
	{
	}

	void ComponentsBindings::SetTransformRotation_Internal(uint32_t entityRuntimeID, glm::vec3& position)
	{
	}

	void ComponentsBindings::GetTransformRotation_Internal(uint32_t entityRuntimeID, glm::vec3& position)
	{
	}

	void ComponentsBindings::SetTransformScale_Internal(uint32_t entityRuntimeID, glm::vec3& position)
	{
	}

	void ComponentsBindings::GetTransformScale_Internal(uint32_t entityRuntimeID, glm::vec3& position)
	{
	}
}

