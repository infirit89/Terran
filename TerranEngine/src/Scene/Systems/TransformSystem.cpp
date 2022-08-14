#include "trpch.h"
#include "TransformSystem.h"

#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"

#include "Physics/Physics.h"
#include "Physics/PhysicsUtils.h"

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
		TransformComponent& tc = entity.GetComponent<TransformComponent>();
		
		if (s_Context->IsPlaying())
		{
			Shared<PhysicsBody2D>& physicsBody = Physics2D::GetPhysicsBody(entity);
			if (physicsBody) 
			{
				physicsBody->SetPosition({ tc.Position.x, tc.Position.y });
				physicsBody->SetRotation(tc.Rotation.z);
				physicsBody->SetSleepState(PhysicsBodySleepState::Awake);

				// set all the bodies this physics body is contacing with to be awake
				for (b2ContactEdge* contact = physicsBody->GetPhysicsBody()->GetContactList(); contact; contact = contact->next)
				{
					b2Body* body = contact->other;
                    Entity otherEntity = PhysicsUtils::GetEntityFromB2DUserData(body->GetUserData());
                    Shared<PhysicsBody2D>& otherPhysicsBody = Physics2D::GetPhysicsBody(entity);
					otherPhysicsBody->SetSleepState(PhysicsBodySleepState::Awake);
				}
			}
		}

		if (entity.HasParent()) 
		{
			tc.WorldTransformMatrix = CalculateTransformMatrix(tc) * 
									entity.GetParent().GetWorldMatrix();
			tc.LocalTransformMatrix = glm::inverse(entity.GetParent().GetWorldMatrix()) * 
									CalculateTransformMatrix(tc);
		}
		else 
		{
			tc.WorldTransformMatrix = CalculateTransformMatrix(tc);
			tc.LocalTransformMatrix = tc.WorldTransformMatrix;
		}

		glm::quat rotationQuat = tc.Rotation;

		tc.Forward = glm::normalize(glm::rotate(rotationQuat, glm::vec3(0.0f, 0.0f, 1.0f)));
		tc.Up = glm::normalize(glm::rotate(rotationQuat, glm::vec3(0.0f, 1.0f, 0.0f)));
		tc.Right = glm::normalize(glm::rotate(rotationQuat, glm::vec3(1.0f, 0.0f, 0.0f)));
		tc.IsDirty = false;

		for (size_t i = 0; i < entity.GetChildCount(); i++)
		{
			Entity currEntity = entity.GetChild(i);

			currEntity.GetTransform().IsDirty = true;
			UpdateEntityTransform(currEntity);
		}
	}
}
