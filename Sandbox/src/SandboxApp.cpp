#include "Terran.h"
#include "Core/EntryPoint.h"

#include "SandboxLayer.h"

namespace TerranEngine 
{
	class SandboxApp : public Application 
	{
	public:
		SandboxApp() : Application()
		{
			WindowData data;
			data.Name = "Sandbox";
			data.Width = 1080;
			data.Height = 790;

			Create(data);

			PushLayer(new SandboxLayer());
		}

		~SandboxApp()
		{

		}
	};

	Application* CreateApplication() 
	{
		return new SandboxApp();
	}
}