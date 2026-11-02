#pragma once

#include <vector>
#include <Coral/Assembly.hpp>
#include <LibCore/Log.h>
#include <LibScript/ScriptEngine.h>
#include <LibScript/ScriptTypes.h>
#include <LibScript/bindings/BindingBase.h>
#include <cstdint>
#include <spdlog/common.h>
#include <string>

namespace Terran::Script {

class SceneBidning final {

public:
    static void bind(Coral::ManagedAssembly& assembly)
    {
        // BIND_INTERNAL_FUNC(LogBinding::Log_LogICall);
    }

private:
    // static bool SceneManager_LoadSceneICall(Coral::String scenePath)
    // {
    //
    // AssetInfo const& sceneAssetInfo = AssetManager::GetAssetInfoByPath(std::string(std::string(scenePath) + ".terran"));
    // Terran::Core::Shared<Scene> loadedScene = AssetManager::GetAssetByAssetInfo<Scene>(sceneAssetInfo);
    // if (!loadedScene)
    //     return false;
    //
    // SceneManager::GetCurrentScene()->StopRuntime();
    //
    // SceneManager::SetCurrentScene(Scene::CopyScene(loadedScene));
    // SceneManager::GetCurrentScene()->StartRuntime();
    //
    // return true;
    // }
};

}
