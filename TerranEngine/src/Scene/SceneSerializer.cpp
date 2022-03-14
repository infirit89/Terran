#include "trpch.h"

#include "SceneSerializer.h"
#include "Entity.h"

#include "Scripting/ScriptEngine.h"

#include <json.hpp>

#include <fstream>
#include <iomanip>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


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
				{ "X", value.x },
				{ "Y", value.y },
				{ "Z", value.z }
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
				{ "X", value.x },
				{ "Y", value.y },
				{ "Z", value.z },
				{ "W", value.w }
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
	

	static json SerializeUUIDVector(const std::vector<UUID>& vec) 
	{
		if (vec.size() <= 0)
			return NULL;

		json result;

		for (const auto& id : vec)
			result.push_back(std::to_string(id));

		return result;
	}

	static void SerializeField(json& j, ScriptComponent& scriptComponent) 
	{
		if (scriptComponent.PublicFields.empty())
			return;

		for (auto& hashedName : scriptComponent.FieldOrder)
		{
			ScriptField field = scriptComponent.PublicFields.at(hashedName);

			switch (field.GetType())
			{
			case ScriptFieldType::Bool: 
			{
				bool value = false;
				field.GetValue(&value);
				j[field.GetName()] = value;
				break;
			}
			case ScriptFieldType::Char: 
			{
				char value = ' ';
				field.GetValue(&value);
				j[field.GetName()] = value;
				break;
			}
			case ScriptFieldType::Int: 
			{
				int value = 0;
				field.GetValue(&value);
				j[field.GetName()] = value;
				break;
			}
			case ScriptFieldType::Float: 
			{
				float value = 0.0f;
				field.GetValue(&value);
				j[field.GetName()] = value;
				break;
			}
			case ScriptFieldType::Double: 
			{
				double value = 0.0;
				field.GetValue(&value);
				j[field.GetName()] = value;
				break;
			}
			
			default:	TR_ERROR("Unsupported field type"); break;
			}
		}
	}

	static void SerializeEntity(json& j, Entity entity) 
	{
		TR_ASSERT(entity.HasComponent<TagComponent>(), "Can't serialize an entity that doesn't have a tag component");

		json& jObject = j["Entity " + std::to_string(entity.GetID())] =
		{
			{"TagComponent", 
			{
				{ "Tag",	entity.GetComponent<TagComponent>().Name},
				{ "ID",		std::to_string(entity.GetID())}
			}}
		};

		if (entity.HasComponent<TransformComponent>()) 
		{
			jObject.push_back(
				{"TransformComponent", 
				{
					SerializeVec3("LocalPosition",	entity.GetTransform().Position),
					SerializeVec3("LocalScale",		entity.GetTransform().Scale),
					SerializeVec3("LocalRotation",	entity.GetTransform().Rotation),
				}}
			);
		}

		if (entity.HasComponent<CameraComponent>()) 
		{
			auto& camComp = entity.GetComponent<CameraComponent>();

			jObject.push_back(
				{ "CameraComponent",
				{
					{ "Camera", 
					{	{ "Size",	camComp.Camera.GetOrthographicSize()},
						{ "Near",	camComp.Camera.GetOrthographicNear()},
						{ "Far",	camComp.Camera.GetOrthographicFar()},
					}},
					{ "Primary", camComp.Primary}
				} }
			);
		}

		if (entity.HasComponent<SpriteRendererComponent>()) 
		{
			auto& sprComp = entity.GetComponent<SpriteRendererComponent>();

			jObject.push_back(
				{ "SpriteRendererComponent",
				{
					SerializeVec4("Color", sprComp.Color),
					{"ZIndex", sprComp.ZIndex}
				} }
			);
		}

		if (entity.HasComponent<CircleRendererComponent>()) 
		{
			auto& crComp = entity.GetComponent<CircleRendererComponent>();

			jObject.push_back(
				{ "CircleRendererComponent",
				{
					SerializeVec4("Color", crComp.Color),
					{ "Thickness", crComp.Thickness }
				}}
			);
		}

		if (entity.HasComponent<RelationshipComponent>()) 
		{
			auto& rlComp = entity.GetComponent<RelationshipComponent>();
			
			jObject.push_back(
				{ "RelationshipComponent",
				{
					{ "ChildrenCount",   entity.GetChildCount() },
					{ "Children",		 SerializeUUIDVector(rlComp.Children) },
					{ "Parent",			 (entity.HasParent() ? std::to_string(rlComp.ParentID) : "null") },
				} }
			);
		}

		if (entity.HasComponent<ScriptComponent>()) 
		{
			auto& scriptComponent = entity.GetComponent<ScriptComponent>();

			jObject.push_back(
				{ "ScriptComponent",
				{
					{ "ModuleName", scriptComponent.ModuleName },

					{ "Fields", {} }
				} }
			);

			SerializeField(jObject["ScriptComponent"]["Fields"], scriptComponent);
		}
	}

	// NOTE: temporary scene version should put it somewhere else, where it'd make more sense
	static int sceneVersion = 0;

	void SceneSerializer::SerializeJson(const std::filesystem::path& scenePath)
	{
		json j;

		j["Version"] = sceneVersion;

		j["Scene"] =  "Name";

		j["Entities"] = {};

		m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity = { entityID, m_Scene.get() };

			SerializeEntity(j["Entities"], entity);
		});

		std::ofstream ofs(scenePath);

		try
		{
			ofs << std::setw(4) << j << std::endl;
		}
		catch (const std::exception& e)
		{
			TR_ERROR(e.what());
		}
	}

	std::string SceneSerializer::ReadJson(const std::filesystem::path& scenePath)
	{
		json j;

		try
		{
			std::ifstream ifs(scenePath);

			ifs >> j;
		}
		catch (const std::exception& ex)
		{
			TR_ERROR(ex.what());
			return "";
		}

		return j.dump();
	}

	static void DesirializeScriptable(Entity entity, json& jScriptComponent) 
	{
		ScriptComponent& scriptComponent = entity.AddComponent<ScriptComponent>();

		scriptComponent.ModuleName = jScriptComponent["ModuleName"];

		ScriptEngine::InitializeScriptable(entity);

		if (jScriptComponent["Fields"] != "null")
		{
			for (auto& [hashedName, field] : scriptComponent.PublicFields)
			{
				if (jScriptComponent["Fields"].contains(field.GetName()))
				{
					json jScriptFieldValue = jScriptComponent["Fields"][field.GetName()];

					try 
					{
						switch (field.GetType())
						{
						case ScriptFieldType::Bool:
						{
							bool value = jScriptFieldValue;
							field.SetValue(&value);
							break;
						}
						case ScriptFieldType::Int64:
						{
							int64_t value = jScriptFieldValue;
							field.SetValue(&value);
							break;
						}
						case ScriptFieldType::Int:
						{
							int32_t value = jScriptFieldValue;
							field.SetValue(&value);
							break;
						}
						case ScriptFieldType::Int16:
						{
							int16_t value = jScriptFieldValue;
							field.SetValue(&value);
							break;
						}
						case ScriptFieldType::Int8:
						{
							int8_t value = jScriptFieldValue;
							field.SetValue(&value);
							break;
						}
						case ScriptFieldType::UInt64:
						{
							uint64_t value = jScriptFieldValue;
							field.SetValue(&value);
							break;
						}
						case ScriptFieldType::UInt:
						{
							uint32_t value = jScriptFieldValue;
							field.SetValue(&value);
							break;
						}
						case ScriptFieldType::UInt16:
						{
							uint16_t value = jScriptFieldValue;
							field.SetValue(&value);
							break;
						}
						case ScriptFieldType::UInt8:
						{
							uint8_t value = jScriptFieldValue;
							field.SetValue(&value);
							break;
						}
						case ScriptFieldType::Float:
						{
							float value = jScriptFieldValue;
							field.SetValue(&value);
							break;
						}
						case ScriptFieldType::Double:
						{
							double value = jScriptFieldValue;
							field.SetValue(&value);
							break;
						}
						}
					}
					catch (const std::exception& ex) 
					{
						TR_ERROR(ex.what());
					}
				}
			}
		}
	}

	static bool DesirializeEntity(json& jEntity, json& jScene, Shared<Scene> scene)
	{
		try
		{
			TR_ASSERT(jEntity.contains("TagComponent"), "Can't desirialize an entity that doesn't have a tag component");
			UUID uuid = UUID::FromString(jEntity["TagComponent"]["ID"]);
		
			if (scene->FindEntityWithUUID(uuid))
				return true;

			std::string tag = jEntity["TagComponent"]["Tag"];

			Entity entity = scene->CreateEntityWithUUID(tag, uuid);

			{
				auto& transform = entity.GetTransform();

				transform.Position = DeserializeVec3(jEntity["TransformComponent"], "LocalPosition");
				transform.Scale = DeserializeVec3(jEntity["TransformComponent"], "LocalScale");
				transform.Rotation = DeserializeVec3(jEntity["TransformComponent"], "LocalRotation");

			}

			if (jEntity.contains("CameraComponent"))
			{
				json jCamera = jEntity["CameraComponent"];

				OrthographicCamera cam;
				cam.SetOrthographicSize(jCamera["Camera"]["Size"]);
				cam.SetOrthographicNear(jCamera["Camera"]["Near"]);
				cam.SetOrthographicFar(jCamera["Camera"]["Far"]);

				entity.AddComponent<CameraComponent>().Camera = cam;
				entity.GetComponent<CameraComponent>().Primary = jCamera["Primary"];
			}

			if (jEntity.contains("SpriteRendererComponent"))
				entity.AddComponent<SpriteRendererComponent>().Color = DeserializeVec4(jEntity["SpriteRendererComponent"], "Color");

			if (jEntity.contains("CircleRendererComponent"))
			{
				entity.AddComponent<CircleRendererComponent>().Color = DeserializeVec4(jEntity["CircleRendererComponent"], "Color");
				entity.GetComponent<CircleRendererComponent>().Thickness = jEntity["CircleRendererComponent"]["Thickness"];
			}

			if (jEntity.contains("RelationshipComponent"))
			{
				json jRelation = jEntity["RelationshipComponent"];

				RelationshipComponent& rlComp = entity.AddComponent<RelationshipComponent>();

				if (jRelation["ChildrenCount"] > 0)
				{
					for (auto& id : jRelation["Children"])
					{
						if (!scene->FindEntityWithUUID(UUID::FromString(id)))
							DesirializeEntity(jScene["Entity " + std::string(id)], jScene, scene);

						Entity e = scene->FindEntityWithUUID(UUID::FromString(id));

						entity.AddChild(e);
					}
				}
				if (jRelation["Parent"] != "null")
				{
					if (!scene->FindEntityWithUUID(UUID::FromString(jRelation["Parent"])))
						DesirializeEntity(jScene["Entity " + std::string(jRelation["Parent"])], jScene, scene);

					entity.SetParentID(UUID::FromString(jRelation["Parent"]));
				}
			}

			if (jEntity.contains("ScriptComponent")) 
			{
				json jScriptComponent = jEntity["ScriptComponent"];

				DesirializeScriptable(entity, jScriptComponent);
			}
		}
		catch (const std::exception& ex)
		{
			TR_ERROR(ex.what());
			return false;
		}
		
		return true;
	}

	bool SceneSerializer::DesirializeJson(const std::string& data)
	{
		try 
		{
			json j = json::parse(data);
			
			// NOTE: bad version handling, this is a temporary fix, should think of a better one
			if (!j.contains("Version"))
			{
				// TODO: shit error message should think of a better one
				TR_ERROR("Couldn't find the version of the scene file");
				return false;
			}
			else 
			{
				if(j["Version"] != sceneVersion) 
				{
					// TODO: shit error message should think of a better one
					TR_ERROR("The version of the scene file doesn't match the current version");
					return false;
				}

				for (auto jEntity : j["Entities"]) 
				{
					if (!DesirializeEntity(jEntity, j["Entities"], m_Scene))
						return false;
				}
			}
		}
		catch (const std::exception& ex) 
		{
			TR_ERROR(ex.what());
			return false;
		}

		return true;
	}
}
