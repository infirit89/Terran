#include "trpch.h"

#include "SceneSerializer.h"
#include "Entity.h"

#include "Scripting/ScriptField.h"
#include "Scripting/ScriptCache.h"
#include "Scripting/ScriptEngine.h"

#include "Utils/SerializerUtils.h"

#include <json.hpp>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <fstream>
#include <iomanip>

using json = nlohmann::ordered_json;

// NOTE: this is not the final version of the scene serializer, this is a poc
// NOTE: think about using yaml instead of json, because json has some limitation that i dont really like

namespace TerranEngine 
{
	SceneSerializer::SceneSerializer(const Shared<Scene>& scene)
		: m_Scene(scene)
	{
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
		GCHandle handle = ScriptEngine::GetScriptInstanceGCHandle(entity.GetSceneID(), entity.GetID());
		
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
				char value = (char)field->GetData<wchar_t>(handle);
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
				j[field->GetName()] = SerializerUtils::SerializeVec2(value);
				break;
			}
			case ScriptType::Vector3: 
			{
				glm::vec3 value = field->GetData<glm::vec3>(handle);
				j[field->GetName()] = SerializerUtils::SerializeVec3(value);
				break;
			}
			case ScriptType::Color:
			{
				glm::vec4 value = field->GetData<glm::vec4>(handle);
				j[field->GetName()] = SerializerUtils::SerializeVec4(value);
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
					{ "Position", SerializerUtils::SerializeVec3(entity.GetTransform().Position) },
					{ "Rotation", SerializerUtils::SerializeVec3(entity.GetTransform().Rotation) },
					{ "Scale",	SerializerUtils::SerializeVec3(entity.GetTransform().Scale) },
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
					{ "Color", SerializerUtils::SerializeVec4(sprComp.Color) },
					{ "TexturePath", sprComp.Texture ? sprComp.Texture->GetPath().string() : ""},
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
					{ "Color", SerializerUtils::SerializeVec4(crComp.Color) },
					{ "Thickness", crComp.Thickness }
				}}
			);
		}

		if (entity.HasComponent<TextRendererComponent>()) 
		{
			auto& textRendererComponent = entity.GetComponent<TextRendererComponent>();
			
			try 
			{
				
				jObject.push_back(
					{ "TextRendererComponent",
					{
						{ "Color", SerializerUtils::SerializeVec4(textRendererComponent.TextColor) },
						{ "Text", textRendererComponent.Text },
						{ "FontPath", textRendererComponent.FontAtlas ? textRendererComponent.FontAtlas->GetPath() : "" }
					} }
				);
			}
			catch (const std::exception& ex) 
			{
				TR_TRACE(ex.what());
			}
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
					{ "Offset", SerializerUtils::SerializeVec2(bcComponent.Offset) },
					{ "Size", SerializerUtils::SerializeVec2(bcComponent.Size) },
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
					{ "Offset", SerializerUtils::SerializeVec2(ccComponent.Offset) },
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

#define CATCH_JSON_EXCEPTION()\
catch(const std::exception& ex)\
{\
	TR_ERROR(ex.what());\
}

	static void DesirializeScriptable(Entity entity, json& jScriptComponent) 
	{
		auto& scriptComponent = entity.AddComponent<ScriptComponent>();
		scriptComponent.ModuleName = jScriptComponent["ModuleName"];

		if (ScriptEngine::ClassExists(scriptComponent.ModuleName)) 
		{
			ScriptEngine::InitializeScriptable(entity);
			
			if (jScriptComponent["Fields"] != "null")
			{
				for (const auto& fieldID : scriptComponent.PublicFieldIDs)
				{
					GCHandle handle = ScriptEngine::GetScriptInstanceGCHandle(entity.GetSceneID(), entity.GetID());
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
							const uint8_t value = jScriptFieldValue;
							field->SetData<char>((char)value, handle);
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
						case ScriptType::String:
						{
							std::string value = jScriptFieldValue;
							field->SetData(value, handle);
							break;
						}
						case ScriptType::Vector2: 
						{
							const glm::vec2 value = SerializerUtils::DeserializeVec2(jScriptComponent["Fields"], field->GetName());
							field->SetData(value, handle);
							break;
						}
						case ScriptType::Vector3: 
						{
							const glm::vec3 value = SerializerUtils::DeserializeVec3(jScriptComponent["Fields"], field->GetName());
							field->SetData(value, handle);
							break;
						}
						case ScriptType::Color: 
						{
							const glm::vec4 value = SerializerUtils::DeserializeVec4(jScriptComponent["Fields"], field->GetName());
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
					CATCH_JSON_EXCEPTION();
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
				glm::vec3 position = SerializerUtils::DeserializeVec3(jEntity["TransformComponent"], "Position");
				glm::vec3 scale = SerializerUtils::DeserializeVec3(jEntity["TransformComponent"], "Scale");
				glm::vec3 rotation = SerializerUtils::DeserializeVec3(jEntity["TransformComponent"], "Rotation");
					
				transform.Position = position;
				transform.Rotation = rotation;
				transform.Scale = scale;
			}
			CATCH_JSON_EXCEPTION();
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
			CATCH_JSON_EXCEPTION();
		}

		if (jEntity.contains("SpriteRendererComponent")) 
		{
			SpriteRendererComponent& spriteRenderer = entity.AddComponent<SpriteRendererComponent>();

			try 
			{
				glm::vec4 color = SerializerUtils::DeserializeVec4(jEntity["SpriteRendererComponent"], "Color");

				if (jEntity["SpriteRendererComponent"]["TexturePath"] != "") 
				{
					TextureParameters textureParameters;
					spriteRenderer.Texture = CreateShared<Texture>((std::string)jEntity["SpriteRendererComponent"]["TexturePath"], textureParameters);
				}

				spriteRenderer.Color = color;
			}
			CATCH_JSON_EXCEPTION();
		}

		if (jEntity.contains("CircleRendererComponent"))
		{
			CircleRendererComponent& circleRenderer = entity.AddComponent<CircleRendererComponent>();

			try
			{
				glm::vec4 color = SerializerUtils::DeserializeVec4(jEntity["CircleRendererComponent"], "Color");
				float thickness = jEntity["CircleRendererComponent"]["Thickness"];

				circleRenderer.Color = color;
				circleRenderer.Thickness = thickness;
			}
			CATCH_JSON_EXCEPTION();
		}

		if (jEntity.contains("TextRendererComponent")) 
		{
			TextRendererComponent& textRenderer = entity.AddComponent<TextRendererComponent>();

			try 
			{
				glm::vec4 color = SerializerUtils::DeserializeVec4(jEntity["TextRendererComponent"], "Color");
				textRenderer.TextColor = color;

				textRenderer.Text = jEntity["TextRendererComponent"]["Text"];

				if (jEntity["TextRendererComponent"]["FontPath"] != "")
					textRenderer.FontAtlas = CreateShared<FontAtlas>(jEntity["TextRendererComponent"]["FontPath"]);
			}
			CATCH_JSON_EXCEPTION();
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

						Entity child = scene->FindEntityWithUUID(UUID::FromString(id));
						child.SetParent(entity, true);
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
			CATCH_JSON_EXCEPTION();
		}

		if (jEntity.contains("BoxCollider2D")) 
		{
			json jBoxCollider2DComponent = jEntity["BoxCollider2D"];

			BoxCollider2DComponent& bcComponent = entity.AddComponent<BoxCollider2DComponent>();

			try 
			{
				glm::vec2 offset = SerializerUtils::DeserializeVec2(jBoxCollider2DComponent, "Offset");
				glm::vec2 size = SerializerUtils::DeserializeVec2(jBoxCollider2DComponent, "Size");
				bool isSensor = jBoxCollider2DComponent["IsSensor"];

				bcComponent.Offset = offset;
				bcComponent.Size = size;
				bcComponent.IsSensor = isSensor;
			}
			CATCH_JSON_EXCEPTION();
		}

		if (jEntity.contains("CircleCollider2D")) 
		{
			json jCircleCollider2DComponent = jEntity["CircleCollider2D"];

			CircleCollider2DComponent& ccComponent = entity.AddComponent<CircleCollider2DComponent>();

			try 
			{
				glm::vec2 offset = SerializerUtils::DeserializeVec2(jCircleCollider2DComponent, "Offset");
				float radius = jCircleCollider2DComponent["Radius"];
				bool isSensor = jCircleCollider2DComponent["IsSensor"];

				ccComponent.Offset = offset;
				ccComponent.Radius = radius;
				ccComponent.IsSensor = isSensor;
			}
			CATCH_JSON_EXCEPTION();
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
