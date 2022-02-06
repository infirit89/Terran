#include "trpch.h"
#include "ScriptBindings.h"

#include "ScriptString.h"
#include "ScriptEngine.h"

#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Scene/SceneManager.h"

#include "glm/glm.hpp"

#include <mono/jit/jit.h>

namespace TerranEngine 
{
    // ---- Entity ----
    static bool HasComponent_Internal(uint32_t entityRuntimeID, MonoString* componentType);
    static void AddComponent_Internal(uint32_t entityRuntimeID, MonoString* componentType);
    static void RemoveComponent_Internal(uint32_t entityRuntimeID, MonoString* componentType);

    static uint32_t FindEntityWithName_Internal(MonoString* monoName);
    // ----------------

    // ---- Transform component ----
    static void SetTransformPosition_Internal(uint32_t entityRuntimeID, glm::vec3 inPosition);
    static glm::vec3 GetTransformPosition_Internal(uint32_t entityRuntimeID);

    static void SetTransformRotation_Internal(uint32_t entityRuntimeID, glm::vec3 inRotation);
    static glm::vec3 GetTransformRotation_Internal(uint32_t entityRuntimeID);

    static void SetTransformScale_Internal(uint32_t entityRuntimeID, glm::vec3 inScale);
    static glm::vec3 GetTransformScale_Internal(uint32_t entityRuntimeID);
    // -----------------------------

    // ---- Tag component ----
    static void SetTagName_Internal(uint32_t entityRuntimeID, MonoString* inName);
    static MonoString* GetTagName_Internal(uint32_t entityRuntimeID);
    // -----------------------

    // ---- Utils ----
    static void Log_Internal(uint8_t logLevel, MonoString* monoMessage);
    // ---------------

    template <typename Func>
    static void BindInternalFunc(const char* funcName, Func func)
    {
        mono_add_internal_call(funcName, func);
    }

    void ScriptBindings::Bind()
    {
        BindInternalFunc("TerranScriptCore.Entity::HasComponent_Internal", HasComponent_Internal);
        BindInternalFunc("TerranScriptCore.Entity::AddComponent_Internal", AddComponent_Internal);
        BindInternalFunc("TerranScriptCore.Entity::RemoveComponent_Internal", RemoveComponent_Internal);

        BindInternalFunc("TerranScriptCore.Entity::FindEntityWithName_Internal", FindEntityWithName_Internal);

        BindInternalFunc("TerranScriptCore.Transform::GetTransformPosition_Internal", GetTransformPosition_Internal);
        BindInternalFunc("TerranScriptCore.Transform::SetTransformPosition_Internal",
            SetTransformPosition_Internal);

        BindInternalFunc("TerranScriptCore.Tag::SetTagName_Internal", SetTagName_Internal);
        BindInternalFunc("TerranScriptCore.Tag::GetTagName_Internal", GetTagName_Internal);


        BindInternalFunc("TerranScriptCore.Log::Log_Internal", Log_Internal);
    }

    enum class ComponentType
    {
        None = 0,
        TransformComponent,
        TagComponent,
        ScriptableComponent,
    };

    static Scene* GetScenePtr() { return SceneManager::GetCurrentScene().get(); }

    static ComponentType GetComponentType(MonoString* componentTypeStr)
    {
        ScriptString string(componentTypeStr);

        if (strcmp(string.GetUTF8Str(), "TerranScriptCore.Transform") == 0)
            return ComponentType::TransformComponent;
        else if (strcmp(string.GetUTF8Str(), "TerranScriptCore.Tag") == 0)
            return ComponentType::TagComponent;
        else if (strcmp(string.GetUTF8Str(), "TerranScriptCore.Scriptable") == 0)
            return ComponentType::ScriptableComponent;

        return ComponentType::None;
    }

    static bool HasComponent_Internal(uint32_t entityRuntimeID, MonoString* componentTypeStr)
    {
        TR_TRACE("get component");

        ComponentType type = GetComponentType(componentTypeStr);

        if (!GetScenePtr())
        {
            TR_ERROR("No active scene loaded");
            return false;
        }

        Entity entity((entt::entity)entityRuntimeID, GetScenePtr());

        switch (type)
        {
        case ComponentType::TransformComponent:     return entity.HasComponent<TransformComponent>();
        case ComponentType::TagComponent:           return entity.HasComponent<TagComponent>();
        case ComponentType::ScriptableComponent:    return entity.HasComponent<ScriptComponent>();
        }


        TR_ERROR("No component with the corresponding type exists");
        return false;
    }

    static void AddComponent_Internal(uint32_t entityRuntimeID, MonoString* componentTypeStr)
    {
        ComponentType type = GetComponentType(componentTypeStr);

        if (!GetScenePtr())
        {
            TR_ERROR("No active scene loaded");
            return;
        }

        Entity entity((entt::entity)entityRuntimeID, GetScenePtr());

        switch (type)
        {
        case ComponentType::TransformComponent:         entity.AddComponent<TransformComponent>(); break;
        case ComponentType::TagComponent:               entity.AddComponent<TagComponent>(); break;
        case ComponentType::ScriptableComponent:        entity.AddComponent<ScriptComponent>(); break;
        }
    }

