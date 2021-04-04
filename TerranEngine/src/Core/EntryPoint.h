#pragma once

#include "Application.h"
#include "Log.h"

int main() 
{
	TerranEngine::Log::Init();

	auto application = TerranEngine::CreateApplication();

	application->Run();

	delete application;
}