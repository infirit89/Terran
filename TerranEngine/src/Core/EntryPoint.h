#pragma once
#include "Application.h"

int main(char** argc, char** argv) 
{
	auto application = TerranEngine::CreateApplication();

	application->Run();

	delete application;
}