    static void RemoveComponent_Internal(uint32_t entityRuntimeID, MonoString* componentTypeStr)
    {
        ComponentType type = GetComponentType(componentTypeStr);

        if (!GetScenePtr())
        {
            TR_ERROR("No active scene loaded");
            return;
        }

        Entity entity((entt::entity)entityRuntimeID, GetScenePtr());

        switch (type)
        {
        case ComponentType::TransformComponent:     entity.RemoveComponent<TransformComponent>(); break;
        case ComponentType::TagComponent:           entity.RemoveComponent<TagComponent>(); break;
        case ComponentType::ScriptableComponent:    entity.RemoveComponent<ScriptComponent>(); break;
        }
    }

    static uint32_t FindEntityWithName_Internal(MonoString* monoName)
    {
        ScriptString name(monoName);
        return (uint32_t)(SceneManager::GetCurrentScene()->FindEntityWithName(name.GetUTF8Str()));
    }

// bullshit?
#define SET_COMPONENT_VAR(var, entityID, componentType)\
	Entity entity((entt::entity)entityID, SceneManager::GetCurrentScene().get());\
	if(entity)\
		entity.GetComponent<componentType>().var = var;\
	else\
		TR_ERROR("Invalid entity id");

// bullshit #2? 
#define GET_COMPONENT_VAR(var, entityID, componentType)\
	Entity entity((entt::entity)entityID, SceneManager::GetCurrentScene().get());\
	if(entity)\
		var = entity.GetComponent<componentType>().var;\
	else\
		TR_ERROR("Invalid entity id");

    // ---- Transform ----
    static void SetTransformPosition_Internal(uint32_t entityRuntimeID, glm::vec3 inPosition)
    {
        Entity entity((entt::entity)entityRuntimeID, SceneManager::GetCurrentScene().get());
        if (entity) 
        {
            entity.GetComponent<TransformComponent>().Position = inPosition; \
            entity.GetComponent<TransformComponent>().IsDirty = true;
        }
        else
            TR_ERROR("Invalid entity id");
    }

    static glm::vec3 GetTransformPosition_Internal(uint32_t entityRuntimeID)
    {
        Entity entity((entt::entity)entityRuntimeID, SceneManager::GetCurrentScene().get());
        if (entity) 
        {
            glm::vec3 pos = entity.GetComponent<TransformComponent>().Position;
            return pos;
        }
        else
            TR_ERROR("Invalid entity id");

        return { 0.0f, 0.0f, 0.0f };
    }

    static void SetTransformRotation_Internal(uint32_t entityRuntimeID, glm::vec3 Rotation)
    {
        SET_COMPONENT_VAR(Rotation, entityRuntimeID, TransformComponent);
        entity.GetComponent<TransformComponent>().IsDirty = true;
    }

    static glm::vec3 GetTransformRotation_Internal(uint32_t entityRuntimeID)
    {
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        GET_COMPONENT_VAR(Rotation, entityRuntimeID, TransformComponent);

        return Rotation;
    }

    static void SetTransformScale_Internal(uint32_t entityRuntimeID, glm::vec3 Scale)
    {
        SET_COMPONENT_VAR(Scale, entityRuntimeID, TransformComponent);
        entity.GetComponent<TransformComponent>().IsDirty = true;
    }

    static glm::vec3 GetTransformScale_Internal(uint32_t entityRuntimeID)
    {
        glm::vec3 Scale = { 0.0f, 0.0f, 0.0f };
        GET_COMPONENT_VAR(Scale, entityRuntimeID, TransformComponent);

        return Scale;
    }
    // -------------------

    static void SetTagName_Internal(uint32_t entityRuntimeID, MonoString* inName) 
    {
        ScriptString nameStr(inName);
        const char* Name = nameStr.GetUTF8Str();

        SET_COMPONENT_VAR(Name, entityRuntimeID, TagComponent);
    }

    static MonoString* GetTagName_Internal(uint32_t entityRuntimeID)
    {
        std::string Name = "";
        GET_COMPONENT_VAR(Name, entityRuntimeID, TagComponent);
        return ScriptString(Name.c_str()).GetStringInternal();
    }

    static void Log_Internal(uint8_t logLevel, MonoString* monoMessage)
    {
        // TODO: for now it logs to the console,
        // should make it log to a ui debug console inside the editor
        ScriptString message(monoMessage);

        switch (logLevel)
        {
        case 1 << 0:
            TR_TRACE(message.GetUTF8Str());
            break;
        case 1 << 1:
            TR_WARN(message.GetUTF8Str());
            break;
        case 1 << 2:
            TR_ERROR(message.GetUTF8Str());
            break;
        }
    }
}
