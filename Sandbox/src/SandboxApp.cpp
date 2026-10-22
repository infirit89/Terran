#include "SandboxLayer.h"

#include <LibAsset/AssetManager.h>
#include <LibCore/Assert.h>
#include <LibMain/Application.h>
#include <LibScene/SceneManager.h>
#include <LibScript/ScriptEngine.h>
#include <LibWindow/WindowSystem.h>

#include <span>
#include <string_view>

namespace TerranEngine {

class SandboxApp : public Terran::Main::Application {
public:
    SandboxApp()
    {
        auto windowSystemRes = layer_stack().push<Terran::Window::WindowSystem>();
        TR_ASSERT(windowSystemRes.is_ok(), "Failed to start the window system");

        auto assetManagerRes = layer_stack().push<Terran::Asset::AssetManager>();
        TR_ASSERT(assetManagerRes.is_ok(), "Failed to start the asset manager");

        auto sceneManagerRes = layer_stack().push<Terran::World::SceneManager>(
            layer_stack().get<Terran::Asset::AssetManager>()
        );
        TR_ASSERT(sceneManagerRes.is_ok(), "Failed to start the scene manager");

        auto script_system_res =  layer_stack().push<Terran::Script::ScriptEngine>("Resources/Scripts/Terran.Core.dll");
        TR_ASSERT(script_system_res.is_ok(), "Failed to start the script system");

        layer_stack().push<SandboxLayer>(
            layer_stack().get<Terran::Window::WindowSystem>(),
            layer_stack().get<Terran::World::SceneManager>(),
            layer_stack().get<Terran::Script::ScriptEngine>());

    }

    virtual ~SandboxApp() override = default;
};

}

Terran::Main::Application* create_application(std::span<std::string_view>)
{
    return new TerranEngine::SandboxApp();
}
