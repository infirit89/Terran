#include "Application.h"
#include "Log.h"

#include <iostream>

namespace TerranEngine 
{
	Application::Application(std::string name)
		: m_Name(name)
	{
	}
	Application::~Application()
	{
	}

	void Application::Run()
	{
		TR_CORE_INFO("Test");
	}
}