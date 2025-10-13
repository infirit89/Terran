#include "trpch.h"
#include "SceneManager.h"

#include "Core/Application.h"

#include "Events/SceneEvent.h"

namespace TerranEngine {

Terran::Core::Shared<Scene> SceneManager::s_CurrentScene;
std::unordered_map<Terran::Core::UUID, Terran::Core::Shared<Scene>> SceneManager::s_ActiveScenes;

Terran::Core::Shared<Scene> SceneManager::CreateEmptyScene()
{
    // TODO: create memory asset???
    Terran::Core::Shared<Scene> scene = Terran::Core::CreateShared<Scene>();
    s_ActiveScenes[scene->GetHandle()] = scene;
    return scene;
}

void SceneManager::RemoveScene(const Terran::Core::UUID& id)
{
    if (s_ActiveScenes.contains(id))
        s_ActiveScenes.erase(id);

    if (s_CurrentScene->GetHandle() == id)
        s_CurrentScene = nullptr;
}

Terran::Core::Shared<Scene> SceneManager::GetScene(const Terran::Core::UUID& id)
{
    if (s_ActiveScenes.contains(id))
        return s_ActiveScenes.at(id);

    return nullptr;
}

void SceneManager::SetCurrentScene(Terran::Core::Shared<Scene> newScene)
{
    Terran::Core::UUID id({ 0 });
    if (s_CurrentScene)
        id = s_CurrentScene->GetHandle();

    SceneTransitionEvent sceneTransitionEvent(s_CurrentScene, newScene);
    Application::Get()->DispatchEvent(sceneTransitionEvent);

    s_CurrentScene = newScene;
    s_ActiveScenes[newScene->GetHandle()] = newScene;

    if (!id)
        return;

    if (s_ActiveScenes.contains(id)) {
        if (s_ActiveScenes[id].use_count() > 1)
            return;

        s_ActiveScenes.erase(id);
    }
}

}
