#include "trpch.h"
#include "SceneSerializer.h"

#include <json.hpp>

#include <iomanip>
#include <fstream>
using json = nlohmann::ordered_json;

#include "Entity.h"

namespace TerranEngine 
{
	SceneSerializer::SceneSerializer(const Shared<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static void SerializeEntity(json& j, Entity entity) 
	{
		TR_ASSERT(entity.HasComponent<TagComponent>(), "Can't serialize an entity that doesn't have a tag component");

		j["Entity : " + std::to_string(entity.GetID())] = "Test";
	}

	void SceneSerializer::SerializeJson(const std::string& filePath)
	{
		json j;

		j["Scene"] =  "Name";

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
