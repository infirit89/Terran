#pragma once

#include "Application.h"
#include "Log.h"
#include "Utils/Utils.h"

class Bas 
{
public:
	int GetH() { return h; }

private:
	int h;
};

class Dec : public Bas 
{

};

int main(int argc, char** argv) 
{
	TerranEngine::Log::Init();

	auto application = TerranEngine::CreateApplication(argc, argv);
	application->Run();
	delete application;

	return 0;
}
