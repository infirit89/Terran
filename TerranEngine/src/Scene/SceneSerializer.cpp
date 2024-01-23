#include "trpch.h"

#include "SceneSerializer.h"
#include "Entity.h"

#include "Scripting/ScriptField.h"
#include "Scripting/ScriptCache.h"
#include "Scripting/ScriptEngine.h"

#include "Assets/AssetManager.h"

#include "Utils/SerializerUtils.h"

#include <yaml-cpp/yaml.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

// NOTE: this is not the final version of the scene serializer, this is a poc
// NOTE: think about using yaml instead of json, because json has some limitation that i dont really like

namespace TerranEngine 
{
	const char* SceneSerializer::SceneFilter = "Terran Scene\0*.terran\0";
	static const char* SerializerVersion = "yml1.0";

	SceneSerializer::SceneSerializer(const Shared<Scene>& scene)
		: m_Scene(scene) { }

#define WRITE_SCRIPT_FIELD(FieldType, Type)\
	case NativeType::FieldType:\
	out << YAML::Key << field->GetName() << YAML::Value << field->GetData<Type>(handle);\
	break

#define WRITE_SCRIPT_FIELD_ARRAY_ELEMENT(FieldType, Type)\
	case NativeType::FieldType:\
	{\
	Type value = array.At(i);\
	out << value;\
	}\
	break

