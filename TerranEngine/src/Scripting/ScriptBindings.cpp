#include "trpch.h"
#include "ScriptBindings.h"

#include "ScriptString.h"
#include "ScriptingEngine.h"

#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Scene/SceneManager.h"

namespace TerranEngine 
{
    void ScriptBindings::Bind()
    {
        ScriptingEngine::BindInternalFunc("TerranScriptCore.Entity::HasComponent_Internal", HasComponent_Internal);
        ScriptingEngine::BindInternalFunc("TerranScriptCore.Entity::AddComponent_Internal", AddComponent_Internal);
        ScriptingEngine::BindInternalFunc("TerranScriptCore.Entity::RemoveComponent_Internal", RemoveComponent_Internal);

        ScriptingEngine::BindInternalFunc("TerranScriptCore.Entity::FindEntityWithName_Internal", FindEntityWithName_Internal);

        ScriptingEngine::BindInternalFunc("TerranScriptCore.Transform::GetTransformPosition_Internal", GetTransformPosition_Internal);
        ScriptingEngine::BindInternalFunc("TerranScriptCore.Transform::SetTransformPosition_Internal",
            SetTransformPosition_Internal);

        ScriptingEngine::BindInternalFunc("TerranScriptCore.Tag::SetTagName_Internal", SetTagName_Internal);
        ScriptingEngine::BindInternalFunc("TerranScriptCore.Tag::GetTagName_Internal", GetTagName_Internal);


        ScriptingEngine::BindInternalFunc("TerranScriptCore.Log::Log_Internal", Log_Internal);
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

    bool ScriptBindings::HasComponent_Internal(uint32_t entityRuntimeID, MonoString* componentTypeStr)
    {
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
        case ComponentType::ScriptableComponent:    return entity.HasComponent<ScriptableComponent>();
        }


        TR_ERROR("No component with the corresponding type exists");
        return false;
    }
    void ScriptBindings::AddComponent_Internal(uint32_t entityRuntimeID, MonoString* componentTypeStr)
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
        case ComponentType::ScriptableComponent:        entity.AddComponent<ScriptableComponent>(); break;
        }
    }

    void ScriptBindings::RemoveComponent_Internal(uint32_t entityRuntimeID, MonoString* componentTypeStr)
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
        case ComponentType::ScriptableComponent:    entity.RemoveComponent<ScriptableComponent>(); break;
        }
    }

    uint32_t ScriptBindings::FindEntityWithName_Internal(MonoString* monoName)
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
    void ScriptBindings::SetTransformPosition_Internal(uint32_t entityRuntimeID, glm::vec3 inPosition)
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

    glm::vec3 ScriptBindings::GetTransformPosition_Internal(uint32_t entityRuntimeID)
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

    void ScriptBindings::SetTransformRotation_Internal(uint32_t entityRuntimeID, glm::vec3 Rotation)
    {
        SET_COMPONENT_VAR(Rotation, entityRuntimeID, TransformComponent);
        entity.GetComponent<TransformComponent>().IsDirty = true;
    }

    glm::vec3 ScriptBindings::GetTransformRotation_Internal(uint32_t entityRuntimeID)
    {
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        GET_COMPONENT_VAR(Rotation, entityRuntimeID, TransformComponent);

        return Rotation;
    }

    void ScriptBindings::SetTransformScale_Internal(uint32_t entityRuntimeID, glm::vec3 Scale)
    {
        SET_COMPONENT_VAR(Scale, entityRuntimeID, TransformComponent);
        entity.GetComponent<TransformComponent>().IsDirty = true;
    }

    glm::vec3 ScriptBindings::GetTransformScale_Internal(uint32_t entityRuntimeID)
    {
        glm::vec3 Scale = { 0.0f, 0.0f, 0.0f };
        GET_COMPONENT_VAR(Scale, entityRuntimeID, TransformComponent);

        return Scale;
    }
    // -------------------

    void ScriptBindings::SetTagName_Internal(uint32_t entityRuntimeID, MonoString* inName) 
    {
        ScriptString nameStr(inName);
        const char* Name = nameStr.GetUTF8Str();

        SET_COMPONENT_VAR(Name, entityRuntimeID, TagComponent);
    }

    MonoString* ScriptBindings::GetTagName_Internal(uint32_t entityRuntimeID)
    {
        std::string Name = "";
        GET_COMPONENT_VAR(Name, entityRuntimeID, TagComponent);
        return ScriptString(Name.c_str()).GetStringInternal();
    }

    void ScriptBindings::Log_Internal(uint8_t logLevel, MonoString* monoMessage)
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
