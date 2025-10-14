#include "Core/EntryPoint.h"
#include "Terran.h"

#include "SandboxLayer.h"

namespace TerranEngine {

class SandboxApp : public Application {
public:
    SandboxApp(ApplicationData const& appData)
        : Application(appData)
    {

        PushLayer(new SandboxLayer());
    }

    ~SandboxApp()
    {
    }
};

Application* CreateApplication(int argc, char** argv)
{
    ApplicationData appData;
    return new SandboxApp(appData);
}

}