	static void SerializeScriptFields(YAML::Emitter& out, Entity entity) 
	{
		ScriptComponent& sc = entity.GetComponent<ScriptComponent>();
		GCHandle handle = ScriptEngine::GetScriptInstanceGCHandle(entity.GetSceneID(), entity.GetID());
		
		for (auto& fieldID : sc.PublicFieldIDs)
		{
			out << YAML::BeginMap;
			//ScriptField* field = ScriptCache::GetCachedFieldFromID(fieldID);

			//if(field->GetType().IsArray()) 
			//{
			//	ScriptArray array = field->GetArray(handle);
			//	out << YAML::Key << field->GetName() << YAML::Value << YAML::Flow << YAML::BeginSeq;
			//	
			//	for(size_t i = 0; i < array.Length(); i++) 
			//	{
			//		switch (array.GetElementType().GetNativeType())
			//		{
			//			WRITE_SCRIPT_FIELD_ARRAY_ELEMENT(Bool, bool);
			//			// NOTE: maybe wchar_t?
			//			WRITE_SCRIPT_FIELD_ARRAY_ELEMENT(Char, char);
			//			WRITE_SCRIPT_FIELD_ARRAY_ELEMENT(Int8, int8_t);
			//			WRITE_SCRIPT_FIELD_ARRAY_ELEMENT(Int16, int16_t);
			//			WRITE_SCRIPT_FIELD_ARRAY_ELEMENT(Int32, int32_t);
			//			WRITE_SCRIPT_FIELD_ARRAY_ELEMENT(Int64, int64_t);
			//			WRITE_SCRIPT_FIELD_ARRAY_ELEMENT(UInt8, uint8_t);
			//			WRITE_SCRIPT_FIELD_ARRAY_ELEMENT(UInt16, uint16_t);
			//			WRITE_SCRIPT_FIELD_ARRAY_ELEMENT(UInt32, uint32_t);
			//			WRITE_SCRIPT_FIELD_ARRAY_ELEMENT(UInt64, uint64_t);
			//			WRITE_SCRIPT_FIELD_ARRAY_ELEMENT(Float, float);
			//			WRITE_SCRIPT_FIELD_ARRAY_ELEMENT(Double, double);
			//			WRITE_SCRIPT_FIELD_ARRAY_ELEMENT(String, std::string);
			//			WRITE_SCRIPT_FIELD_ARRAY_ELEMENT(Vector2, glm::vec2);
			//			WRITE_SCRIPT_FIELD_ARRAY_ELEMENT(Vector3, glm::vec3);
			//			WRITE_SCRIPT_FIELD_ARRAY_ELEMENT(Color, glm::vec4);
			//			WRITE_SCRIPT_FIELD_ARRAY_ELEMENT(Entity, UUID);
			//			default: TR_ASSERT(false, "Unsupported field type"); break;
			//		}
			//	}

			//	out << YAML::EndSeq;
			//}
			//else 
			//{
			//	switch (field->GetType().GetNativeType())
			//	{
			//		WRITE_SCRIPT_FIELD(Bool, bool);
			//		WRITE_SCRIPT_FIELD(Char, wchar_t);
			//		WRITE_SCRIPT_FIELD(Int8, int8_t);
			//		WRITE_SCRIPT_FIELD(Int16, int16_t);
			//		WRITE_SCRIPT_FIELD(Int32, int32_t);
			//		WRITE_SCRIPT_FIELD(Int64, int64_t);
			//		WRITE_SCRIPT_FIELD(UInt8, uint8_t);
			//		WRITE_SCRIPT_FIELD(UInt16, uint16_t);
			//		WRITE_SCRIPT_FIELD(UInt32, uint32_t);
			//		WRITE_SCRIPT_FIELD(UInt64, uint64_t);
			//		WRITE_SCRIPT_FIELD(Float, float);
			//		WRITE_SCRIPT_FIELD(Double, double);
			//		WRITE_SCRIPT_FIELD(String, std::string);
			//		WRITE_SCRIPT_FIELD(Vector2, glm::vec2);
			//		WRITE_SCRIPT_FIELD(Vector3, glm::vec3);
			//		WRITE_SCRIPT_FIELD(Color, glm::vec4);
			//		WRITE_SCRIPT_FIELD(Entity, UUID);
			//		default: TR_ASSERT(false, "Unsupported field type"); break;
			//	}
			//}

			out << YAML::EndMap;
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

			/*Shared<Texture2D> texture = 
					AssetManager::GetAsset<Texture2D>(spriteRendererComponent.TextureHandle);

			WRITE_COMPONENT_PROPERY("TextureFilter",
				(texture ? texture->GetTextureParameters().Filter : TextureFilter::Linear));*/
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
				//WRITE_COMPONENT_PROPERY("Fields", YAML::BeginSeq);
				out << YAML::Key << "Fields" << YAML::Value;
				out << YAML::BeginSeq;
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

		if (entity.HasComponent<CapsuleCollider2DComponent>()) 
		{
			auto& capsuleColliderComponenet = entity.GetComponent<CapsuleCollider2DComponent>();

			BEGIN_COMPONENT_MAP("CapsuleCollider2DComponent");
			WRITE_COMPONENT_PROPERY("Offset", capsuleColliderComponenet.Offset);
			WRITE_COMPONENT_PROPERY("Size", capsuleColliderComponenet.Size);
			WRITE_COMPONENT_PROPERY("Sensor", capsuleColliderComponenet.Sensor);
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

#define READ_SCRIPT_FIELD(FieldType, Type)\
	case NativeType::FieldType:\
	{\
	Type value = scriptField.as<Type>();\
	cachedField->SetData(value, handle);\
	}\
	break

#define READ_SCRIPT_FIELD_ARRAY_ELEMENT(FieldType, Type)\
	case NativeType::FieldType:\
	{\
	Utils::Variant value = scriptField[i].as<Type>();\
	array.Set(i, value);\
	}\
	break

	static void DeserializeScriptFields(GCHandle handle, ScriptComponent& scriptComponent, YAML::Node scriptFields) 
	{
		for (const auto& fieldID : scriptComponent.PublicFieldIDs)
		{
			/*ScriptField* cachedField = ScriptCache::GetCachedFieldFromID(fieldID);
			YAML::Node scriptField;
			bool valid = false;
			
			for (auto field : scriptFields)
			{
				if (field[cachedField->GetName()]) 
				{
					scriptField = field[cachedField->GetName()];
					valid = true;
					break;
				}
			}

			if (!valid) continue;
			
			TR_TRACE(scriptField);

			if (cachedField->GetType().IsArray()) 
			{
				ScriptArray array = cachedField->GetArray(handle);
				for(size_t i = 0; i < array.Length() && i < scriptField.size(); i++) 
				{
					switch(array.GetElementType().GetNativeType())
					{
						READ_SCRIPT_FIELD_ARRAY_ELEMENT(Bool, bool);
						READ_SCRIPT_FIELD_ARRAY_ELEMENT(Char, char);
						READ_SCRIPT_FIELD_ARRAY_ELEMENT(Int64, int64_t);
						READ_SCRIPT_FIELD_ARRAY_ELEMENT(Int32, int32_t);
						READ_SCRIPT_FIELD_ARRAY_ELEMENT(Int16, int16_t);
						READ_SCRIPT_FIELD_ARRAY_ELEMENT(Int8, int8_t);
						READ_SCRIPT_FIELD_ARRAY_ELEMENT(UInt64, uint64_t);
						READ_SCRIPT_FIELD_ARRAY_ELEMENT(UInt32, uint32_t);
						READ_SCRIPT_FIELD_ARRAY_ELEMENT(UInt16, uint16_t);
						READ_SCRIPT_FIELD_ARRAY_ELEMENT(UInt8, uint8_t);
						READ_SCRIPT_FIELD_ARRAY_ELEMENT(Float, float);
						READ_SCRIPT_FIELD_ARRAY_ELEMENT(Double, double);
						READ_SCRIPT_FIELD_ARRAY_ELEMENT(String, std::string);
						READ_SCRIPT_FIELD_ARRAY_ELEMENT(Vector2, glm::vec2);
						READ_SCRIPT_FIELD_ARRAY_ELEMENT(Vector3, glm::vec3);
						READ_SCRIPT_FIELD_ARRAY_ELEMENT(Color, glm::vec4);
						READ_SCRIPT_FIELD_ARRAY_ELEMENT(Entity, UUID);
					}
				}
			}
			else
			{
				switch (cachedField->GetType().GetNativeType())
				{
					READ_SCRIPT_FIELD(Bool, bool);
					READ_SCRIPT_FIELD(Char, char);
					READ_SCRIPT_FIELD(Int64, int64_t);
					READ_SCRIPT_FIELD(Int32, int32_t);
					READ_SCRIPT_FIELD(Int16, int16_t);
					READ_SCRIPT_FIELD(Int8, int8_t);
					READ_SCRIPT_FIELD(UInt64, uint64_t);
					READ_SCRIPT_FIELD(UInt32, uint32_t);
					READ_SCRIPT_FIELD(UInt16, uint16_t);
					READ_SCRIPT_FIELD(UInt8, uint8_t);
					READ_SCRIPT_FIELD(Float, float);
					READ_SCRIPT_FIELD(Double, double);
					READ_SCRIPT_FIELD(String, std::string);
					READ_SCRIPT_FIELD(Vector2, glm::vec2);
					READ_SCRIPT_FIELD(Vector3, glm::vec3);
					READ_SCRIPT_FIELD(Color, glm::vec4);
					READ_SCRIPT_FIELD(Entity, UUID);
				}
			}*/

		}
	}

	static YAML::Node FindEntity(YAML::Node scene, const UUID& entityID)
	{
		for (auto entity : scene)
		{
			UUID id = entity["Entity"].as<UUID>();
			if (id == entityID) return entity;
		}

		return { };
	}

	static Entity DeserializeEntity(YAML::Node data, YAML::Node scene, Shared<Scene> deserializedScene)
	{
		UUID id = data["Entity"].as<UUID>();
		if (!id)
		{
			TR_ASSERT(false, "Invalid id");
			return { };
		}

		Entity entity = deserializedScene->FindEntityWithUUID(id);
		if (entity) return entity;

		auto tagComponent = data["TagComponent"];
		if (!tagComponent)
		{
			TR_ASSERT(false, "Invalid tag component");
			return { };
		}

		std::string name = tagComponent["Tag"].as<std::string>();
		Entity deserializedEntity = deserializedScene->CreateEntityWithUUID(name, id);

		auto transformComponent = data["TransformComponent"];
		if (transformComponent) 
		{
			auto& tc = deserializedEntity.GetTransform();
			tc.Position = transformComponent["Position"].as<glm::vec3>(glm::vec3(0.0f, 0.0f, 0.0f));
			tc.Rotation = transformComponent["Rotation"].as<glm::vec3>(glm::vec3(0.0f, 0.0f, 0.0f));
			tc.Scale = transformComponent["Scale"].as<glm::vec3>(glm::vec3(1.0f, 1.0f, 1.0f));
		}

		auto cameraComponent = data["CameraComponent"];
		if (cameraComponent) 
		{
			auto& cc = deserializedEntity.AddComponent<CameraComponent>();
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
			auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
			src.Color = spriteRendererComponent["Color"].as<glm::vec4>(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			src.TextureHandle = spriteRendererComponent["Texture"].as<UUID>();
		}

		auto circleRendererComponent = data["CircleRendererComponent"];
		if (circleRendererComponent) 
		{
			auto& crc = deserializedEntity.AddComponent<CircleRendererComponent>();
			crc.Color = circleRendererComponent["Color"].as<glm::vec4>(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			crc.Thickness = circleRendererComponent["Thickness"].as<float>(1.0f);
		}

		auto relationshipComponent = data["RelationshipComponent"];
		if (relationshipComponent) 
		{
			// TODO:
			auto& rc = deserializedEntity.AddComponent<RelationshipComponent>();
			for (auto childID : relationshipComponent["Children"])
			{
				UUID deserializedChildID = childID.as<UUID>();
				Entity child = deserializedScene->FindEntityWithUUID(deserializedChildID);
				if (!child)
				{
					// todo deserialize;
					YAML::Node childNode = FindEntity(scene, deserializedChildID);
					child = DeserializeEntity(childNode, scene, deserializedScene);
				}

				if(child)
					child.SetParent(deserializedEntity, true);
			}
		}

		auto scriptComponent = data["ScriptComponent"];
		if (scriptComponent)
		{
			// TODO:
			auto& sc = deserializedEntity.AddComponent<ScriptComponent>();
			sc.ModuleName = scriptComponent["ModuleName"].as<std::string>();

			if (ScriptEngine::ClassExists(sc.ModuleName)) 
			{
				//GCHandle handle = ScriptEngine::InitializeScriptable(deserializedEntity);
				//auto scriptFields = scriptComponent["Fields"];
				//if (scriptFields)
					//DeserializeScriptFields(handle, sc, scriptFields);
			}
		}

		auto boxColliderComponent = data["BoxCollider2DComponent"];
		if (boxColliderComponent) 
		{
			auto& bcc = deserializedEntity.AddComponent<BoxCollider2DComponent>();
			bcc.Offset = boxColliderComponent["Offset"].as<glm::vec2>(glm::vec2(0.0f, 0.0f));
			bcc.Size = boxColliderComponent["Size"].as<glm::vec2>(glm::vec2(1.0f, 1.0f));
			bcc.Sensor = boxColliderComponent["Sensor"].as<bool>(false);
		}

		auto circleColliderComponent = data["CircleCollider2DComponent"];
		if (circleRendererComponent) 
		{
			auto& ccc = deserializedEntity.AddComponent<CircleCollider2DComponent>();
			ccc.Offset = circleColliderComponent["Offset"].as<glm::vec2>(glm::vec2(0.0f, 0.0f));
			ccc.Radius = circleColliderComponent["Radius"].as<float>(0.5f);
			ccc.Sensor = circleColliderComponent["Sensor"].as<bool>(false);
		}

		auto rigidbodyComponent = data["Rigidbody2DComponent"];
		if (rigidbodyComponent) 
		{
			auto& rbc = deserializedEntity.AddComponent<Rigidbody2DComponent>();
			rbc.BodyType = PhysicsBodyTypeFromString(rigidbodyComponent["BodyType"].as<std::string>());
			rbc.FixedRotation = rigidbodyComponent["FixedRotation"].as<bool>(false);
			rbc.SleepState = PhysicsBodySleepStateFromString(rigidbodyComponent["SleepState"].as<std::string>());
			rbc.GravityScale = rigidbodyComponent["GravityScale"].as<float>(1.0f);
		}
		
		auto capsuleColliderComponent = data["CapsuleCollider2DComponent"];
		if (capsuleColliderComponent)
		{
			auto& ccc = deserializedEntity.AddComponent<CapsuleCollider2DComponent>();
			ccc.Offset = capsuleColliderComponent["Offset"].as<glm::vec2>(glm::vec2(0.0f, 0.0f));
			ccc.Size = capsuleColliderComponent["Size"].as<glm::vec2>(glm::vec2(0.5f, 1.0f));
			ccc.Sensor = capsuleColliderComponent["Sensor"].as<bool>(false);
		}

		return deserializedEntity;
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
				DeserializeEntity(entity, entities, m_Scene);
			}
		}

		return true;
	}
}
