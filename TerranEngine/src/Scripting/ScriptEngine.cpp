#include "trpch.h"
#include "ScriptEngine.h"

#include "ScriptString.h"
#include "ScriptBindings.h"
#include "ScriptMarshal.h"

#include "Core/Log.h"
#include "Core/FileUtils.h"

#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Scene/SceneManager.h"

#include <glm/glm.hpp>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/mono-debug.h>
#include <mono/utils/mono-logger.h>

#include <unordered_map>

namespace TerranEngine
{
	static MonoDomain* s_CoreDomain;
	static MonoDomain* s_NewDomain;

	static MonoAssembly* s_CurrentAssembly;
	
	static MonoImage* s_CurrentImage;
	
	static const char* s_AssemblyPath;

	static std::unordered_map<uint32_t, ScriptClass> s_Classes;

	static MonoAssembly* LoadAssembly(const char* fileName);
	static MonoImage* GetImageFromAssemly(MonoAssembly* assembly);

	static void OnLogMono(const char* log_domain, const char* log_level, const char* message, mono_bool fatal, void* user_data);

	static ScriptMethod GetMethodFromImage(MonoImage* image, const char* methodSignature);

	struct ScriptableInstance 
	{
		ScriptObject Object;
		ScriptMethod Constructor;
		ScriptMethod InitMethod;
		ScriptMethod UpdateMethod;

		ScriptMethod PhysicsBeginContact;

		void GetMethods(ScriptClass& scriptClass) 
		{
			Constructor = GetMethodFromImage(s_CurrentImage, "TerranScriptCore.Scriptable:.ctor(byte[])");

			InitMethod = scriptClass.GetMethod(":Init()");
			UpdateMethod = scriptClass.GetMethod(":Update()");

			PhysicsBeginContact = scriptClass.GetMethod(":BeginContact(Entity)");
		}
	};

	static ScriptableInstance s_EmptyInstance;

	static std::unordered_map<UUID, std::unordered_map<UUID, ScriptableInstance>> s_ScriptableInstanceMap;

	static std::unordered_map<UUID, std::unordered_map<std::string, ScriptFieldBackup>> s_ScriptFieldBackup;

	static ScriptableInstance& GetInstance(const UUID& sceneUUID, const UUID& entityUUID) 
	{
		if (s_ScriptableInstanceMap.find(sceneUUID) != s_ScriptableInstanceMap.end()) 
		{
			if (s_ScriptableInstanceMap[sceneUUID].find(entityUUID) != s_ScriptableInstanceMap[sceneUUID].end())
				return s_ScriptableInstanceMap[sceneUUID][entityUUID];
		}

		return s_EmptyInstance;
	}

	void ScriptEngine::Init(const char* fileName)
	{
		s_AssemblyPath = fileName;
		std::string monoPath = FileUtils::GetEnvironmentVariable("MONO_PATH");
		//std::string monoPath = "Resources/Mono/";

		std::string libPath = monoPath + "lib";
		std::string etcPath = monoPath + "etc";

		mono_set_dirs(libPath.c_str(), etcPath.c_str());

#ifdef TR_DEBUG

		mono_debug_init(MONO_DEBUG_FORMAT_MONO);

		mono_trace_set_level_string("debug");

		mono_trace_set_log_handler(OnLogMono, nullptr);

#endif

		s_CoreDomain = mono_jit_init("CoreDomain");
		
		if (!s_CoreDomain)
		{
			TR_ERROR("Couldn't initialize the Mono Domain!");
			return;
		}

		NewDomain();
	}

	void ScriptEngine::CleanUp()
	{
		mono_jit_cleanup(s_CoreDomain);
		TR_INFO("Deinitialized the scripting core");
	}

	void ScriptEngine::NewDomain() 
	{
		s_NewDomain = mono_domain_create_appdomain("ScriptAssemblyDomain", NULL);
		
		if (!mono_domain_set(s_NewDomain, false)) 
		{
			TR_ERROR("Couldn't set the new domain");
			return;
		}

		s_CurrentAssembly = LoadAssembly(s_AssemblyPath);

		if (!s_CurrentAssembly) return;

		s_CurrentImage = GetImageFromAssemly(s_CurrentAssembly);

		if (!s_CurrentImage) return;

		TR_INFO("Successfuly loaded: {0}", s_AssemblyPath);

		ScriptBindings::Bind();
	}

	std::string ScriptEngine::GetAssemblyPath() 
	{
		return s_AssemblyPath;
	}

