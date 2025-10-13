#pragma once

#include "Application.h"
#include "LibCore/Log.h"
#include "Utils/Utils.h"

int main(int argc, char** argv) 
{
    Terran::Core::Log::Init();

	auto application = TerranEngine::CreateApplication(argc, argv);
	application->Run();
	delete application;

    Terran::Core::Log::Shutdown();

	return 0;
}
