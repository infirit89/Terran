#include "trpch.h"
#include "ScriptEngine.h"

#include "ScriptBindings.h"
#include "ScriptMarshal.h"
#include "GCManager.h"
#include "ScriptCache.h"
#include "ScriptMethodThunks.h"
#include "ScriptAssembly.h"

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
	struct ScriptEngineData 
	{
		MonoDomain* CoreDomain;
		MonoDomain* NewDomain;

		MonoAssembly* CurrentAssembly;

		MonoImage* CurrentImage;

		std::filesystem::path AssemblyPath;
		std::filesystem::path MonoPath = "mono";

		std::filesystem::path LibPath = MonoPath / "lib";
		std::filesystem::path EtcPath = MonoPath / "etc";

		std::filesystem::path MonoConfigPath = EtcPath / "config";

		std::unordered_map<uint32_t, ScriptClass> Classes;
	};

	static MonoAssembly* LoadAssembly(const std::filesystem::path& asseblyPath);
	static MonoImage* GetImageFromAssemly(MonoAssembly* assembly);

	static void OnLogMono(const char* log_domain, const char* log_level, const char* message, mono_bool fatal, void* user_data);

	static ScriptMethod GetMethodFromImage(MonoImage* image, const char* methodSignature);
	
	static ScriptEngineData* s_ScriptEngineData;

	struct ScriptableInstance 
	{
		GCHandle ObjectHandle;
		ScriptMethodThunks<MonoArray*> Constructor;
		ScriptMethodThunks<> InitMethod;
		ScriptMethodThunks<> UpdateMethod;
		
		ScriptMethodThunks<MonoArray*> PhysicsBeginContact;
		ScriptMethodThunks<MonoArray*> PhysicsEndContact;

		ScriptMethodThunks<> PhysicsUpdateMethod;

		void GetMethods(ScriptClass& scriptClass) 
		{
			// TODO: return ScriptMethod*
			Constructor.SetFromMethod(GetMethodFromImage(s_ScriptEngineData->CurrentImage, "Terran.Scriptable:.ctor(byte[])"));

			InitMethod.SetFromMethod(scriptClass.GetMethod(":Init()"));
			UpdateMethod.SetFromMethod(scriptClass.GetMethod(":Update()"));

			PhysicsBeginContact.SetFromMethod(scriptClass.GetMethod(":OnCollisionBegin(Entity)"));
			PhysicsEndContact.SetFromMethod(scriptClass.GetMethod(":OnCollisionEnd(Entity)"));

			PhysicsUpdateMethod.SetFromMethod(scriptClass.GetMethod(":PhysicsUpdate()"));
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

	static Shared<AssemblyInfo> GenerateAssemblyInfo() 
	{
		Shared<AssemblyInfo> info = CreateShared<AssemblyInfo>();

		const MonoTableInfo* tableInfo = mono_image_get_table_info(s_ScriptEngineData->CurrentImage, MONO_TABLE_TYPEDEF);
		int rows = mono_table_info_get_rows(tableInfo);

		for (size_t i = 0; i < rows; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(tableInfo, i, cols, MONO_TYPEDEF_SIZE);

			std::string namespaceName = mono_metadata_string_heap(s_ScriptEngineData->CurrentImage, cols[MONO_TYPEDEF_NAMESPACE]);
			std::string className = mono_metadata_string_heap(s_ScriptEngineData->CurrentImage, cols[MONO_TYPEDEF_NAME]);
		

			info->ClassInfoMap[namespaceName].emplace_back(std::move(className));
		}

		info->CurrentImage = s_ScriptEngineData->CurrentImage;

		return info;
	}

	void ScriptEngine::Initialize(const std::filesystem::path& asseblyPath)
	{
		s_ScriptEngineData = new ScriptEngineData();
		s_ScriptEngineData->AssemblyPath = asseblyPath;
		
		mono_set_dirs(s_ScriptEngineData->LibPath.string().c_str(), s_ScriptEngineData->EtcPath.string().c_str());

#ifdef TR_DEBUG
		mono_debug_init(MONO_DEBUG_FORMAT_MONO);
		mono_trace_set_level_string("debug");
		mono_trace_set_log_handler(OnLogMono, nullptr);
#endif

		mono_config_parse(s_ScriptEngineData->MonoConfigPath.string().c_str());

		s_ScriptEngineData->CoreDomain = mono_jit_init("CoreDomain");
		
		if (!s_ScriptEngineData->CoreDomain)
		{
			TR_ERROR("Couldn't initialize the Mono Domain!");
			return;
		}

		NewDomain();

		Shared<AssemblyInfo> info = GenerateAssemblyInfo();
		ScriptCache::CacheClassesFromAssemblyInfo(info);

		/*const MonoTableInfo* tableInfo = mono_image_get_table_info(s_CurrentImage, MONO_TABLE_FIELDRVA);
		int rows = mono_table_info_get_rows(tableInfo);

		for (size_t i = 0; i < rows; i++)
		{
			uint32_t cols[MONO_FIELD_RVA_SIZE];
			mono_metadata_decode_row(tableInfo, i, cols, MONO_FIELD_RVA_SIZE);

			const char* className = mono_metadata_string_heap(s_CurrentImage, cols[MONO_FIELD_RVA_RVA]);
			const char* namespaceName = mono_metadata_string_heap(s_CurrentImage, cols[MONO_FIELD_RVA_FIELD]);
			TR_TRACE("Class: {0}, Namespace: {1}", className, namespaceName);
		}*/
	}

	void ScriptEngine::Shutdown()
	{
		mono_jit_cleanup(s_ScriptEngineData->CoreDomain);
		delete s_ScriptEngineData;
		TR_INFO("Deinitialized the scripting core");
	}

	void ScriptEngine::NewDomain() 
	{
		s_ScriptEngineData->NewDomain = mono_domain_create_appdomain("ScriptAssemblyDomain", NULL);
		
		if (!mono_domain_set(s_ScriptEngineData->NewDomain, false))
		{
			TR_ERROR("Couldn't set the new domain");
			return;
		}

		s_ScriptEngineData->CurrentAssembly = LoadAssembly(s_ScriptEngineData->AssemblyPath);

		if (!s_ScriptEngineData->CurrentAssembly) return;

		s_ScriptEngineData->CurrentImage = GetImageFromAssemly(s_ScriptEngineData->CurrentAssembly);

		if (!s_ScriptEngineData->CurrentImage) return;

		TR_INFO("Successfuly loaded: {0}", s_ScriptEngineData->AssemblyPath);

		ScriptCache::CacheCoreClasses();
		ScriptBindings::Bind();
	}

	std::filesystem::path ScriptEngine::GetAssemblyPath()
	{
		return s_ScriptEngineData->AssemblyPath;
	}

	void ScriptEngine::UnloadDomain() 
	{
		if (s_ScriptEngineData->NewDomain != s_ScriptEngineData->CoreDomain)
		{
			SetCurrentFieldStates(SceneManager::GetCurrentScene()->GetID());

			mono_domain_set(s_ScriptEngineData->CoreDomain, false);

			if (!mono_domain_finalize(s_ScriptEngineData->NewDomain, 2000))
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

			mono_domain_unload(s_ScriptEngineData->NewDomain);

			s_ScriptEngineData->Classes.clear();
		}

		ScriptCache::ClearClassCache();
	}

	ScriptClass ScriptEngine::GetClass(const std::string& moduleName)
	{
		if (!s_ScriptEngineData->CurrentImage)
		{
			TR_ERROR("Can't locate the class {0}, as there is no loaded script image", moduleName);
			return ScriptClass();
		}

		std::hash<std::string> hasher;
		uint32_t hashedName = hasher(moduleName);

		if (s_ScriptEngineData->Classes.find(hashedName) != s_ScriptEngineData->Classes.end())
			return s_ScriptEngineData->Classes[hashedName];

		size_t dotPosition = moduleName.find_last_of(".");

		std::string& namespaceName = moduleName.substr(0, dotPosition);
		std::string& className = moduleName.substr(dotPosition + 1);
		
		// NOTE: Access violation when the script assembly isn't loaded
		MonoClass* klass = mono_class_from_name(s_ScriptEngineData->CurrentImage, namespaceName.c_str(), className.c_str());

		if (!klass) 
		{
			TR_ERROR("Class wasn't found");
			return ScriptClass();
		}

		ScriptClass scriptClass(klass);
		s_ScriptEngineData->Classes[hashedName] = scriptClass;

		return scriptClass;
	}

	bool ScriptEngine::ClassExists(const std::string& moduleName)
	{
		return ScriptCache::GetCachedClassFromName(moduleName);
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
			ScriptClass* klass = ScriptCache::GetCachedClassFromName(scriptComponent.ModuleName);
			
			if (!klass) return;

			if (klass->IsInstanceOf(TR_API_CACHED_CLASS(Scriptable)))
			{
				// TODO: display error in ui
				TR_WARN("Class {0} doesn't extend Scriptable", scriptComponent.ModuleName);
				return;
			}

			ScriptableInstance instance;
			instance.ObjectHandle = GCManager::CreateStrongHadle(ScriptObject::CreateInstace(*klass));
			instance.GetMethods(*klass);

			MonoArray* uuidArray = ScriptMarshal::UUIDToMonoArray(entity.GetID());

			void* args[] = { uuidArray };

			MonoException* exc = nullptr;
			ScriptObject object = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.Constructor.Invoke(object.GetMonoObject(), uuidArray, &exc);

			s_ScriptableInstanceMap[entity.GetSceneID()][entity.GetID()] = instance;

			if (!s_ScriptFieldBackup.empty()) 
			{
				std::unordered_map<std::string, ScriptFieldBackup> fieldBackup = s_ScriptFieldBackup.at(entity.GetID());
				for (auto& [fieldName, field] : fieldBackup)
				{
					std::hash<std::string> hasher;
					uint32_t hashedName = hasher(fieldName);
					if (object.GetFieldMap().find(hashedName) != object.GetFieldMap().end())
					{
						ScriptField& objectField = object.GetFieldMap().at(hashedName);

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

			scriptComponent.FieldOrder = object.GetFieldOrder();
			scriptComponent.PublicFields = object.GetFieldMap();
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
				GCHandle& handle = s_ScriptableInstanceMap[entity.GetSceneID()][entity.GetID()].ObjectHandle;
				GCManager::FreeHandle(handle);
				s_ScriptableInstanceMap[entity.GetSceneID()].erase(entity.GetID());
			}
		}
	}

	void ScriptEngine::OnStart(Entity entity) 
	{
		ScriptableInstance instance = GetInstance(entity.GetSceneID(), entity.GetID());

		entity.GetComponent<ScriptComponent>().Started = true;

		if (instance.InitMethod)
		{
			MonoException* exc = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.InitMethod.Invoke(monoObject, &exc);
		}
	}

	void ScriptEngine::OnUpdate(Entity entity) 
	{
		ScriptableInstance instance = GetInstance(entity.GetSceneID(), entity.GetID());

		if (instance.UpdateMethod) 
		{
			MonoException* exc = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.UpdateMethod.Invoke(monoObject, &exc);
		}
	}

	void ScriptEngine::OnPhysicsBeginContact(Entity collider, Entity collidee)
	{
		ScriptableInstance instance = GetInstance(collider.GetSceneID(), collider.GetID());

		if (instance.PhysicsBeginContact) 
		{
			MonoArray* uuidArr = ScriptMarshal::UUIDToMonoArray(collidee.GetID());
			//void* args[] = { monoUuidArr };
			MonoException* exc = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.PhysicsBeginContact.Invoke(monoObject, uuidArr, &exc);
		}
	}

	void ScriptEngine::OnPhysicsEndContact(Entity collider, Entity collidee)
	{
		ScriptableInstance instance = GetInstance(collider.GetSceneID(), collider.GetID());

		if (instance.PhysicsEndContact)
		{
			MonoArray* uuidArr = ScriptMarshal::UUIDToMonoArray(collidee.GetID());
			//void* args[] = { monoUuidArr };
			MonoException* exc = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.PhysicsBeginContact.Invoke(monoObject, uuidArr, &exc);
		}
	}

	void ScriptEngine::OnPhysicsUpdate(Entity entity)
	{
		ScriptableInstance instance = GetInstance(entity.GetSceneID(), entity.GetID());

		if (instance.PhysicsUpdateMethod) 
		{
			MonoException* exc = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.PhysicsUpdateMethod.Invoke(monoObject, &exc);
		}
	}

	ScriptObject ScriptEngine::GetScriptInstanceScriptObject(const UUID& sceneUUID, const UUID& entityUUID)
	{
		ScriptableInstance instance = GetInstance(sceneUUID, entityUUID);

		return GCManager::GetManagedObject(instance.ObjectHandle);
	}

	GCHandle ScriptEngine::GetScriptInstanceGCHandle(const UUID& sceneUUID, const UUID& entityUUID)
	{
		ScriptableInstance instance = GetInstance(sceneUUID, entityUUID);
		return instance.ObjectHandle;
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
				ScriptObject object = GCManager::GetManagedObject(GetInstance(sceneID, entityID).ObjectHandle);

				for (auto& [fieldName, field] : object.GetFieldMap())
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
		MonoAssembly* newAssembly = mono_domain_assembly_open(s_ScriptEngineData->NewDomain, asseblyPath.string().c_str());

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
