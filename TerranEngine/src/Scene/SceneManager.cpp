#include "trpch.h"
#include "SceneManager.h"

namespace TerranEngine 
{
    Shared<Scene> SceneManager::s_CurrentScene;
    std::unordered_map<UUID, Shared<Scene>> SceneManager::s_ActiveScenes;

    Shared<Scene> SceneManager::CreateEmpyScene()
    {
        Shared<Scene> scene = CreateShared<Scene>();
        s_ActiveScenes[scene->GetID()] = scene;
        return scene;
    }

    void SceneManager::RemoveScene(const UUID& id) 
    {
        if (s_ActiveScenes.find(id) != s_ActiveScenes.end())
            s_ActiveScenes.erase(id);

        if (s_CurrentScene->GetID() == id)
            s_CurrentScene = nullptr;
    }

    Shared<Scene> SceneManager::GetScene(const UUID& id) 
    {
        if (s_ActiveScenes.find(id) != s_ActiveScenes.end())
            return s_ActiveScenes[id];

        return nullptr;
    }

    void SceneManager::SetCurrentScene(const Shared<Scene>& scene)
    {
        UUID id({ 0 });
        if(s_CurrentScene)
            id = s_CurrentScene->GetID();

        s_CurrentScene = scene;

        if (!id)
            return;

        if (s_ActiveScenes.find(id) != s_ActiveScenes.end()) 
        {
            if (s_ActiveScenes[id].use_count() > 1)
                return;

            s_ActiveScenes.erase(id);
        }
    }
}
