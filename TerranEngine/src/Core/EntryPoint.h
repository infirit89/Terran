#pragma once

#include "Application.h"
#include "Log.h"

extern TerranEngine::Application* TerranEngine::CreateApplication();

int main() 
{
	TerranEngine::Log::Init();

	auto application = TerranEngine::CreateApplication();

	application->Run();

	delete application;
}