#include "trpch.h"

#include "SceneSerializer.h"
#include "Entity.h"

#include <json.hpp>
#include <glm/glm.hpp>

#include <iomanip>
#include <fstream>

using json = nlohmann::ordered_json;

namespace TerranEngine 
{
	SceneSerializer::SceneSerializer(const Shared<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static json SerializeVec3(const std::string& name, const glm::vec3& value)
	{
		return
		{
			name, { 
				{"X", value.x},
				{"Y", value.y},
				{"Z", value.z}
			}
		};
	}

	static json SerializeVec4(const std::string& name, const glm::vec4& value)
	{
		return
		{
			name, {
				{"X", value.x},
				{"Y", value.y},
				{"Z", value.z},
				{"W", value.w}
			}
		};
	}

	static void SerializeEntity(json& j, Entity entity) 
	{
		TR_ASSERT(entity.HasComponent<TagComponent>(), "Can't serialize an entity that doesn't have a tag component");

		j["Entity " + std::to_string(uint32_t(entity))] =
		{
			{"TagComponent", 
			{
				{"Tag", entity.GetComponent<TagComponent>().Name},
				{"ID", std::to_string(entity.GetID())}
			}}
		};

		if (entity.HasComponent<TransformComponent>()) 
		{
			j["Entity " + std::to_string(uint32_t(entity))].push_back(
				{"TransformComponent", 
				{
					SerializeVec3("Position", entity.GetTransform().Position),
					SerializeVec3("Scale", entity.GetTransform().Scale),
					SerializeVec3("Rotation", entity.GetTransform().Rotation)
				}}
			);
		}

		if (entity.HasComponent<CameraComponent>()) 
		{
			auto& camComp = entity.GetComponent<CameraComponent>();

			j["Entity " + std::to_string(uint32_t(entity))].push_back(
				{ "CameraComponent",
				{
					{"Camera", 
					{	{"Width", camComp.Camera.GetWidth()},
						{"Height", camComp.Camera.GetHeight()},
						{"Depth", camComp.Camera.GetDepth()},
					}},
					{"Primary", camComp.Primary}
				} }
			);
		}

		if (entity.HasComponent<SpriteRendererComponent>()) 
		{
			auto& sprComp = entity.GetComponent<SpriteRendererComponent>();

			j["Entity " + std::to_string(uint32_t(entity))].push_back(
				{ "SpriteRendererComponent",
				{
					SerializeVec4("Color", sprComp.Color)
				} }
			);
		}
	}

	void SceneSerializer::SerializeJson(const std::string& filePath)
	{
		json j;

		j["Scene"] =  "Name";
		j["Entities"] = m_Scene->m_Registry.alive();

		m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity = { entityID, m_Scene.get() };
//			if (!entity)
//				return;

			SerializeEntity(j, entity);
		});
		//j["Entity 1"]["ID"] = { "Components", {"Transform"}};
		std::ofstream ofs("res/TestScene.json");

		ofs << std::setw(4) << j << std::endl;
	}

	void SceneSerializer::DesirializeJson(const std::string& filePath)
	{
	}
}
