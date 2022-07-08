#include "trpch.h"

#include "SceneSerializer.h"
#include "Entity.h"

#include "Scripting/ScriptField.h"
#include "Scripting/ScriptCache.h"
#include "Scripting/ScriptEngine.h"

#include <json.hpp>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <fstream>
#include <iomanip>

using json = nlohmann::ordered_json;

namespace TerranEngine 
{
	SceneSerializer::SceneSerializer(const Shared<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static json SerializeVec2(const glm::vec2& value) 
	{
		return
		{
			{ "X", value.x },
			{ "Y", value.y },
		};
	}

	static glm::vec2 DeserializeVec2(json& j, const std::string& name)
	{
		glm::vec2 vec = glm::vec2(0.0f);

		vec.x = j[name]["X"];
		vec.y = j[name]["Y"];

		return vec;
	}

	static json SerializeVec3(const glm::vec3& value)
	{
		return
		{
			{ "X", value.x },
			{ "Y", value.y },
			{ "Z", value.z }
		};
	}

	static glm::vec3 DeserializeVec3(json& j, const std::string& name) 
	{
		glm::vec3 vec = glm::vec3(0.0f);

		vec.x = j[name]["X"];
		vec.y = j[name]["Y"];
		vec.z = j[name]["Z"];

		return vec;
	}

	static json SerializeVec4(const glm::vec4& value)
	{
		return
		{
			{ "X", value.x },
			{ "Y", value.y },
			{ "Z", value.z },
			{ "W", value.w }
		};
	}


	static glm::vec4 DeserializeVec4(json& j, const std::string& name)
	{
		glm::vec4 vec = glm::vec4(0.0f);

		vec.x = j[name]["X"];
		vec.y = j[name]["Y"];
		vec.z = j[name]["Z"];
		vec.w = j[name]["W"];

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

	static void SerializeField(json& j, Entity entity) 
	{
		ScriptComponent& sc = entity.GetComponent<ScriptComponent>();
		GCHandle& handle = ScriptEngine::GetScriptInstanceGCHandle(entity.GetSceneID(), entity.GetID());
		
		for (auto& fieldID : sc.PublicFieldIDs)
		{
			ScriptField* field = ScriptCache::GetCachedFieldFromID(fieldID);

			switch (field->GetType().TypeEnum)
			{
			case ScriptType::Bool: 
			{
				bool value = field->GetData<bool>(handle);
				j[field->GetName()] = value;
				break;
			}
			case ScriptType::Char: 
			{
				char value = field->GetData<wchar_t>(handle);
				j[field->GetName()] = value;
				break;
			}
			case ScriptType::Int8:
			{
				int8_t value = field->GetData<int8_t>(handle);
				j[field->GetName()] = value;
				break;
			}
			case ScriptType::Int16:
			{
				int16_t value = field->GetData<int16_t>(handle);
				j[field->GetName()] = value;
				break;
			}
			case ScriptType::Int32: 
			{
				int32_t value = field->GetData<int32_t>(handle);
				j[field->GetName()] = value;
				break;
			}
			case ScriptType::Int64:
			{
				int64_t value = field->GetData<int64_t>(handle);
				j[field->GetName()] = value;
				break;
			}
			case ScriptType::UInt8:
			{
				uint8_t value = field->GetData<uint8_t>(handle);
				j[field->GetName()] = value;
				break;
			}
			case ScriptType::UInt16:
			{
				uint16_t value = field->GetData<uint16_t>(handle);
				j[field->GetName()] = value;
				break;
			}
			case ScriptType::UInt32:
			{
				uint32_t value = field->GetData<uint32_t>(handle);
				j[field->GetName()] = value;
				break;
			}
			case ScriptType::UInt64:
			{
				uint64_t value = field->GetData<uint64_t>(handle);
				j[field->GetName()] = value;
				break;
			}
			case ScriptType::Float: 
			{
				float value = field->GetData<float>(handle);
				j[field->GetName()] = value;
				break;
			}
			case ScriptType::Double: 
			{
				double value = field->GetData<double>(handle);
				j[field->GetName()] = value;
				break;
			}
			case ScriptType::String:
			{
				std::string value = field->GetData<std::string>(handle);
				j[field->GetName()] = value;
				break;
			}
			case ScriptType::Vector2: 
			{
				// NOTE: might need to box
				glm::vec2 value = field->GetData<glm::vec2>(handle);
				j[field->GetName()] = SerializeVec2(value);
				break;
			}
			case ScriptType::Vector3: 
			{
				glm::vec3 value = field->GetData<glm::vec3>(handle);
				j[field->GetName()] = SerializeVec3(value);
				break;
			}
			case ScriptType::Color:
			{
				glm::vec4 value = field->GetData<glm::vec4>(handle);
				j[field->GetName()] = SerializeVec4(value);
				break;
			}
			case ScriptType::Entity:
			{
				UUID value = field->GetData<UUID>(handle);
				j[field->GetName()] = std::to_string(value);
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
				{ "Tag",	entity.GetComponent<TagComponent>().Name },
				{ "ID",		std::to_string(entity.GetID()) }
			}}
		};

		if (entity.HasComponent<TransformComponent>()) 
		{
			jObject.push_back(
				{"TransformComponent", 
				{
					{ "Position", SerializeVec3(entity.GetTransform().Position) },
					{ "Rotation", SerializeVec3(entity.GetTransform().Rotation) },
					{ "Scale",	SerializeVec3(entity.GetTransform().Scale) },
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
					{	{ "Size",	camComp.Camera.GetOrthographicSize() },
						{ "Near",	camComp.Camera.GetOrthographicNear() },
						{ "Far",	camComp.Camera.GetOrthographicFar() },
					}},
					{ "Primary", camComp.Primary }
				} }
			);
		}

		if (entity.HasComponent<SpriteRendererComponent>()) 
		{
			auto& sprComp = entity.GetComponent<SpriteRendererComponent>();

			jObject.push_back(
				{ "SpriteRendererComponent",
				{
					{ "Color", SerializeVec4(sprComp.Color) },
					{ "ZIndex", sprComp.ZIndex }
				} }
			);
		}

		if (entity.HasComponent<CircleRendererComponent>()) 
		{
			auto& crComp = entity.GetComponent<CircleRendererComponent>();

			jObject.push_back(
				{ "CircleRendererComponent",
				{
					{ "Color", SerializeVec4(crComp.Color) },
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

			SerializeField(jObject["ScriptComponent"]["Fields"], entity);
		}

		if (entity.HasComponent<Rigidbody2DComponent>()) 
		{
			auto& rbComponent = entity.GetComponent<Rigidbody2DComponent>();

			jObject.push_back(
				{ "Rigibody2D",
				{
					{ "BodyType", (int)rbComponent.BodyType },
					{ "FixedRotation", rbComponent.FixedRotation },
					{ "SleepState", (int)rbComponent.SleepState },
					{ "GravityScale", rbComponent.GravityScale}
				} }
			);
		}

		if (entity.HasComponent<BoxCollider2DComponent>()) 
		{
			auto& bcComponent = entity.GetComponent<BoxCollider2DComponent>();

			jObject.push_back(
				{ "BoxCollider2D",
				{
					{ "Offset", SerializeVec2(bcComponent.Offset) },
					{ "Size", SerializeVec2(bcComponent.Size) },
					{ "IsSensor", bcComponent.IsSensor }
				} }
			);
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			auto& ccComponent = entity.GetComponent<CircleCollider2DComponent>();

			jObject.push_back(
				{ "CircleCollider2D",
				{
					{ "Offset", SerializeVec2(ccComponent.Offset) },
					{ "Radius", ccComponent.Radius },
					{ "IsSensor", ccComponent.IsSensor }
				} }
			);
		}
	}

	void SceneSerializer::SerializeJson(const std::filesystem::path& scenePath)
	{
		json j;

		j["Scene"] =  "Name";

		j["Entities"] = {};

		const auto tcView = m_Scene->GetEntitiesWith<TagComponent>();
		// m_Scene->m_Registry.each([&](auto entityID)
		// {
		// 	Entity entity = { entityID, m_Scene.get() };
		//
		// 	SerializeEntity(j["Entities"], entity);
		// });

		for (auto e : tcView)
		{
			Entity entity(e, m_Scene->GetRaw());
			SerializeEntity(j["Entities"], entity);
		}

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

#define PRINT_JSON_ERROR()\
catch(const std::exception& ex)\
{\
	TR_ERROR(ex.what());\
}

	static void DesirializeScriptable(Entity entity, json& jScriptComponent) 
	{
		ScriptComponent& scriptComponent = entity.AddComponent<ScriptComponent>();

		scriptComponent.ModuleName = jScriptComponent["ModuleName"];

		if (ScriptEngine::ClassExists(scriptComponent.ModuleName)) 
		{
			ScriptEngine::InitializeScriptable(entity);
			
			if (jScriptComponent["Fields"] != "null")
			{
				for (const auto& fieldID : scriptComponent.PublicFieldIDs)
				{
					GCHandle& handle = ScriptEngine::GetScriptInstanceGCHandle(entity.GetSceneID(), entity.GetID());
					ScriptField* field = ScriptCache::GetCachedFieldFromID(fieldID);

					if (!jScriptComponent["Fields"].contains(field->GetName()))
						return;
					
					json jScriptFieldValue = jScriptComponent["Fields"][field->GetName()];

					try 
					{
						switch (field->GetType().TypeEnum)
						{
						case ScriptType::Bool:
						{
							const bool value = jScriptFieldValue;
							field->SetData(value, handle);
							break;
						}
						case ScriptType::Char:
						{
							std::string strValue = jScriptFieldValue;
							char value = strValue.size() > 0 ? strValue.at(0) : 0;
							field->SetData(value, handle);
							break;
						}
						case ScriptType::Int64:
						{
							const int64_t value = jScriptFieldValue;
							field->SetData(value, handle);
							break;
						}
						case ScriptType::Int32:
						{
							const int32_t value = jScriptFieldValue;
							field->SetData(value, handle);
							break;
						}
						case ScriptType::Int16:
						{
							const int16_t value = jScriptFieldValue;
							field->SetData(value, handle);
							break;
						}
						case ScriptType::Int8:
						{
							const int8_t value = jScriptFieldValue;
							field->SetData(value, handle);
							break;
						}
						case ScriptType::UInt64:
						{
							const uint64_t value = jScriptFieldValue;
							field->SetData(value, handle);
							break;
						}
						case ScriptType::UInt32:
						{
							const uint32_t value = jScriptFieldValue;
							field->SetData(value, handle);
							break;
						}
						case ScriptType::UInt16:
						{
							const uint16_t value = jScriptFieldValue;
							field->SetData(value, handle);
							break;
						}
						case ScriptType::UInt8:
						{
							const uint8_t value = jScriptFieldValue;
							field->SetData(value, handle);
							break;
						}
						case ScriptType::Float:
						{
							const float value = jScriptFieldValue;
							field->SetData(value, handle);
							break;
						}
						case ScriptType::Double:
						{
							const double value = jScriptFieldValue;
							field->SetData(value, handle);
							break;
						}
						case ScriptType::Vector2: 
						{
							const glm::vec2 value = DeserializeVec2(jScriptComponent["Fields"], field->GetName());
							field->SetData(value, handle);
							break;
						}
						case ScriptType::Vector3: 
						{
							const glm::vec3 value = DeserializeVec3(jScriptComponent["Fields"], field->GetName());
							field->SetData(value, handle);
							break;
						}
						case ScriptType::Color: 
						{
							const glm::vec4 value = DeserializeVec4(jScriptComponent["Fields"], field->GetName());
							field->SetData(value, handle);
							break;
						}
						case ScriptType::Entity:
						{
							UUID value = UUID::FromString(jScriptFieldValue);
							field->SetData(value, handle);
							break;
						}
						}
					}
					PRINT_JSON_ERROR();
				}
			}
		}
	}

	static bool DesirializeEntity(json& jEntity, json& jScene, Shared<Scene> scene)
	{
		Entity entity;

		try
		{
			if (!jEntity.contains("TagComponent"))
				TR_ERROR("Can't desirialize an entity that doesn't have a tag component");

			UUID uuid = UUID::FromString(jEntity["TagComponent"]["ID"]);
		
			if (scene->FindEntityWithUUID(uuid))
				return true;

			std::string tag = jEntity["TagComponent"]["Tag"];

			entity = scene->CreateEntityWithUUID(tag, uuid);

		}
		catch (const std::exception& ex)
		{
			TR_ERROR(ex.what());
			return false;
		}

		{
			auto& transform = entity.GetTransform();

			try
			{
				glm::vec3 position = DeserializeVec3(jEntity["TransformComponent"], "Position");
				glm::vec3 scale = DeserializeVec3(jEntity["TransformComponent"], "Scale");
				glm::vec3 rotation = DeserializeVec3(jEntity["TransformComponent"], "Rotation");
					
				transform.Position = position;
				transform.Rotation = rotation;
				transform.Scale = scale;
			}
			PRINT_JSON_ERROR();
		}

		if (jEntity.contains("CameraComponent"))
		{
			json jCamera = jEntity["CameraComponent"];

			CameraComponent& cameraComponent = entity.AddComponent<CameraComponent>();
				
			try
			{
				CameraComponent tempCamComponent;

				OrthographicCamera cam;
				cam.SetOrthographicSize(jCamera["Camera"]["Size"]);
				cam.SetOrthographicNear(jCamera["Camera"]["Near"]);
				cam.SetOrthographicFar(jCamera["Camera"]["Far"]);

				tempCamComponent.Primary = jCamera["Primary"];

				cameraComponent = tempCamComponent;
			}
			PRINT_JSON_ERROR();
		}

		if (jEntity.contains("SpriteRendererComponent")) 
		{
			SpriteRendererComponent& spriteRenderer = entity.AddComponent<SpriteRendererComponent>();

			try 
			{
				glm::vec4 color = DeserializeVec4(jEntity["SpriteRendererComponent"], "Color");

				spriteRenderer.Color = color;
			}
			PRINT_JSON_ERROR();
		}
		if (jEntity.contains("CircleRendererComponent"))
		{
			CircleRendererComponent& circleRenderer = entity.AddComponent<CircleRendererComponent>();

			try
			{
				glm::vec4 color = DeserializeVec4(jEntity["CircleRendererComponent"], "Color");
				float thickness = jEntity["CircleRendererComponent"]["Thickness"];

				circleRenderer.Color = color;
				circleRenderer.Thickness = thickness;
			}
			PRINT_JSON_ERROR();
		}

		if (jEntity.contains("RelationshipComponent"))
		{
			json jRelation = jEntity["RelationshipComponent"];

			RelationshipComponent& relationshipComponent = entity.AddComponent<RelationshipComponent>();

			try 
			{
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
			catch (const std::exception& ex) 
			{
				TR_ERROR(ex.what());
				relationshipComponent = RelationshipComponent();
			}
		}

		if (jEntity.contains("ScriptComponent")) 
		{
			json jScriptComponent = jEntity["ScriptComponent"];

			DesirializeScriptable(entity, jScriptComponent);
		}

		if (jEntity.contains("Rigibody2D")) 
		{
			json jRigidbody2DComponent = jEntity["Rigibody2D"];

			Rigidbody2DComponent& rbComponent = entity.AddComponent<Rigidbody2DComponent>();

			try 
			{
				PhysicsBodyType bodyType = (PhysicsBodyType)jRigidbody2DComponent["BodyType"];
				bool fixedRotation = jRigidbody2DComponent["FixedRotation"];
				PhysicsBodySleepState sleepState = (PhysicsBodySleepState)jRigidbody2DComponent["SleepState"];
				float gravityScale = jRigidbody2DComponent["GravityScale"];

				rbComponent.BodyType = bodyType;
				rbComponent.FixedRotation = fixedRotation;
				rbComponent.SleepState = sleepState;
				rbComponent.GravityScale = gravityScale;
			}
			PRINT_JSON_ERROR();
		}

		if (jEntity.contains("BoxCollider2D")) 
		{
			json jBoxCollider2DComponent = jEntity["BoxCollider2D"];

			BoxCollider2DComponent& bcComponent = entity.AddComponent<BoxCollider2DComponent>();

			try 
			{
				glm::vec2 offset = DeserializeVec2(jBoxCollider2DComponent, "Offset");
				glm::vec2 size = DeserializeVec2(jBoxCollider2DComponent, "Size");
				bool isSensor = jBoxCollider2DComponent["IsSensor"];

				bcComponent.Offset = offset;
				bcComponent.Size = size;
				bcComponent.IsSensor = isSensor;
			}
			PRINT_JSON_ERROR();
		}

		if (jEntity.contains("CircleCollider2D")) 
		{
			json jCircleCollider2DComponent = jEntity["CircleCollider2D"];

			CircleCollider2DComponent& ccComponent = entity.AddComponent<CircleCollider2DComponent>();

			try 
			{
				glm::vec2 offset = DeserializeVec2(jCircleCollider2DComponent, "Offset");
				float radius = jCircleCollider2DComponent["Radius"];
				bool isSensor = jCircleCollider2DComponent["IsSensor"];

				ccComponent.Offset = offset;
				ccComponent.Radius = radius;
				ccComponent.IsSensor = isSensor;
			}
			PRINT_JSON_ERROR();
		}
		
		return true;
	}

	bool SceneSerializer::DesirializeJson(const std::string& data)
	{
		try 
		{
			json j = json::parse(data);


			for (auto jEntity : j["Entities"]) 
			{
				if (!DesirializeEntity(jEntity, j["Entities"], m_Scene))
					return false;
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
