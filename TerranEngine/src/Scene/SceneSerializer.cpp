#include "trpch.h"

#include "SceneSerializer.h"
#include "Entity.h"

#include "Scripting/ScriptField.h"
#include "Scripting/ScriptCache.h"
#include "Scripting/ScriptEngine.h"

#include "Utils/SerializerUtils.h"

#include <yaml-cpp/yaml.h>

#include <json.hpp>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

using json = nlohmann::ordered_json;

// NOTE: this is not the final version of the scene serializer, this is a poc
// NOTE: think about using yaml instead of json, because json has some limitation that i dont really like

namespace TerranEngine 
{
	const char* SceneSerializer::SceneFilter = "Terran Scene\0*.terran\0";
	static const char* SerializerVersion = "yml1.0";

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

#define WRITE_SCRIPT_FIELD(FieldType, Type)\
	case ScriptType::FieldType:\
	out << YAML::Key << field->GetName() << YAML::Value << field->GetData<Type>(handle);\
	break

	static void SerializeScriptFields(YAML::Emitter& out, Entity entity) 
	{
		ScriptComponent& sc = entity.GetComponent<ScriptComponent>();
		GCHandle handle = ScriptEngine::GetScriptInstanceGCHandle(entity.GetSceneID(), entity.GetID());
		
		for (auto& fieldID : sc.PublicFieldIDs)
		{
			ScriptField* field = ScriptCache::GetCachedFieldFromID(fieldID);


			switch (field->GetType().TypeEnum)
			{
				WRITE_SCRIPT_FIELD(Bool, bool);
				WRITE_SCRIPT_FIELD(Char, wchar_t);
				WRITE_SCRIPT_FIELD(Int8, int8_t);
				WRITE_SCRIPT_FIELD(Int16, int16_t);
				WRITE_SCRIPT_FIELD(Int32, int32_t);
				WRITE_SCRIPT_FIELD(Int64, int64_t);
				WRITE_SCRIPT_FIELD(UInt8, uint8_t);
				WRITE_SCRIPT_FIELD(UInt16, uint16_t);
				WRITE_SCRIPT_FIELD(UInt32, uint32_t);
				WRITE_SCRIPT_FIELD(UInt64, uint64_t);
				WRITE_SCRIPT_FIELD(Float, float);
				WRITE_SCRIPT_FIELD(Double, double);
				WRITE_SCRIPT_FIELD(String, std::string);
				WRITE_SCRIPT_FIELD(Vector2, glm::vec2);
				WRITE_SCRIPT_FIELD(Vector3, glm::vec3);
				WRITE_SCRIPT_FIELD(Color, glm::vec4);
				WRITE_SCRIPT_FIELD(Entity, UUID);
				default:	TR_ERROR("Unsupported field type"); break;
			}
		}
	}

#define BEGIN_COMPONENT_MAP(componentKey)\
	out << YAML::Key << componentKey;\
	out << YAML::BeginMap

#define END_COMPONENT_MAP()\
	out << YAML::EndMap

#define WRITE_COMPONENT_PROPERY(propertyName, property)\
	out << YAML::Key << propertyName << YAML::Value << property

