#include "trpch.h"
#include "TransformSystem.h"

#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"

#include "Physics/Physics.h"

#include "Utils/Debug/Profiler.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <box2d/box2d.h>

namespace TerranEngine 
{
	Scene* TransformSystem::s_Context;

	static glm::mat4 CalculateTransformMatrix(TransformComponent& transform) 
	{
		return glm::translate(glm::mat4(1.0f), transform.Position) *
			glm::toMat4(glm::quat(transform.Rotation)) *
			glm::scale(glm::mat4(1.0f), transform.Scale);
	}

	void TransformSystem::Update()
	{
		TR_PROFILE_FUNCN("TransformSystem::Update");

		auto transformView = s_Context->GetEntitiesWith<TransformComponent>();

		for (auto e : transformView)
		{
			Entity entity(e, s_Context);

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
		TransformComponent& transformComponent = entity.GetComponent<TransformComponent>();
		
		if (transformComponent.IsDirty) 
		{
			if (s_Context->HasRuntimeStarted() && entity.HasComponent<Rigidbody2DComponent>())
			{
				PhysicsBody2D& physicsBody = Physics2D::GetPhysicsBody(entity);
				physicsBody.SetPosition({ transformComponent.Position.x, transformComponent.Position.y });
				physicsBody.SetRotation(transformComponent.Rotation.z);
				physicsBody.SetSleepState(PhysicsBodySleepState::Awake);

				// set all the bodies this physics body is contacing with to be awake
				for (b2ContactEdge* contact = physicsBody.GetPhysicsBodyInternal()->GetContactList(); contact; contact = contact->next)
				{
					b2Body* body = contact->other;
					PhysicsBody2D otherPhysicsBody(body);
					otherPhysicsBody.SetSleepState(PhysicsBodySleepState::Awake);
				}
			}

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
