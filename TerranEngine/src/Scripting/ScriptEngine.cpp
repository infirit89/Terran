#include "trpch.h"
#include "ScriptEngine.h"

#include "ScriptBindings.h"
#include "ScriptMarshal.h"
#include "GCManager.h"
#include "ScriptCache.h"

#include "Core/Log.h"
#include "Core/FileUtils.h"
#include "Core/Hash.h"

#include "Scene/Components.h"
#include "Scene/SceneManager.h"

#include <glm/glm.hpp>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/mono-debug.h>
#include <mono/utils/mono-logger.h>
#include <mono/metadata/mono-config.h>

#include <unordered_map>

namespace TerranEngine
{
	static MonoDomain* s_CoreDomain;
	static MonoDomain* s_NewDomain;

	static MonoAssembly* s_CurrentAssembly;
	
	static MonoImage* s_CurrentImage;
	
	static std::filesystem::path s_AssemblyPath;
	static std::filesystem::path s_MonoPath = "mono";

	static std::filesystem::path s_LibPath = s_MonoPath / "lib";
	static std::filesystem::path s_EtcPath = s_MonoPath / "etc";

	static std::filesystem::path s_MonoConfigPath = s_EtcPath / "config";

	static std::unordered_map<uint32_t, ScriptClass> s_Classes;

	static MonoAssembly* LoadAssembly(const std::filesystem::path& asseblyPath);
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
		ScriptMethod PhysicsEndContact;
		
		ScriptMethod PhysicsUpdateMethod;