	void ScriptEngine::UnloadDomain() 
	{
		if (s_NewDomain != s_CoreDomain) 
		{
			SetCurrentFieldStates(SceneManager::GetCurrentScene()->GetID());

			mono_domain_set(s_CoreDomain, false);

			if (!mono_domain_finalize(s_NewDomain, 2000))
			{
				TR_ERROR("Finalizing the domain timed out");
				return;
			}
			mono_gc_collect(mono_gc_max_generation());

			auto scriptView = SceneManager::GetCurrentScene()->GetEntitiesWith<ScriptComponent>();

			for (auto e : scriptView)
			{
				Entity entity(e, SceneManager::GetCurrentScene().get());

				UninitalizeScriptable(entity);
			}

			mono_domain_unload(s_NewDomain);

			s_Classes.clear();
		}
	}

	ScriptClass ScriptEngine::GetClass(const std::string& moduleName)
	{
		if (!s_CurrentImage)
		{
			TR_ERROR("Can't locate the class {0}, as there is no loaded script image", moduleName);
			return ScriptClass();
		}

		std::hash<std::string> hasher;
		uint32_t hashedName = hasher(moduleName);

		if (s_Classes.find(hashedName) != s_Classes.end())
			return s_Classes[hashedName];
		
		size_t dotPosition = moduleName.find_last_of(".");

		std::string& namespaceName = moduleName.substr(0, dotPosition);
		std::string& className = moduleName.substr(dotPosition + 1);
		
		// NOTE: Access violation when the script assembly isn't loaded
		MonoClass* klass = mono_class_from_name(s_CurrentImage, namespaceName.c_str(), className.c_str());

		if (!klass) 
		{
			TR_ERROR("Class wasn't found");
			return ScriptClass();
		}
		ScriptClass scriptClass(klass);
		s_Classes[hashedName] = scriptClass;

		return scriptClass;
	}

	bool ScriptEngine::ClassExists(const std::string& moduleName)
	{
		return GetClass(moduleName).GetNativeClassPtr() != nullptr;
	}

	static ScriptMethod GetMethodFromImage(MonoImage* image, const char* methodSignature)
	{
		MonoMethodDesc* monoDesc = mono_method_desc_new(methodSignature, false);
		if (!monoDesc)
		{
			TR_ERROR("Couldn't find a matching description ({0}) in the image", methodSignature);

			return NULL;
		}

		MonoMethod* monoMethod = mono_method_desc_search_in_image(monoDesc, image);

		if (!monoMethod)
		{
			TR_ERROR("Couldn't find the method with signature: {0} in image", methodSignature);

			return NULL;
		}

		mono_method_desc_free(monoDesc);

		return ScriptMethod(monoMethod);
	}

