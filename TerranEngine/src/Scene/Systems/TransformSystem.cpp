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
	static glm::mat4 CalculateTransformMatrix(TransformComponent& transform) 
	{
		return glm::translate(glm::mat4(1.0f), transform.Position) *
			glm::toMat4(glm::quat(transform.Rotation)) *
			glm::scale(glm::mat4(1.0f), transform.Scale);
	}

	void TransformSystem::Update(Scene* scene)
	{
		TR_PROFILE_FUNCN("TransformSystem::Update");

		auto transformView = scene->GetEntitiesWith<TransformComponent>();

		for (auto e : transformView)
		{
			Entity entity(e, scene);

			Entity parent = entity.GetParent();

			if ((entity.GetTransform().IsDirty))
				UpdateEntityTransform(entity);
			else if (parent && parent.GetTransform().IsDirty)
				UpdateEntityTransform(parent);
			else
				break;
		}
	}

	void TransformSystem::UpdateEntityTransform(Entity entity)
	{
		TransformComponent& transformComponent = entity.GetTransform();
		
		if (entity.GetTransform().IsDirty) 
		{
			if (entity.HasParent()) 
			{
				transformComponent.WorldTransformMatrix = CalculateTransformMatrix(transformComponent) * entity.GetParent().GetWorldMatrix();

				transformComponent.LocalTransformMatrix = glm::inverse(entity.GetParent().GetWorldMatrix()) * CalculateTransformMatrix(transformComponent);
			}
			else 
			{
				transformComponent.WorldTransformMatrix = CalculateTransformMatrix(transformComponent);

				transformComponent.LocalTransformMatrix = transformComponent.WorldTransformMatrix;
			}
		}


		transformComponent.IsDirty = false;

		for (size_t i = 0; i < entity.GetChildCount(); i++)
		{
			Entity currEntity = entity.GetChild(i);

			currEntity.GetTransform().IsDirty = true;
			UpdateEntityTransform(currEntity);
		}
	}
}
