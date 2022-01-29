#include "trpch.h"
#include "TransformSystem.h"

#include "Scene/Components.h"
#include "Scene/Entity.h"

#include "Scene/Scene.h"

#include "Utils/Debug/Profiler.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace TerranEngine 
{
	TransformSystem::TransformSystem(Scene* scene)
		: m_Scene(scene) 
	{ }

	static glm::mat4 GetTransfromMatrix(TransformComponent& transform) 
	{
		return glm::translate(glm::mat4(1.0f), transform.Position) *
			glm::toMat4(glm::quat(transform.Rotation)) *
			glm::scale(glm::mat4(1.0f), transform.Scale);
	}

	void TransformSystem::Update()
	{
		TR_PROFILE_FUNCN("TransformSystem::Update");

		auto transformView = m_Scene->GetEntitiesWith<TransformComponent>();

		for (auto e : transformView)
		{
			Entity entity(e, m_Scene);

			if (entity.GetTransform().Dirty || (entity.HasParent() && (entity.GetParent().GetTransform().Dirty))) 
				UpdateEntityTransform(entity);
			else
				break;
		}
	}

	void TransformSystem::UpdateEntityTransform(Entity entity)
	{
		TransformComponent& transformComponent = entity.GetTransform();
		
		if (entity.HasParent())
		{
			TransformComponent& parentTransform = entity.GetParent().GetTransform();

			transformComponent.Position = transformComponent.LocalPosition + parentTransform.Position;
			transformComponent.Rotation = transformComponent.LocalRotation + parentTransform.Rotation;
			transformComponent.Scale = transformComponent.LocalScale * parentTransform.Scale;
		}
		else 
		{
			transformComponent.Position = transformComponent.LocalPosition;
			transformComponent.Rotation = transformComponent.LocalRotation;
			transformComponent.Scale =	transformComponent.LocalScale;
		}

		transformComponent.WorldTransformMatrix = GetTransfromMatrix(transformComponent);
		transformComponent.LocalTransformMatrix = glm::inverse(transformComponent.WorldTransformMatrix);

		transformComponent.Dirty = false;

		for (size_t i = 0; i < entity.GetChildCount(); i++)
		{
			TR_TRACE(entity.GetChildCount());
			Entity currEntity = entity.GetChild(i);

			currEntity.GetTransform().Dirty = true;
			UpdateEntityTransform(currEntity);
		}

	}
}