	void ScriptEngine::InitializeScriptable(Entity entity)
	{
		if (s_ScriptableInstanceMap.find(entity.GetID()) == s_ScriptableInstanceMap.end())
		{
			ScriptComponent& scriptComponent = entity.GetComponent<ScriptComponent>();
			ScriptClass klass = ScriptEngine::GetClass(scriptComponent.ModuleName);
			if (!klass.GetNativeClassPtr())
			{
				TR_WARN("Couldn't find the class: {0}", scriptComponent.ModuleName);
				return;
			}

			ScriptClass parentKlass = klass.GetParent();
			if ((!parentKlass.GetNativeClassPtr()) ||
				(strcmp(parentKlass.GetName(), "Scriptable") != 0 && strcmp(parentKlass.GetNamespace(), "TerranScriptCore") != 0))
			{
				// TODO: display error in ui
				TR_WARN("Class {0} doesn't extend Scriptable", scriptComponent.ModuleName);
				return;
			}

			ScriptableInstance instance;
			instance.Object = klass.CreateInstance();
			instance.GetMethods(klass);

			MonoArray* uuidArray = ScriptMarshal::UUIDToMonoArray(entity.GetID());

			void* args[] = { uuidArray };

			instance.Constructor.Invoke(instance.Object, args);

			s_ScriptableInstanceMap[entity.GetSceneID()][entity.GetID()] = instance;

			if (!s_ScriptFieldBackup.empty()) 
			{
				std::unordered_map<std::string, ScriptFieldBackup> fieldBackup = s_ScriptFieldBackup.at(entity.GetID());
				for (auto& [fieldName, field] : fieldBackup)
				{
					std::hash<std::string> hasher;
					uint32_t hashedName = hasher(fieldName);
					if (instance.Object.GetFieldMap().find(hashedName) != instance.Object.GetFieldMap().end()) 
					{
						ScriptField& objectField = instance.Object.GetFieldMap().at(hashedName);

						if (objectField.GetType() == field.Type) 
						{
							switch (field.Type)
							{
							case ScriptFieldType::Bool: 
							{
								bool value = field.Data;
								objectField.SetData(value);
								break;
							}
							case ScriptFieldType::Int8:
							{
								int8_t value = field.Data;
								objectField.SetData(value);
								break;
							}
							case ScriptFieldType::Int16:
							{
								int16_t value = field.Data;
								objectField.SetData(value);
								break;
							}
							case ScriptFieldType::Int:
							{
								int32_t value = field.Data;
								objectField.SetData(value);
								break;
							}
							case ScriptFieldType::Int64:
							{
								int64_t value = field.Data;
								objectField.SetData(value);
								break;
							}
							case ScriptFieldType::UInt8:
							{
								uint8_t value = field.Data;
								objectField.SetData(value);
								break;
							}
							case ScriptFieldType::UInt16:
							{
								uint16_t value = field.Data;
								objectField.SetData(value);
								break;
							}
							case ScriptFieldType::UInt:
							{
								uint32_t value = field.Data;
								objectField.SetData(value);
								break;
							}
							case ScriptFieldType::UInt64:
							{
								uint64_t value = field.Data;
								objectField.SetData(value);
								break;
							}
							case ScriptFieldType::Float: 
							{
								float value = field.Data;
								objectField.SetData(value);
								break;
							}
							case ScriptFieldType::Double: 
							{
								double value = field.Data;
								objectField.SetData(value);
								break;
							}
							case ScriptFieldType::String: 
							{
								const char* value = field.Data;
								objectField.SetData<const char*>(value);
								break;
							}
							case ScriptFieldType::Vector2: 
							{
								glm::vec2 value = field.Data;
								objectField.SetData(value);
								break;
							}
							case ScriptFieldType::Vector3:
							{
								glm::vec3 value = field.Data;
								objectField.SetData(value);
								break;
							}
							}
						}
					}
				}
			}

			scriptComponent.FieldOrder = instance.Object.GetFieldOrder();
			scriptComponent.PublicFields = instance.Object.GetFieldMap();
		}
	}

	void ScriptEngine::UninitalizeScriptable(Entity entity)
	{
		if (s_ScriptableInstanceMap.find(entity.GetSceneID()) != s_ScriptableInstanceMap.end()) 
		{
			if (s_ScriptableInstanceMap[entity.GetSceneID()].find(entity.GetID()) != s_ScriptableInstanceMap[entity.GetSceneID()].end()) 
			{
				s_ScriptableInstanceMap[entity.GetSceneID()][entity.GetID()].Object.Uninitialize();
				s_ScriptableInstanceMap[entity.GetSceneID()].erase(entity.GetID());
			}
		}
	}

	void ScriptEngine::StartScriptable(Entity entity) 
	{
		ScriptableInstance instace = GetInstance(entity.GetSceneID(), entity.GetID());

		entity.GetComponent<ScriptComponent>().Started = true;

		if (instace.InitMethod.GetNativeMethodPtr())
			instace.InitMethod.Invoke(instace.Object, nullptr);
	}

	void ScriptEngine::UpdateScriptable(Entity entity) 
	{
		ScriptableInstance instance = GetInstance(entity.GetSceneID(), entity.GetID());

		if (instance.UpdateMethod.GetNativeMethodPtr())
			instance.UpdateMethod.Invoke(instance.Object, nullptr);
	}

	void ScriptEngine::PhysicsBeginContact(Entity collider, Entity collidee)
	{
		ScriptableInstance instance = GetInstance(collider.GetSceneID(), collider.GetID());

		if (instance.PhysicsBeginContact.GetNativeMethodPtr()) 
		{
			MonoArray* monoUuidArr = ScriptMarshal::UUIDToMonoArray(collidee.GetID());

			void* args[] = { monoUuidArr };

			instance.PhysicsBeginContact.Invoke(instance.Object, args);
		}
	}

	ScriptObject ScriptEngine::GetScriptInstanceScriptObject(const UUID& sceneUUID, const UUID& entityUUID)
	{
		ScriptableInstance instance = GetInstance(sceneUUID, entityUUID);

		return instance.Object;
	}

	void ScriptEngine::ClearFieldBackupMap()
	{
		auto scriptFieldBackupCpy = s_ScriptFieldBackup;
		for (auto& [entityID, fieldBackupMap] : scriptFieldBackupCpy)
		{
			auto fieldBackupCpy = fieldBackupMap;
			for (auto& [fieldName, field] : fieldBackupCpy)
			{
				switch (field.Type)
				{
				case ScriptFieldType::String:
				{
					if (field.Data.ptr)
						delete[](char*)field.Data.ptr;
					break;
				}
				case ScriptFieldType::Vector2:
				{
					if (field.Data.ptr)
						delete (glm::vec2*)field.Data.ptr;
					break;
				}
				case ScriptFieldType::Vector3:
				{
					if (field.Data.ptr)
						delete (glm::vec3*)field.Data.ptr;
					break;
				}
				}

				fieldBackupMap.erase(fieldName);
			}

			s_ScriptFieldBackup.erase(entityID);
		}
	}

