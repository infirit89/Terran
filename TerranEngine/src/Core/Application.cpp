#include "Application.h"

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
		std::cout << m_Name << std::endl;
	}
}