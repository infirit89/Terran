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

	static glm::vec3 DeserializeVec3(json& j, const std::string& name) 
	{
		glm::vec3 vec = glm::vec3(0.0f);

		if (j.contains(name))
		{
			vec.x = j[name]["X"];
			vec.y = j[name]["Y"];
			vec.z = j[name]["Z"];
		}

		return vec;
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

	static glm::vec4 DeserializeVec4(json& j, const std::string& name)
	{
		glm::vec4 vec = glm::vec4(0.0f);

		if (j.contains(name))
		{
			vec.x = j[name]["X"];
			vec.y = j[name]["Y"];
			vec.z = j[name]["Z"];
			vec.w = j[name]["W"];
		}

		return vec;
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

		std::ofstream ofs("res/TestScene.json");

		ofs << std::setw(4) << j << std::endl;
	}

	void SceneSerializer::DesirializeJson(const std::string& filePath)
	{
		std::ifstream ifs("res/TestScene.json");

		json j;
		ifs >> j;

		for (size_t i = 0; i < j["Entities"]; i++)
		{
			json jEntity = j["Entity " + std::to_string(i)];
			TR_ASSERT(jEntity.contains("TagComponent"), "Can't desirialize an entity that doesn't have a tag component");
			std::string tag = jEntity["TagComponent"]["Tag"];
			UUID uuid = UUID::FromString(jEntity["TagComponent"]["ID"]);

			Entity entity = m_Scene->CreateEntityWithUUID(tag, uuid);
			auto& transform = entity.GetTransform();
			transform.Position = DeserializeVec3(jEntity["TransformComponent"], "Position");
			transform.Scale = DeserializeVec3(jEntity["TransformComponent"], "Scale");
			transform.Rotation = DeserializeVec3(jEntity["TransformComponent"], "Rotation");

			if (jEntity.contains("CameraComponent"))
			{
				json jCamera = jEntity["CameraComponent"];

				OrthographicCamera cam(jCamera["Camera"]["Width"], jCamera["Camera"]["Height"], jCamera["Camera"]["Depth"]);

				entity.AddComponent<CameraComponent>().Camera = cam;
				entity.GetComponent<CameraComponent>().Primary = jCamera["Primary"];
			}

			if (jEntity.contains("SpriteRendererComponent"))
			{
				entity.AddComponent<SpriteRendererComponent>().Color = DeserializeVec4(jEntity["SpriteRendererComponent"], "Color");
			}
		}
	}
}
