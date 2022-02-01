#pragma once

#include "Graphics/OrthographicCamera.h"
#include "Graphics/Texture.h"
#include "Core/Base.h"
#include "Core/UUID.h"

#include "Scripting/ScriptObject.h"
#include "Scripting/ScriptingEngine.h"

#include "Utils/Debug/Profiler.h"

#include <glm/glm.hpp>

#include <entt.hpp>

#include <vector>

namespace TerranEngine 
{
	struct TagComponent 
	{
		std::string Name;
		UUID ID;

		TagComponent() = default;

		TagComponent(const std::string& name, const UUID& id)
			: Name(name), ID(id) {}

		TagComponent(const std::string& name)
			: Name(name) {}
	};

	struct TransformComponent 
	{
		glm::vec3 LocalPosition =	{ 0.0f, 0.0f, 0.0f };
		glm::vec3 LocalRotation =	{ 0.0f, 0.0f, 0.0f };
		glm::vec3 LocalScale =		{ 1.0f, 1.0f, 1.0f };

		glm::vec3 Position =	LocalPosition;
		glm::vec3 Rotation =	LocalRotation;
		glm::vec3 Scale =		LocalScale;

		bool Dirty = true;

		glm::mat4 WorldTransformMatrix = glm::mat4(1.0f);
		glm::mat4 LocalTransformMatrix = glm::mat4(1.0f);

		TransformComponent() = default;
	};

	struct CameraComponent 
	{
		OrthographicCamera Camera;
		bool Primary = true;

		glm::vec4 BackgroundColor = { 0.1f, 0.1f, 0.1f, 1.0f };

		CameraComponent() = default;

	};

	struct SpriteRendererComponent 
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Shared<Texture> Texture = nullptr;

		int ZIndex = 0;

		SpriteRendererComponent() = default;
	};

	struct CircleRendererComponent 
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;

		CircleRendererComponent() = default;
	};

	struct RelationshipComponent 
	{
		UUID ParentID;
		
		// list containing all the child ids
		std::vector<UUID> Children;
	};

	struct ScriptableComponent 
	{
		ScriptableComponent() = default;

		ScriptableComponent(const std::string& moduleName) 
			: ModuleName(moduleName), RuntimeObject(ScriptingEngine::GetClass(moduleName)->CreateInstance()) { }

		void OnCreate(uint32_t entityID) 
		{
			if (!m_Created) 
			{
				Shared<UInt32> entityIDParam = CreateShared<UInt32>(entityID);
				RuntimeObject->Execute("SetEntityID", { entityIDParam });
				RuntimeObject->Execute("Init");
				m_Created = true;
			}
		}

		void OnUpdate() 
		{
			TR_PROFILE_FUNCN("ScriptableComponent::OnUpdate");
			RuntimeObject->Execute("Update");
		}

		std::string ModuleName;
		Shared<ScriptObject> RuntimeObject;

	private:
		bool m_Created = false;
	};
}