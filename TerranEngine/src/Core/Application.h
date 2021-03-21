#pragma once

#include <string>

int main();

namespace TerranEngine {

	class Application
	{
	public:
		Application(std::string);
		virtual ~Application();
	private:
		void Run();
	private:
		std::string m_Name;
		friend int ::main();
	};

	Application* CreateApplication();
}

