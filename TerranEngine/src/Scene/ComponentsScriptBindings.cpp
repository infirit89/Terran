#include "trpch.h"
#include "ComponentsScriptBindings.h"

#include "Scripting/ScriptingEngine.h"
#include "Scene.h"

namespace TerranEngine 
{
	struct Test 
	{
		glm::vec3 pos { 0.0f, 0.0f, 1.0f };
	};

	static Test test;

	void ComponentsBindings::Bind()
	{

		ScriptingEngine::BindInternalFunc("TerranScriptCore.Transform::GetTransformPosition_Internal", GetTransformPosition_Internal);

		ScriptingEngine::BindInternalFunc("TerranScriptCore.Transform::SetTransformPosition_Internal",
			SetTransformPosition_Internal);
	}

	void ComponentsBindings::SetTransformPosition_Internal(glm::vec3& position)
	{
		test.pos = position;
	}

	void ComponentsBindings::GetTransformPosition_Internal(glm::vec3& position)
	{
		position = test.pos;
	}
}