	static void SerializeEntity(YAML::Emitter& out, Entity entity) 
	{
		// TODO: convert to a verify assert
		TR_ASSERT(entity.HasComponent<TagComponent>(), "Can't serialize an entity that doesn't have a tag component");
		out << YAML::BeginMap;
		WRITE_COMPONENT_PROPERY("Entity", entity.GetID());

		BEGIN_COMPONENT_MAP("TagComponent");
		auto& tagComponent = entity.GetComponent<TagComponent>();
		WRITE_COMPONENT_PROPERY("Tag", tagComponent.Name);
		END_COMPONENT_MAP();

		if (entity.HasComponent<TransformComponent>()) 
		{
			auto& transformComponent = entity.GetTransform();

			BEGIN_COMPONENT_MAP("TransformComponent");
			WRITE_COMPONENT_PROPERY("Position", transformComponent.Position);
			WRITE_COMPONENT_PROPERY("Scale", transformComponent.Scale);	
			WRITE_COMPONENT_PROPERY("Rotation", transformComponent.Rotation);
			END_COMPONENT_MAP();
		}

		if (entity.HasComponent<CameraComponent>())
		{
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			BEGIN_COMPONENT_MAP("CameraComponent");

			WRITE_COMPONENT_PROPERY("Camera", YAML::BeginMap);
			WRITE_COMPONENT_PROPERY("Size", cameraComponent.Camera.GetOrthographicSize());
			WRITE_COMPONENT_PROPERY("Near", cameraComponent.Camera.GetOrthographicNear());
			WRITE_COMPONENT_PROPERY("Far", cameraComponent.Camera.GetOrthographicFar());
			out << YAML::EndMap;

			WRITE_COMPONENT_PROPERY("Primary", cameraComponent.Primary);
			WRITE_COMPONENT_PROPERY("ClearColor", cameraComponent.BackgroundColor);

			END_COMPONENT_MAP();
		}

		if (entity.HasComponent<SpriteRendererComponent>()) 
		{
			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
			BEGIN_COMPONENT_MAP("SpriteRendererComponent");

			WRITE_COMPONENT_PROPERY("Color", spriteRendererComponent.Color);
			WRITE_COMPONENT_PROPERY("Texture", spriteRendererComponent.TextureHandle);

			END_COMPONENT_MAP();
		}

		if (entity.HasComponent<CircleRendererComponent>()) 
		{
			auto& circleRendererComponent = entity.GetComponent<CircleRendererComponent>();
			BEGIN_COMPONENT_MAP("CircleRendererComponent");

			WRITE_COMPONENT_PROPERY("Color", circleRendererComponent.Color);
			WRITE_COMPONENT_PROPERY("Thickness", circleRendererComponent.Thickness);

			END_COMPONENT_MAP();
		}

		if (entity.HasComponent<TextRendererComponent>()) 
		{
			auto& textRendererComponent = entity.GetComponent<TextRendererComponent>();
			BEGIN_COMPONENT_MAP("TextRendererCompoent");

			WRITE_COMPONENT_PROPERY("Color", textRendererComponent.TextColor);
			WRITE_COMPONENT_PROPERY("Text", textRendererComponent.Text);
			// TODO: save font handle

			END_COMPONENT_MAP();
		}

		if (entity.HasComponent<RelationshipComponent>()) 
		{
			auto& relationshipComponent = entity.GetComponent<RelationshipComponent>();
			BEGIN_COMPONENT_MAP("RelationshipComponent");

			WRITE_COMPONENT_PROPERY("Children", YAML::BeginSeq);
			for (auto child : relationshipComponent.Children)
				out << child;
			out << YAML::EndSeq;

			WRITE_COMPONENT_PROPERY("Parent", relationshipComponent.Parent);

			END_COMPONENT_MAP();
		}

		if (entity.HasComponent<ScriptComponent>()) 
		{
			auto& scriptComponent = entity.GetComponent<ScriptComponent>();
			BEGIN_COMPONENT_MAP("ScriptComponent");

			WRITE_COMPONENT_PROPERY("ModuleName", scriptComponent.ModuleName);
			if (!scriptComponent.PublicFieldIDs.empty()) 
			{
				WRITE_COMPONENT_PROPERY("Fields", YAML::BeginSeq);
				SerializeScriptFields(out, entity);
				out << YAML::EndSeq;
			}

			END_COMPONENT_MAP();
		}

		if (entity.HasComponent<Rigidbody2DComponent>()) 
		{
			auto& rigidbodyComponent = entity.GetComponent<Rigidbody2DComponent>();
			BEGIN_COMPONENT_MAP("Rigidbody2DComponent");

			WRITE_COMPONENT_PROPERY("BodyType", PhysicsBodyTypeToString(rigidbodyComponent.BodyType));
			WRITE_COMPONENT_PROPERY("FixedRotation", rigidbodyComponent.FixedRotation);
			WRITE_COMPONENT_PROPERY("SleepState", PhysicsBodySleepStateToString(rigidbodyComponent.SleepState));
			WRITE_COMPONENT_PROPERY("GravityScale", rigidbodyComponent.GravityScale);

			END_COMPONENT_MAP();
		}

		if (entity.HasComponent<BoxCollider2DComponent>()) 
		{
			auto& boxCollider2DComponent = entity.GetComponent<BoxCollider2DComponent>();
			BEGIN_COMPONENT_MAP("BoxCollider2DComponent");

			WRITE_COMPONENT_PROPERY("Offset", boxCollider2DComponent.Offset);
			WRITE_COMPONENT_PROPERY("Size", boxCollider2DComponent.Size);
			WRITE_COMPONENT_PROPERY("Sensor", boxCollider2DComponent.Sensor);

			END_COMPONENT_MAP();
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			auto& circleCollider2DComponent = entity.GetComponent<CircleCollider2DComponent>();
			BEGIN_COMPONENT_MAP("CircleCollider2DComponent");

			WRITE_COMPONENT_PROPERY("Offset", circleCollider2DComponent.Offset);
			WRITE_COMPONENT_PROPERY("Radius", circleCollider2DComponent.Radius);
			WRITE_COMPONENT_PROPERY("Sensor", circleCollider2DComponent.Sensor);

			END_COMPONENT_MAP();
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::SerializeEditor(const std::filesystem::path& scenePath)
	{
		YAML::Emitter out;
		
		out << YAML::BeginMap;

		out << YAML::Key << "SerializerVersion" << YAML::Value << SerializerVersion;
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		const auto tagComponentView = m_Scene->GetEntitiesWith<TagComponent>();
		for (auto e : tagComponentView)
		{
			Entity entity(e, m_Scene.get());
			SerializeEntity(out, entity);
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream ofs(scenePath);
		ofs << out.c_str();
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

	static bool DesirializeEntity(YAML::Node data, Shared<Scene> scene)
	{
		UUID id = data["Entity"].as<UUID>();
		if (!id) return false;

		auto tagComponent = data["TagComponent"];
		if (!tagComponent) return false;
		std::string name = tagComponent["Tag"].as<std::string>();
		Entity entity = scene->CreateEntityWithUUID(name, id);

		auto transformComponent = data["TransformComponent"];
		if (transformComponent) 
		{
			auto& tc = entity.GetTransform();
			tc.Position = transformComponent["Position"].as<glm::vec3>(glm::vec3(0.0f, 0.0f, 0.0f));
			tc.Rotation = transformComponent["Rotation"].as<glm::vec3>(glm::vec3(0.0f, 0.0f, 0.0f));
			tc.Scale = transformComponent["Scale"].as<glm::vec3>(glm::vec3(1.0f, 1.0f, 1.0f));
		}

		auto cameraComponent = data["CameraComponent"];
		if (cameraComponent) 
		{
			auto& cc = entity.AddComponent<CameraComponent>();
			auto camera = cameraComponent["Camera"];
			cc.Camera.SetOrthographicSize(camera["Size"].as<float>(10.0f));
			cc.Camera.SetOrthographicNear(camera["Near"].as<float>(-10.0f));
			cc.Camera.SetOrthographicFar(camera["Far"].as<float>(10.0f));

			cc.Primary = cameraComponent["Primary"].as<bool>(false);
			cc.BackgroundColor = cameraComponent["ClearColor"].as<glm::vec4>(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		}

		auto spriteRendererComponent = data["SpriteRendererComponent"];
		if (spriteRendererComponent) 
		{
			auto& src = entity.AddComponent<SpriteRendererComponent>();
			src.Color = spriteRendererComponent["Color"].as<glm::vec4>(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			src.TextureHandle = spriteRendererComponent["Texture"].as<UUID>();
		}

		auto relationshipComponent = data["RelationshipComponent"];
		if (relationshipComponent) 
		{

		}

#if 0
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
				spriteRenderer.TextureHandle = UUID::FromString(jEntity["SpriteRendererComponent"]["Texture"]);

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

					Entity parent = scene->FindEntityWithUUID(UUID::FromString(jRelation["Parent"]));
					entity.SetParent(parent);
				}

			}
			CATCH_JSON_EXCEPTION();
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
				bcComponent.Sensor = isSensor;
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
				ccComponent.Sensor = isSensor;
			}
			CATCH_JSON_EXCEPTION();
		}
#endif
		
		return true;
	}

	bool SceneSerializer::DesirializeEditior(const std::filesystem::path& scenePath)
	{
		YAML::Node data;
		try 
		{
			data = YAML::LoadFile(scenePath.string());
		}
		catch (const YAML::ParserException& ex) 
		{
			TR_ERROR(ex.what());
			return false;
		}

		auto entities = data["Entities"];
		if (entities) 
		{
			for (auto entity : entities)
			{
				DesirializeEntity(entity, m_Scene);
			}
		}

		return true;
	}
}
