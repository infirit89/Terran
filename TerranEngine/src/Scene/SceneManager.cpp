#include "trpch.h"
#include "SceneManager.h"

#include "Core/Application.h"

#include "Events/SceneEvent.h"

namespace TerranEngine 
{
    Shared<Scene> SceneManager::s_CurrentScene;
    std::unordered_map<UUID, Shared<Scene>> SceneManager::s_ActiveScenes;

    Shared<Scene> SceneManager::CreateEmptyScene()
    {
        // TODO: create memory asset???
        Shared<Scene> scene = CreateShared<Scene>();
        s_ActiveScenes[scene->GetHandle()] = scene;
        return scene;
    }

    void SceneManager::RemoveScene(const UUID& id) 
    {
        if (s_ActiveScenes.contains(id))
            s_ActiveScenes.erase(id);

        if (s_CurrentScene->GetHandle() == id)
            s_CurrentScene = nullptr;
    }

    Shared<Scene> SceneManager::GetScene(const UUID& id) 
    {
        if (s_ActiveScenes.contains(id))
            return s_ActiveScenes.at(id);

        return nullptr;
    }

    void SceneManager::SetCurrentScene(Shared<Scene> newScene)
    {
        UUID id({ 0 });
        if(s_CurrentScene)
            id = s_CurrentScene->GetHandle();

        SceneTransitionEvent sceneTransitionEvent(s_CurrentScene, newScene);
        Application::Get()->DispatchEvent(sceneTransitionEvent);

        s_CurrentScene = newScene;
        s_ActiveScenes[newScene->GetHandle()] = newScene;

        if (!id)
            return;

        if (s_ActiveScenes.contains(id)) 
        {
            if (s_ActiveScenes[id].use_count() > 1)
                return;

            s_ActiveScenes.erase(id);
        }
    }
}