		void GetMethods(ScriptClass& scriptClass) 
		{
			Constructor = GetMethodFromImage(s_CurrentImage, "Terran.Scriptable:.ctor(byte[])");

			InitMethod = scriptClass.GetMethod(":Init()");
			UpdateMethod = scriptClass.GetMethod(":Update()");

			PhysicsBeginContact = scriptClass.GetMethod(":OnCollisionBegin(Entity)");
			PhysicsEndContact = scriptClass.GetMethod(":OnCollisionEnd(Entity)");

			PhysicsUpdateMethod = scriptClass.GetMethod(":PhysicsUpdate()");
		}
	};

	union ScriptFieldData
	{
		double dValue;
		int64_t iValue;
		bool bValue;
		void* ptr;

		operator bool() { return bValue; }

		operator int8_t() { return static_cast<int8_t>(iValue); }
		operator int16_t() { return static_cast<int16_t>(iValue); }
		operator int32_t() { return static_cast<int32_t>(iValue); }
		operator int64_t() { return static_cast<int64_t>(iValue); }

		operator uint8_t() { return static_cast<uint8_t>(iValue); }
		operator uint16_t() { return static_cast<uint16_t>(iValue); }
		operator uint32_t() { return static_cast<uint32_t>(iValue); }
		operator uint64_t() { return static_cast<uint64_t>(iValue); }

		operator float() { return static_cast<float>(dValue); }
		operator double() { return static_cast<double>(dValue); }
		operator const char* () { return static_cast<const char*>(ptr); }
		operator glm::vec2() { return *static_cast<glm::vec2*>(ptr); }
		operator glm::vec3() { return *static_cast<glm::vec3*>(ptr); }
	};

	struct ScriptFieldBackup
	{
		ScriptFieldData Data;
		ScriptFieldType Type;
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

	void ScriptEngine::Initialize(const std::filesystem::path& asseblyPath)
	{
		s_AssemblyPath = asseblyPath;
		
		mono_set_dirs(s_LibPath.string().c_str(), s_EtcPath.string().c_str());

#ifdef TR_DEBUG
		mono_debug_init(MONO_DEBUG_FORMAT_MONO);
		mono_trace_set_level_string("debug");
		mono_trace_set_log_handler(OnLogMono, nullptr);
#endif

		mono_config_parse(s_MonoConfigPath.string().c_str());

		s_CoreDomain = mono_jit_init("CoreDomain");
		
		if (!s_CoreDomain)
		{
			TR_ERROR("Couldn't initialize the Mono Domain!");
			return;
		}

		NewDomain();
	}

	void ScriptEngine::Shutdown()
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

		ScriptCache::CacheCoreClasses();
		ScriptBindings::Bind();
	}

	std::filesystem::path ScriptEngine::GetAssemblyPath()
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

		ScriptCache::ClearClassCache();
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
		return GetClass(moduleName).GetMonoClassPtr() != nullptr;
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
			if (!klass.GetMonoClassPtr())
			{
				TR_WARN("Couldn't find the class: {0}", scriptComponent.ModuleName);
				return;
			}

			ScriptClass parentKlass = klass.GetParent();
			if ((!parentKlass.GetMonoClassPtr()) ||
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
		if (!entity || !entity.HasComponent<TagComponent>())
			return;

		if (s_ScriptableInstanceMap.find(entity.GetSceneID()) != s_ScriptableInstanceMap.end()) 
		{
			if (s_ScriptableInstanceMap[entity.GetSceneID()].find(entity.GetID()) != s_ScriptableInstanceMap[entity.GetSceneID()].end()) 
			{
				GCHandle& handle = s_ScriptableInstanceMap[entity.GetSceneID()][entity.GetID()].Object.GetGCHandle();
				GCManager::FreeHandle(handle);
				s_ScriptableInstanceMap[entity.GetSceneID()].erase(entity.GetID());
			}
		}
	}

	void ScriptEngine::OnStart(Entity entity) 
	{
		ScriptableInstance instace = GetInstance(entity.GetSceneID(), entity.GetID());

		entity.GetComponent<ScriptComponent>().Started = true;

		if (instace.InitMethod.GetNativeMethodPtr())
			instace.InitMethod.Invoke(instace.Object, nullptr);
	}

	void ScriptEngine::OnUpdate(Entity entity) 
	{
		ScriptableInstance instance = GetInstance(entity.GetSceneID(), entity.GetID());

		if (instance.UpdateMethod.GetNativeMethodPtr())
			instance.UpdateMethod.Invoke(instance.Object, nullptr);
	}

	void ScriptEngine::OnPhysicsBeginContact(Entity collider, Entity collidee)
	{
		ScriptableInstance instance = GetInstance(collider.GetSceneID(), collider.GetID());

		if (instance.PhysicsBeginContact.GetNativeMethodPtr()) 
		{
			MonoArray* monoUuidArr = ScriptMarshal::UUIDToMonoArray(collidee.GetID());
			void* args[] = { monoUuidArr };
			instance.PhysicsBeginContact.Invoke(instance.Object, args);
		}
	}

	void ScriptEngine::OnPhysicsEndContact(Entity collider, Entity collidee)
	{
		ScriptableInstance instance = GetInstance(collider.GetSceneID(), collider.GetID());

		if (instance.PhysicsEndContact.GetNativeMethodPtr())
		{
			MonoArray* monoUuidArr = ScriptMarshal::UUIDToMonoArray(collidee.GetID());
			void* args[] = { monoUuidArr };
			instance.PhysicsEndContact.Invoke(instance.Object, args);
		}
	}

	void ScriptEngine::OnPhysicsUpdate(Entity entity)
	{
		ScriptableInstance instance = GetInstance(entity.GetSceneID(), entity.GetID());

		if (instance.PhysicsUpdateMethod.GetNativeMethodPtr())
			instance.PhysicsUpdateMethod.Invoke(instance.Object, nullptr);
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
					{
						fieldBackup.Data.iValue = field.GetData<int8_t>();
						break;
					}
					case ScriptFieldType::Int16: 
					{
						fieldBackup.Data.iValue = field.GetData<int16_t>();
						break;
					}
					case ScriptFieldType::Int: 
					{
						fieldBackup.Data.iValue = field.GetData<int32_t>();
						break;
					}
					case ScriptFieldType::Int64: 
					{
						fieldBackup.Data.iValue = field.GetData<int64_t>();
						break;
					}
					case ScriptFieldType::UInt8: 
					{
						fieldBackup.Data.iValue = field.GetData<uint8_t>();
						break;
					}
					case ScriptFieldType::UInt16: 
					{
						fieldBackup.Data.iValue = field.GetData<uint16_t>();
						break;
					}
					case ScriptFieldType::UInt: 
					{
						fieldBackup.Data.iValue = field.GetData<uint32_t>();
						break;
					}
					case ScriptFieldType::UInt64:
					{
						fieldBackup.Data.iValue = field.GetData<uint64_t>();
						break;
					}
					case ScriptFieldType::Float:
					{
						fieldBackup.Data.dValue = field.GetData<float>();
						break;
					}
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

	static MonoAssembly* LoadAssembly(const std::filesystem::path& asseblyPath)
	{
		MonoAssembly* newAssembly = mono_domain_assembly_open(s_NewDomain, asseblyPath.string().c_str());

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
