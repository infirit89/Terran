#pragma once

#include "Core/Base.h"
#include "Core/UUID.h"

#include "Graphics/Texture.h"
#include "Graphics/OrthographicCamera.h"
#include "Graphics/FontAtlas.h"

#include "Physics/PhysicsStates.h"

#include <glm/glm.hpp>

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

		glm::vec3 Forward =		{ 0.0f, 0.0f, 1.0f };
		glm::vec3 Up =			{ 0.0f, 1.0f, 0.0f };
		glm::vec3 Right =		{ 1.0f, 0.0f, 0.0f };

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

	struct LineRendererComponent 
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;

		int PointCount = 2;
		glm::vec3* Points;
		//glm::vec3 Point1 = { 0.0f, 0.0f, 0.0f };
		//glm::vec3 Point2 = { 0.0f, 0.0f, 1.0f };

		LineRendererComponent() 
		{
			Points = new glm::vec3[PointCount];

			Points[0] = { 0.0f, 0.0f, 0.0f };
			Points[1] = { 0.0f, 0.0f, 1.0f };
		}

		~LineRendererComponent() 
		{
			delete[] Points;
		}

	};

	struct TextRendererComponent 
	{
		Shared<FontAtlas> FontAtlas;
		glm::vec4 TextColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		std::string Text = "";
		float LineSpacing = 1.0f;
		float LineWidth = 10.0f;

		TextRendererComponent() = default;
		~TextRendererComponent() = default;
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
		
		std::vector<uint32_t> PublicFieldIDs;
		
		bool ClassExists = true;
		bool Started = false;
	};

	struct Rigidbody2DComponent 
	{
		Rigidbody2DComponent() = default;

		PhysicsBodyType BodyType = PhysicsBodyType::Dynamic;
		PhysicsBodySleepState SleepState = PhysicsBodySleepState::Awake;

		bool FixedRotation = false;
		float GravityScale = 1.0f;
		bool Enabled = true;
	};

	struct BoxCollider2DComponent 
	{
		BoxCollider2DComponent() = default;

		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 1.0f, 1.0f };

		bool IsSensor = false;

		uint32_t ColliderIndex = 0;
	};

	struct CircleCollider2DComponent 
	{
		CircleCollider2DComponent() = default;

		glm::vec2 Offset = { 0.0f, 0.0f };
		float Radius = 0.5f;
		bool IsSensor = false;

		uint32_t ColliderIndex = 0;
	};
}