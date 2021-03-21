#pragma once

#include <string>

int main(char** argc, char** argv);

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
		friend int ::main(char** argc, char** argv);
	};

	Application* CreateApplication();
}

