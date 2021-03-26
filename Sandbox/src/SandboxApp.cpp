#include "Terran.h"
#include "Core/EntryPoint.h"

#include "SandboxLayer.h"

namespace TerranEngine 
{
	class SandboxApp : public Application 
	{
	public:
		SandboxApp() : Application("Test", {  })
		{
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