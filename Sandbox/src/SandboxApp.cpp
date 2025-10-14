#include "LibMain/Application.h"
#include "SandboxLayer.h"
#include <string_view>

namespace TerranEngine {

class SandboxApp : public Terran::Main::Application {
public:
    SandboxApp()
    {

        push_layer(new SandboxLayer());
    }

    ~SandboxApp()
    {
    }
};

}

Terran::Main::Application* create_application(std::span<std::string_view> arguments)
{
    return new TerranEngine::SandboxApp();
}