	void ScriptEngine::SetCurrentFieldStates(const UUID& sceneID)
	{
		if (s_ScriptableInstanceMap.find(sceneID) != s_ScriptableInstanceMap.end())
		{
			std::unordered_map<UUID, ScriptableInstance> entityInstanceMap = s_ScriptableInstanceMap.at(sceneID);

			for (auto& [id, scriptableInstance] : entityInstanceMap)
			{
				UUID entityID = id;
				std::unordered_map<std::string, ScriptFieldBackup> fieldBackupMap;

				for (auto& [fieldName, field] : scriptableInstance.Object.GetFieldMap())
				{
					ScriptFieldBackup fieldBackup;
					fieldBackup.Type = field.GetType();
					switch (field.GetType())
					{
					case ScriptFieldType::Int8:
					case ScriptFieldType::Int16:
					case ScriptFieldType::Int:
					case ScriptFieldType::Int64:
					case ScriptFieldType::UInt8:
					case ScriptFieldType::UInt16:
					case ScriptFieldType::UInt:
					case ScriptFieldType::UInt64:
					{
						fieldBackup.Data.iValue = field.GetData<int64_t>();
						break;
					}
					case ScriptFieldType::Float:
					case ScriptFieldType::Double:
					{
						fieldBackup.Data.dValue = field.GetData<double>();
						break;
					}
					case ScriptFieldType::Bool:
					{
						fieldBackup.Data.bValue = field.GetData<bool>();
						break;
					}
					case ScriptFieldType::String: 
					{
						const char* tempVal = field.GetData<const char*>();
						size_t tempValLength = strlen(tempVal);

						fieldBackup.Data.ptr = new char[tempValLength + 1];

						strcpy((char*)fieldBackup.Data.ptr, tempVal);

						((char*)fieldBackup.Data.ptr)[tempValLength] = '\0';
						break;
					}
					case ScriptFieldType::Vector2: 
					{
						glm::vec2 tempVal = field.GetData<glm::vec2>();

						fieldBackup.Data.ptr = new glm::vec2;

						((glm::vec2*)fieldBackup.Data.ptr)->x = tempVal.x;
						((glm::vec2*)fieldBackup.Data.ptr)->y = tempVal.y;

						break;
					}
					case ScriptFieldType::Vector3:
					{
						glm::vec3 tempVal = field.GetData<glm::vec3>();

						fieldBackup.Data.ptr = new glm::vec3;

						((glm::vec3*)fieldBackup.Data.ptr)->x = tempVal.x;
						((glm::vec3*)fieldBackup.Data.ptr)->y = tempVal.y;
						((glm::vec3*)fieldBackup.Data.ptr)->z = tempVal.z;

						break;
					}
					}

					fieldBackupMap.emplace(field.GetName(), std::move(fieldBackup));
				}

				s_ScriptFieldBackup.emplace(entityID, std::move(fieldBackupMap));
			}
		}

	}

	static MonoAssembly* LoadAssembly(const char* fileName) 
	{
		MonoAssembly* newAssembly = mono_domain_assembly_open(s_NewDomain, fileName);

		if (!newAssembly)
		{
			TR_ERROR("Couldn't load the C# assembly!");
			return nullptr;
		}

		return newAssembly;
	}

	static MonoImage* GetImageFromAssemly(MonoAssembly* assembly)
	{
		MonoImage* newImage = mono_assembly_get_image(assembly);

		if (!newImage)
		{
			TR_ERROR("Couldn't find an image in the script assembly!");
			return nullptr;
		}

		return newImage;
	}

	static void OnLogMono(const char* log_domain, const char* log_level, const char* message, mono_bool fatal, void* user_data) 
	{
		if (log_level != nullptr) 
		{
			if (strcmp(log_level, "info") == 0)
				TR_INFO("Domain: {0}; Message: {1}", log_domain, message);
			else if(strcmp(log_level, "debug") == 0)
				TR_TRACE("Domain: {0}; Message: {1}", log_domain, message);
			else
				TR_TRACE("Domain: {0}; Message: {1}; Log Level: {2}", log_domain, message, log_level);
		}
	}
}
