#include "Terran.h"

namespace TerranEngine 
{
	class SandboxApp : public Application 
	{
	public:
		SandboxApp() : Application("Test")
		{
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