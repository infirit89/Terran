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

	static glm::mat4 CalculateTransfromMatrix(TransformComponent& transform) 
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

			// TODO: if the parent's transform is dirty that pass the parent as an argument to
			// the UpdateEntityTransform function

			Entity parent = entity.GetParent();
			if ((entity.GetTransform().IsDirty))
				UpdateEntityTransform(entity);
			else if (parent)
				UpdateEntityTransform(parent);
			else
				break;
		}
	}

	void TransformSystem::UpdateEntityTransform(Entity entity)
	{
		TransformComponent& transformComponent = entity.GetTransform();
		
		if (entity.HasParent())
			transformComponent.WorldTransformMatrix = CalculateTransfromMatrix(transformComponent) * entity.GetParent().GetWorldMatrix();
		else 
			transformComponent.WorldTransformMatrix = CalculateTransfromMatrix(transformComponent);

		transformComponent.LocalTransformMatrix = glm::inverse(transformComponent.WorldTransformMatrix);

		transformComponent.IsDirty = false;

		for (size_t i = 0; i < entity.GetChildCount(); i++)
		{
			Entity currEntity = entity.GetChild(i);

			currEntity.GetTransform().IsDirty = true;
			UpdateEntityTransform(currEntity);
		}

	}
}
