#pragma once

#include "Core/Base.h"
#include "Core/UUID.h"

#include "Graphics/Texture.h"
#include "Graphics/OrthographicCamera.h"

#include "Scripting/ScriptField.h"

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
		glm::vec3 Position =	{ 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation =	{ 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale =		{ 1.0f, 1.0f, 1.0f };

		bool IsDirty = true;

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

	struct ScriptComponent 
	{
		ScriptComponent() = default;

		ScriptComponent(const std::string& moduleName) 
			: ModuleName(moduleName)
		{
			
		}
		
		// NOTE: think about having an array of scripts so that one entity
		// "can" have more than one script (because of the 1 component of a type per entity)

		std::string ModuleName;
		
		std::vector<uint32_t> FieldOrder;
		std::unordered_map<uint32_t, ScriptField> PublicFields;
		
		bool ClassExists = true;
		bool Started = false;
	};

	enum class RigidbodyBodyType 
	{
		Static = 0,
		Dynamic,
		Kinematic
	};

	struct Rigidbody2DComponent 
	{
		Rigidbody2DComponent() = default;

		RigidbodyBodyType BodyType = RigidbodyBodyType::Dynamic;
		bool FixedRotation = false;

		void* RuntimeBody;
	};

	struct BoxCollider2DComponent 
	{
		glm::vec2 Size = { 0.5f, 0.5f };
	};
}