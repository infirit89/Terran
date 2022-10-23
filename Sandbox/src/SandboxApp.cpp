#include "Terran.h"
#include "Core/EntryPoint.h"

#include "SandboxLayer.h"

namespace TerranEngine 
{
	class SandboxApp : public Application 
	{
	public:
		SandboxApp(const ApplicationData& appData) 
			: Application(appData)
		{
			// NOTE: this doesnt fuckinbg work anymore

			WindowData data;
			data.Name = "Sandbox";
			data.Width = 1080;
			data.Height = 790;

			//Create(data);

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