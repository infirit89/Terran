#pragma once

#include "Application.h"
#include "Log.h"

int main(int argc, char** argv) 
{
	TerranEngine::Log::Init();

	auto application = TerranEngine::CreateApplication(argc, argv);
	application->Run();
	delete application;

	return 0;
}
