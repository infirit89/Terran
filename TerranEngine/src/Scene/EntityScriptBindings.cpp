#include "trpch.h"
#include "EntityScriptBindings.h"

#include "Scripting/ScriptString.h"
#include "Scripting/ScriptingEngine.h"

#include "SceneManager.h"
#include "Entity.h"
#include "Components.h"

namespace TerranEngine 
{
    enum class ComponentType 
    {
        None = 0,
        TransformComponent
    };

    static Scene* GetScenePtr() { return SceneManager::GetCurrentScene().get(); }

    void EntityBindings::Bind()
    {
        ScriptingEngine::BindInternalFunc("TerranScriptCore.Entity::HasComponent_Internal", HasComponent_Internal);

        ScriptingEngine::BindInternalFunc("TerranScriptCore.Entity::AddComponent_Internal", AddComponent_Internal);
        ScriptingEngine::BindInternalFunc("TerranScriptCore.Entity::RemoveComponent_Internal", RemoveComponent_Internal);

        ScriptingEngine::BindInternalFunc("TerranScriptCore.Entity::FindEntityWithName_Internal", FindEntityWithName_Internal);
    }

    static ComponentType GetComponentType(MonoString* componentTypeStr)
    {
        ScriptString string(componentTypeStr);

        if (strcmp(string.GetUTF8Str(), "TerranScriptCore.Transform") == 0) 
            return ComponentType::TransformComponent;

        return ComponentType::None;
    }

    bool EntityBindings::HasComponent_Internal(uint32_t entityRuntimeID, MonoString* componentTypeStr)
    {
        ComponentType type = GetComponentType(componentTypeStr);

		TR_TRACE("Entity ID: {0}", entityRuntimeID);

        if (!GetScenePtr()) 
        {
            TR_ERROR("No active scene loaded");
            return false;
        }

        Entity entity((entt::entity)entityRuntimeID, GetScenePtr());

        switch (type)
        {
        case ComponentType::TransformComponent:     return entity.HasComponent<TransformComponent>();
        }


        TR_ERROR("No component with the corresponding type exists");
        return false;
    }
    void EntityBindings::AddComponent_Internal(uint32_t entityRuntimeID, MonoString* componentTypeStr)
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
        case ComponentType::TransformComponent: entity.AddComponent<TransformComponent>(); break;
		}
    }

    void EntityBindings::RemoveComponent_Internal(uint32_t entityRuntimeID, MonoString* componentTypeStr)
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
		case ComponentType::TransformComponent: entity.RemoveComponent<TransformComponent>(); break;
		}
    }

    uint32_t EntityBindings::FindEntityWithName_Internal(MonoString* monoName)
    {
        ScriptString name(monoName);
        return (uint32_t)(SceneManager::GetCurrentScene()->FindEntityWithName(name.GetUTF8Str()));
    }
}

