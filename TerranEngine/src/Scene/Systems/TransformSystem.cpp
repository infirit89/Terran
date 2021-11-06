#include "trpch.h"
#include "TransformSystem.h"

#include "Scene/Components.h"
#include "Scene/RelationshipComponent.h"
#include "Scene/Entity.h"

#include "Scene/Scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Utils/Debug/DebugTimer.h"
namespace TerranEngine 
{
	static void UpdateChild(Entity& entity)
	{
		TransformComponent& transfromComponent = entity.GetComponent<TransformComponent>();
		RelationshipComponent& relationshipComponent = entity.GetComponent<RelationshipComponent>();

		if (relationshipComponent.Parent)
		{
			TransformComponent& parentTransform = relationshipComponent.Parent.GetTransform();

			transfromComponent.TransformMatrix = glm::translate(glm::mat4(1.0f), transfromComponent.Position) *
				glm::toMat4(glm::quat(transfromComponent.Rotation)) *
				glm::scale(glm::mat4(1.0f), transfromComponent.Scale);

			transfromComponent.TransformMatrix *= parentTransform.TransformMatrix;	


		}
		else 
		{
			transfromComponent.TransformMatrix = glm::translate(glm::mat4(1.0f), transfromComponent.Position) *
				glm::toMat4(glm::quat(transfromComponent.Rotation)) *
				glm::scale(glm::mat4(1.0f), transfromComponent.Scale);
		}
		transfromComponent.Dirty = false;

		Entity currChild = relationshipComponent.FirstChild;

		for (size_t i = 0; i < relationshipComponent.Children; i++)
		{
			currChild.GetTransform().Dirty = true;
			UpdateChild(currChild);
			currChild = currChild.GetComponent<RelationshipComponent>().Next;
		}

	}

	void TransformSystem::Update(Scene* scene)
	{
		//DebugTimer timer;

		auto transformView = scene->GetRegistry().view<TransformComponent>(entt::exclude<RelationshipComponent>);

		for (auto e : transformView)
		{
			Entity entity(e, scene);
			TransformComponent& transfromComponent = entity.GetComponent<TransformComponent>();
			if (transfromComponent.Dirty) 
			{
				transfromComponent.TransformMatrix = glm::translate(glm::mat4(1.0f), transfromComponent.Position) *
										 glm::toMat4(glm::quat(transfromComponent.Rotation)) *
										 glm::scale(glm::mat4(1.0f), transfromComponent.Scale);

				transfromComponent.Dirty = false;
			}
			else
				break;

		}

		auto hierarchicalTransformView = scene->GetRegistry().view<TransformComponent, RelationshipComponent>();
		for (auto e : hierarchicalTransformView) 
		{
			Entity entity(e, scene);
			if (entity.GetTransform().Dirty) 
				UpdateChild(entity);
		}
	}
}
