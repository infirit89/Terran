#include "Core/EntryPoint.h"
#include "Terran.h"

#include "SandboxLayer.h"

namespace TerranEngine {

class SandboxApp : public Application {
public:
    SandboxApp(ApplicationData const& appData)
        : Application(appData)
    {
        // NOTE: this doesnt fuckinbg work anymore

        WindowData data;
        data.Name = "Sandbox";
        data.Width = 1080;
        data.Height = 790;

        // Create(data);

        PushLayer(new SandboxLayer());
    }

    ~SandboxApp()
    {
    }
};

Application* CreateApplication(int argc, char** argv)
{
    ApplicationData appData;
    appData.ScriptCorePath = "Resources/Scripts/TerranScriptCore.dll";
    return new SandboxApp(appData);
}

}

