#include "trpch.h"
#include "TransformSystem.h"

#include "Scene/Components.h"
#include "Scene/Entity.h"

#include "Scene/Scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace TerranEngine 
{

	TransformSystem::TransformSystem(Scene* scene)
		: m_Scene(scene) 
	{ }

	void TransformSystem::Update()
	{
		//auto transformView = m_Scene->GetEntitiesWith<TransformComponent>(entt::exclude<RelationshipComponent>);
		auto transformView = m_Scene->m_Registry.view<TransformComponent>(entt::exclude<RelationshipComponent>);

		for (auto e : transformView)
		{
			Entity entity(e, m_Scene);
			TransformComponent& transformComponent = entity.GetComponent<TransformComponent>();
			if (transformComponent.Dirty)
			{
				transformComponent.Position = transformComponent.LocalPosition;
				transformComponent.Scale = transformComponent.LocalScale;
				transformComponent.Rotation = transformComponent.LocalRotation;

				transformComponent.TransformMatrix = glm::translate(glm::mat4(1.0f), transformComponent.Position) *
										 glm::toMat4(glm::quat(transformComponent.Rotation)) *
										 glm::scale(glm::mat4(1.0f), transformComponent.Scale);

				transformComponent.Dirty = false;
			}
			else
				break;

		}

		//auto hierarchicalTransformView = m_Scene->GetEntitiesWith<TransformComponent, RelationshipComponent>();
		auto hierarchicalTransformView = m_Scene->m_Registry.view<TransformComponent, RelationshipComponent>();

		for (auto e : hierarchicalTransformView) 
		{
			Entity entity(e, m_Scene);
			if (entity.GetTransform().Dirty) 
				UpdateChild(entity);
		}
	}

	void TransformSystem::UpdateChild(Entity entity)
	{
		TransformComponent& transformComponent = entity.GetComponent<TransformComponent>();
		
		if (entity.HasParent())
		{
			TransformComponent& parentTransform = entity.GetParent().GetTransform();

			transformComponent.Position = transformComponent.LocalPosition + parentTransform.Position;
			transformComponent.Rotation = transformComponent.LocalRotation + parentTransform.Rotation;
			transformComponent.Scale = transformComponent.LocalScale * parentTransform.Scale;

			transformComponent.TransformMatrix = glm::translate(glm::mat4(1.0f), transformComponent.Position) *
				glm::toMat4(glm::quat(transformComponent.Rotation)) *
				glm::scale(glm::mat4(1.0f), transformComponent.Scale);
		}
		else 
		{
			transformComponent.Position = transformComponent.LocalPosition;
			transformComponent.Scale = transformComponent.LocalScale;
			transformComponent.Rotation = transformComponent.LocalRotation;

			transformComponent.TransformMatrix = glm::translate(glm::mat4(1.0f), transformComponent.Position) *
				glm::toMat4(glm::quat(transformComponent.Rotation)) *
				glm::scale(glm::mat4(1.0f), transformComponent.Scale);
		}
		transformComponent.Dirty = false;

		for (size_t i = 0; i < entity.GetChildCount(); i++)
		{
			Entity currEntity = entity.GetChild(i);

			currEntity.GetTransform().Dirty = true;
			UpdateChild(currEntity);
		}

	}
}
